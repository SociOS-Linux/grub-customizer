/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "Settings.h"

View_Gtk_Settings_ColorChooser::Columns::Columns(){
	this->add(this->idName);
	this->add(this->name);
	this->add(this->colorCode_background);
	this->add(this->colorCode_foreground);
}

View_Gtk_Settings_ColorChooser::View_Gtk_Settings_ColorChooser()
	: event_lock(false)
{
	refListStore = Gtk::ListStore::create(columns);
	this->set_model(refListStore);
	
	this->pack_start(columns.name);
	
	Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = this->get_cells();
	Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();
	
	this->add_attribute(*cellRenderer, "background", columns.colorCode_background);
	this->add_attribute(*cellRenderer, "foreground", columns.colorCode_foreground);
}
void View_Gtk_Settings_ColorChooser::addColor(std::string const& codeName, std::string const& outputName, std::string const& cell_background, std::string const& cell_foreground){
	this->event_lock = true;
	Gtk::TreeModel::iterator iter = refListStore->append();
	(*iter)[columns.idName] = codeName;
	(*iter)[columns.name] = outputName;
	(*iter)[columns.colorCode_background] = cell_background;
	(*iter)[columns.colorCode_foreground] = cell_foreground;
	this->event_lock = false;
}
void View_Gtk_Settings_ColorChooser::selectColor(std::string const& codeName){
	this->event_lock = true;
	this->set_active(0);
	for (Gtk::TreeModel::iterator iter = this->get_active(); iter; iter++){
		if ((*iter)[columns.idName] == codeName){
			this->set_active(iter);
			break;
		}
	}
	this->event_lock = false;
}
std::string View_Gtk_Settings_ColorChooser::getSelectedColor() const {
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter)
		return (Glib::ustring)(*iter)[columns.idName];
	else
		return "";
}

Pango::Color View_Gtk_Settings_ColorChooser::getSelectedColorAsPangoObject() const {
	Pango::Color color;
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter) {
		color.parse((Glib::ustring)(*iter)[columns.colorCode_background]);
	}
	return color;
}

GrubColorChooser::GrubColorChooser(bool blackIsTransparent) : View_Gtk_Settings_ColorChooser() {
	this->addColor("white",          gettext("white"),         "#ffffff", "#000000");
	this->addColor("yellow",         gettext("yellow"),        "#fefe54", "#000000");
	this->addColor("light-cyan",     gettext("light-cyan"),    "#54fefe", "#000000");
	this->addColor("cyan",           gettext("cyan"),          "#00a8a8", "#000000");
	this->addColor("light-blue",     gettext("light-blue"),    "#5454fe", "#000000");
	this->addColor("blue",           gettext("blue"),          "#0000a8", "#000000");
	this->addColor("light-green",    gettext("light-green"),   "#54fe54", "#000000");
	this->addColor("green",          gettext("green"),         "#00a800", "#000000");
	this->addColor("light-magenta",  gettext("light-magenta"), "#eb4eeb", "#000000");
	this->addColor("magenta",        gettext("magenta"),       "#a800a8", "#000000");
	this->addColor("light-red",      gettext("light-red"),     "#fe5454", "#000000");
	this->addColor("red",            gettext("red"),           "#ff0000", "#000000");
	this->addColor("brown",          gettext("brown"),         "#a85400", "#000000");
	this->addColor("light-gray",     gettext("light-gray"),    "#a8a8a8", "#000000");
	this->addColor("dark-gray",      gettext("dark-gray"),     "#545454", "#000000");
	this->addColor("black", blackIsTransparent ? gettext("transparent") : gettext("black"), "#000000", "#ffffff");
}

View_Gtk_Settings::CustomOption_obj::CustomOption_obj(std::string name, std::string old_name, std::string value, bool isActive){
	this->name = name;
	this->old_name = old_name;
	this->value = value;
	this->isActive = isActive;
}

View_Gtk_Settings::View_Gtk_Settings()
	: event_lock(false), bttAddCustomEntry(Gtk::Stock::ADD), bttRemoveCustomEntry(Gtk::Stock::REMOVE),
	rbDefPredefined(gettext("pre_defined: "), true), rbDefSaved(gettext("previously _booted entry"), true),
	lblDefaultEntry(gettext("default entry")), lblView(gettext("visibility")), chkShowMenu(gettext("show menu")),
	lblTimeout(gettext("Timeout")), lblTimeout2(gettext("Seconds")), lblKernelParams(gettext("kernel parameters")),
	chkGenerateRecovery(gettext("generate recovery entries")), chkOsProber(gettext("look for other operating systems")),
	chkResolution(gettext("custom resolution: ")),
	lblforegroundColor(gettext("font color")), lblBackgroundColor(gettext("background")),
	lblNormalColor(gettext("normal:"), Pango::ALIGN_RIGHT, Pango::ALIGN_CENTER), lblHighlightColor(gettext("highlight:"), Pango::ALIGN_RIGHT, Pango::ALIGN_CENTER),
	lblColorChooser(gettext("menu colors")), lblBackgroundImage(gettext("background image")),
	imgRemoveBackground(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON), imgRemoveFont(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON),
	lblBackgroundRequiredInfo(gettext("To get the colors above working,\nyou have to select a background image!")),
	gccNormalBackground(true), gccHighlightBackground(true), lblFont("_Font", true), cbResolution()
{
	this->set_title("Grub Customizer - "+Glib::ustring(gettext("settings")));
	this->set_icon_name("grub-customizer");
	Gtk::Box* winBox = this->get_vbox();
	winBox->pack_start(tabbox);
	tabbox.append_page(alignCommonSettings, gettext("_General"), true);
	tabbox.append_page(vbAppearanceSettings, gettext("A_ppearance"), true);
	tabbox.append_page(vbAllEntries, gettext("_Advanced"), true);
	
	vbAllEntries.pack_start(hbAllEntriesControl, Gtk::PACK_SHRINK);
	vbAllEntries.pack_start(scrAllEntries);
	hbAllEntriesControl.add(bttAddCustomEntry);
	hbAllEntriesControl.add(bttRemoveCustomEntry);
	hbAllEntriesControl.set_border_width(5);
	hbAllEntriesControl.set_spacing(5);
	scrAllEntries.add(tvAllEntries);
	scrAllEntries.set_border_width(5);
	scrAllEntries.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	refAsListStore = Gtk::ListStore::create(asTreeModel);
	tvAllEntries.set_model(refAsListStore);
	tvAllEntries.append_column_editable(gettext("is active"), asTreeModel.active);
	tvAllEntries.append_column_editable(gettext("name"), asTreeModel.name);
	tvAllEntries.append_column_editable(gettext("value"), asTreeModel.value);
	refAsListStore->signal_row_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_setting_row_changed));
	vbCommonSettings.set_spacing(15);
	vbAppearanceSettings.set_spacing(15);
	
	//default entry group
	vbCommonSettings.pack_start(groupDefaultEntry, Gtk::PACK_SHRINK);
	groupDefaultEntry.add(alignDefaultEntry);
	groupDefaultEntry.set_sensitive(false);
	groupDefaultEntry.set_label_widget(lblDefaultEntry);
	lblDefaultEntry.set_attributes(attrDefaultEntry);
	aDefaultEntry = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
	attrDefaultEntry.insert(aDefaultEntry);
	alignDefaultEntry.add(vbDefaultEntry);
	vbDefaultEntry.add(hbDefPredefined);
	vbDefaultEntry.add(rbDefSaved);
	
	hbDefPredefined.pack_start(rbDefPredefined, Gtk::PACK_SHRINK);
	hbDefPredefined.pack_start(cbDefEntry);
	
	vbDefaultEntry.set_spacing(5);
	groupDefaultEntry.set_shadow_type(Gtk::SHADOW_NONE);
	alignDefaultEntry.set_padding(2, 2, 25, 2);
	rbDefPredefined.set_group(rbgDefEntry);
	rbDefSaved.set_group(rbgDefEntry);
	//cbDefEntry.set_sensitive(false);
	
	//set maximum size of the combobox
	Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = cbDefEntry.get_cells();
	Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();
	cellRenderer->set_fixed_size(200, -1);
	
	cbDefEntry.set_wrap_width(2);
	
	//view group
	alignCommonSettings.add(vbCommonSettings);
	alignCommonSettings.set_padding(20, 0, 0, 0);
	vbCommonSettings.pack_start(groupView, Gtk::PACK_SHRINK);
	groupView.add(alignView);
	groupView.set_shadow_type(Gtk::SHADOW_NONE);
	groupView.set_label_widget(lblView);
	lblView.set_attributes(attrDefaultEntry);
	alignView.add(vbView);
	vbView.set_spacing(5);
	alignView.set_padding(2, 2, 25, 2);
	vbView.add(chkShowMenu);
	vbView.add(chkOsProber);
	vbView.add(hbTimeout);
	hbTimeout.set_spacing(5);
	hbTimeout.pack_start(lblTimeout, Gtk::PACK_SHRINK);
	hbTimeout.pack_start(spTimeout, Gtk::PACK_SHRINK);
	hbTimeout.pack_start(lblTimeout2, Gtk::PACK_SHRINK);
	
	spTimeout.set_digits(0);
	spTimeout.set_increments(1, 5);
	spTimeout.set_range(0, 1000000);
	
	//kernel parameters
	vbCommonSettings.pack_start(groupKernelParams, Gtk::PACK_SHRINK);
	groupKernelParams.add(alignKernelParams);
	groupKernelParams.set_shadow_type(Gtk::SHADOW_NONE);
	groupKernelParams.set_label_widget(lblKernelParams);
	lblKernelParams.set_attributes(attrDefaultEntry);
	alignKernelParams.add(vbKernelParams);
	alignKernelParams.set_padding(10, 2, 25, 2);
	vbKernelParams.add(txtKernelParams);
	vbKernelParams.add(chkGenerateRecovery);
	vbKernelParams.set_spacing(5);
	
	//screen resolution
	vbAppearanceSettings.pack_start(alignResolution, Gtk::PACK_SHRINK);
	alignResolution.add(hbResolution);
	alignResolution.set_padding(10, 0, 6, 0);
	hbResolution.pack_start(chkResolution, Gtk::PACK_SHRINK);
	hbResolution.pack_start(cbResolution);
	cbResolution.append_text("saved");
	
	//color chooser
	vbAppearanceSettings.pack_start(groupColorChooser, Gtk::PACK_SHRINK);
	groupColorChooser.add(alignColorChooser);
	groupColorChooser.set_label_widget(lblColorChooser);
	groupColorChooser.set_shadow_type(Gtk::SHADOW_NONE);
	lblColorChooser.set_attributes(attrDefaultEntry);
	alignColorChooser.add(tblColorChooser);
	tblColorChooser.attach(lblforegroundColor, 1,2,0,1);
	tblColorChooser.attach(lblBackgroundColor, 2,3,0,1);
	tblColorChooser.attach(lblNormalColor, 0,1,1,2);
	tblColorChooser.attach(lblHighlightColor, 0,1,2,3);
	tblColorChooser.attach(gccNormalForeground, 1,2,1,2);
	tblColorChooser.attach(gccNormalBackground, 2,3,1,2);
	tblColorChooser.attach(gccHighlightForeground, 1,2,2,3);
	tblColorChooser.attach(gccHighlightBackground, 2,3,2,3);
	tblColorChooser.set_spacings(10);

	//font selection
	vbAppearanceSettings.pack_start(groupFont, Gtk::PACK_SHRINK);
	groupFont.add(alignFont);
	groupFont.set_label_widget(lblFont);
	groupFont.set_shadow_type(Gtk::SHADOW_NONE);
	lblFont.set_attributes(attrDefaultEntry);
	lblFont.set_mnemonic_widget(bttFont);
	alignFont.add(hbFont);
	hbFont.pack_start(bttFont);
	hbFont.pack_start(bttRemoveFont, Gtk::PACK_SHRINK);
	bttRemoveFont.add(imgRemoveFont);
	bttRemoveFont.set_tooltip_text(gettext("remove font"));
	bttRemoveFont.set_no_show_all(true);


	//background image
	vbAppearanceSettings.pack_start(groupBackgroundImage);
	groupBackgroundImage.set_shadow_type(Gtk::SHADOW_NONE);
	groupBackgroundImage.add(alignBackgroundImage);
	groupBackgroundImage.set_label_widget(lblBackgroundImage);
	lblBackgroundImage.set_attributes(attrDefaultEntry);
	alignBackgroundImage.add(vbBackgroundImage);
	vbBackgroundImage.pack_start(hbBackgroundImage, Gtk::PACK_SHRINK);
	hbBackgroundImage.pack_start(fcBackgroundImage);
	hbBackgroundImage.pack_start(bttRemoveBackground, Gtk::PACK_SHRINK);
	fcBackgroundImage.set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);

	vbBackgroundImage.pack_start(hbImgBtts);
	vbBackgroundImage.pack_start(lblBackgroundRequiredInfo);
	hbImgBtts.pack_start(drwBackgroundPreview);

	vbBackgroundImage.set_spacing(5);
	hbImgBtts.set_spacing(5);
	vbButtons.set_spacing(5);

	bttRemoveBackground.set_tooltip_text(gettext("remove background"));
	bttRemoveBackground.add(imgRemoveBackground);
	bttRemoveBackground.set_no_show_all(true);
	lblBackgroundRequiredInfo.set_no_show_all(true);
	
	//<signals>
	rbDefPredefined.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_default_entry_predefined_toggeled));
	rbDefSaved.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_default_entry_saved_toggeled));
	cbDefEntry.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_default_entry_changed));
	chkShowMenu.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_showMenu_toggled));
	chkOsProber.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_osProber_toggled));
	spTimeout.signal_value_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_timeout_changed));
	txtKernelParams.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_kernelparams_changed));
	chkGenerateRecovery.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_recovery_toggled));
	chkResolution.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_chkResolution_toggled));
	cbResolution.get_entry()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_resolution_selected));
	gccNormalForeground.signal_changed().connect(sigc::bind<View_Gtk_Settings_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Settings::signal_color_changed), gccNormalForeground));
	gccNormalBackground.signal_changed().connect(sigc::bind<View_Gtk_Settings_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Settings::signal_color_changed), gccNormalBackground));
	gccHighlightForeground.signal_changed().connect(sigc::bind<View_Gtk_Settings_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Settings::signal_color_changed), gccHighlightForeground));
	gccHighlightBackground.signal_changed().connect(sigc::bind<View_Gtk_Settings_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Settings::signal_color_changed), gccHighlightBackground));
	bttFont.signal_font_set().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_font_changed));
	bttRemoveFont.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_font_removed));
	fcBackgroundImage.signal_file_set().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_other_image_chosen));
	bttRemoveBackground.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_bttRemoveBackground_clicked));
	bttAddCustomEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_add_row_button_clicked));
	bttRemoveCustomEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_remove_row_button_clicked));
	drwBackgroundPreview.signal_expose_event().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_redraw_preview));

	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	this->set_default_size(500, 600);
}

void View_Gtk_Settings::setEventListener(SettingsController& eventListener){
	this->eventListener = &eventListener;
}

Gtk::VBox& View_Gtk_Settings::getCommonSettingsPane() {
	tabbox.remove(alignCommonSettings);
	alignCommonSettings.remove();
	return vbCommonSettings;
}

Gtk::VBox& View_Gtk_Settings::getAppearanceSettingsPane() {
	tabbox.remove(vbAppearanceSettings);
	return vbAppearanceSettings;
}

void View_Gtk_Settings::show(bool burgMode) {
	this->show_all();
	if (burgMode){
		groupColorChooser.hide();
		groupBackgroundImage.hide();
	}
	else {
		groupColorChooser.show();
		groupBackgroundImage.show();
	}
}

void View_Gtk_Settings::hide(){
	Gtk::Dialog::hide();
}


void View_Gtk_Settings::on_response(int response_id) {
	this->eventListener->hideAction();
}

void View_Gtk_Settings::addEntryToDefaultEntryChooser(std::string const& labelPathValue, std::string const& labelPathLabel, std::string const& numericPathValue, std::string const& numericPathLabel){
	event_lock = true;
	this->defEntryValueMapping[this->defEntryValueMapping.size()] = numericPathValue;
	cbDefEntry.append_text(Glib::ustring::compose(gettext("Entry %1 (by position)"), numericPathLabel));
	this->defEntryValueMapping[this->defEntryValueMapping.size()] = labelPathValue;
	cbDefEntry.append_text(labelPathLabel);
	cbDefEntry.set_active(0);
	this->groupDefaultEntry.set_sensitive(true);
	event_lock = false;
}

void View_Gtk_Settings::clearDefaultEntryChooser(){
	event_lock = true;
	cbDefEntry.clear_items();
	this->defEntryValueMapping.clear();
	this->groupDefaultEntry.set_sensitive(false); //if there's no entry to select, disable this area
	event_lock = false;
}


void View_Gtk_Settings::clearResolutionChooser(){
	this->cbResolution.clear_items();
}
void View_Gtk_Settings::addResolution(std::string const& resolution){
	this->cbResolution.append_text(resolution);
}

View_Gtk_Settings::AdvancedSettingsTreeModel::AdvancedSettingsTreeModel(){
	this->add(active);
	this->add(name);
	this->add(old_name);
	this->add(value);
}

void View_Gtk_Settings::addCustomOption(bool isActive, std::string const& name, std::string const& value){
	this->event_lock = true;
	Gtk::TreeModel::iterator newItemIter = refAsListStore->append();
	(*newItemIter)[asTreeModel.active] = isActive;
	(*newItemIter)[asTreeModel.name] = name;
	(*newItemIter)[asTreeModel.old_name] = name;
	(*newItemIter)[asTreeModel.value] = value;
	this->event_lock = false;
}

void View_Gtk_Settings::selectCustomOption(std::string const& name){
	for (Gtk::TreeModel::iterator iter = refAsListStore->children().begin(); iter != refAsListStore->children().end(); iter++){
		if ((*iter)[asTreeModel.old_name] == name){
			tvAllEntries.set_cursor(refAsListStore->get_path(iter), *tvAllEntries.get_column(name == "" ? 1 : 2), name == "");
			break;
		}
	}
}

std::string View_Gtk_Settings::getSelectedCustomOption(){
	std::vector<Gtk::TreeModel::Path> p = tvAllEntries.get_selection()->get_selected_rows();
	if (p.size() == 1)
		return (Glib::ustring)(*refAsListStore->get_iter(p.front()))[asTreeModel.name];
	else
		return "";
}

void View_Gtk_Settings::removeAllSettingRows(){
	this->event_lock = true;
	this->refAsListStore->clear();
	this->event_lock = false;
}

void View_Gtk_Settings::setActiveDefEntryOption(DefEntryType option){
	this->event_lock = true;
	if (option == this->DEF_ENTRY_SAVED) {
		rbDefSaved.set_active(true);
		cbDefEntry.set_sensitive(false);
	}
	else if (option == this->DEF_ENTRY_PREDEFINED) {
		rbDefPredefined.set_active(true);
		cbDefEntry.set_sensitive(true);
	}
	this->event_lock = false;
}

void View_Gtk_Settings::setDefEntry(std::string const& defEntry){
	this->event_lock = true;

	int pos = 0;
	for (std::map<int, std::string>::iterator iter = this->defEntryValueMapping.begin(); iter != this->defEntryValueMapping.end(); iter++) {
		if (iter->second == defEntry) {
			pos = iter->first;
			break;
		}
	}

	cbDefEntry.set_active(pos);
	this->event_lock = false;
}

void View_Gtk_Settings::setShowMenuCheckboxState(bool isActive){
	this->event_lock = true;
	chkShowMenu.set_active(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setOsProberCheckboxState(bool isActive){
	this->event_lock = true;
	chkOsProber.set_active(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setTimeoutValue(int value){
	this->event_lock = true;
	spTimeout.set_value(value);
	this->event_lock = false;
}

void View_Gtk_Settings::setKernelParams(std::string const& params){
	this->event_lock = true;
	txtKernelParams.set_text(params);
	this->event_lock = false;
}

void View_Gtk_Settings::setRecoveryCheckboxState(bool isActive){
	this->event_lock = true;
	chkGenerateRecovery.set_active(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setResolutionCheckboxState(bool isActive){
	this->event_lock = true;
	chkResolution.set_active(isActive);
	cbResolution.set_sensitive(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setResolution(std::string const& resolution){
	this->event_lock = true;
	cbResolution.get_entry()->set_text(resolution);
	this->event_lock = false;
}

View_ColorChooser& View_Gtk_Settings::getColorChooser(ColorChooserType type){
	View_ColorChooser* result = NULL;
	switch (type){
		case COLOR_CHOOSER_DEFAULT_BACKGROUND: result = &this->gccNormalBackground; break;
		case COLOR_CHOOSER_DEFAULT_FONT: result = &this->gccNormalForeground; break;
		case COLOR_CHOOSER_HIGHLIGHT_BACKGROUND: result = &this->gccHighlightBackground; break;
		case COLOR_CHOOSER_HIGHLIGHT_FONT: result = &this->gccHighlightForeground; break;
	}

	assert(result != NULL);
	return *result;
}

std::string View_Gtk_Settings::getFontName() {
	return bttFont.get_font_name();
}

int View_Gtk_Settings::getFontSize() {
	Pango::FontDescription desc(bttFont.get_font_name());
	return desc.get_size() / 1024;
}

void View_Gtk_Settings::setFontName(std::string const& value) {
	bttFont.set_font_name(value);
	bttRemoveFont.set_visible(value != "");
	imgRemoveFont.set_visible(value != "");
}

Glib::RefPtr<Pango::Layout> View_Gtk_Settings::createFormattedText(Cairo::RefPtr<Cairo::Context>& context, Glib::ustring const& text, std::string const& format, int r, int g, int b, int r_b, int g_b, int b_b, bool black_bg_is_transparent) {
	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(context);
	layout->set_text(text);
	Pango::AttrList attrList;
	if (!black_bg_is_transparent || r_b != 0 || g_b != 0 || b_b != 0) {
		Pango::AttrColor bColor = Pango::Attribute::create_attr_background(r_b*255, g_b*255, b_b*255);
		attrList.insert(bColor);
	}
	Pango::AttrColor fColor = Pango::Attribute::create_attr_foreground(r*255, g*255, b*255);
	attrList.insert(fColor);
	Pango::AttrString font = Pango::Attribute::create_attr_family("monospace");
	if (format == "" || format == "Normal") {
		attrList.insert(font);
	} else {
		layout->set_font_description(Pango::FontDescription(format));
	}
	layout->set_attributes(attrList);
	return layout;
}

void View_Gtk_Settings::setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir){
	this->redraw(menuPicturePath, isInGrubDir);
}

void View_Gtk_Settings::redraw(std::string const& menuPicturePath, bool isInGrubDir, Cairo::RefPtr<Cairo::Context> const* cr){
	this->event_lock = true;
	this->backgroundImagePath = menuPicturePath;

	if (menuPicturePath != "" && !drwBackgroundPreview.get_visible()) {
		drwBackgroundPreview.show();
	}

	if (menuPicturePath != "" && drwBackgroundPreview.get_window()){ //it's important to check whether there's a gdk window, if not, Gdk::Pixbuf::create_from_file produces a crash!
		try {
			Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_file(menuPicturePath, drwBackgroundPreview.get_width(), -1, true);
			if (buf) {
				Cairo::RefPtr<Cairo::Context> context = cr ? *cr : drwBackgroundPreview.get_window()->create_cairo_context();

				drwBackgroundPreview.show();
				Gdk::Cairo::set_source_pixbuf(context, buf, 0, 0);
				context->rectangle(0, 0, buf->get_width(), buf->get_height());
				context->fill();

				std::list<Glib::RefPtr<Pango::Layout> > exampleTexts;
				Pango::Color fg_n = this->gccNormalForeground.getSelectedColorAsPangoObject();
				Pango::Color bg_n = this->gccNormalBackground.getSelectedColorAsPangoObject();
				Pango::Color fg_s = this->gccHighlightForeground.getSelectedColorAsPangoObject();
				Pango::Color bg_s = this->gccHighlightBackground.getSelectedColorAsPangoObject();
				std::string fontName = bttFont.get_font_name();
				this->previewEntryTitles_mutex.lock();
				for (std::list<std::string>::iterator iter = this->previewEntryTitles.begin(); iter != this->previewEntryTitles.end(); iter++) {
					if (iter == this->previewEntryTitles.begin()) {
						exampleTexts.push_back(View_Gtk_Settings::createFormattedText(context, *iter, fontName, fg_s.get_red() / 255, fg_s.get_green() / 255, fg_s.get_blue() / 255, bg_s.get_red() / 255, bg_s.get_green() / 255, bg_s.get_blue() / 255));
					} else {
						exampleTexts.push_back(View_Gtk_Settings::createFormattedText(context, *iter, fontName, fg_n.get_red() / 255, fg_n.get_green() / 255, fg_n.get_blue() / 255, bg_n.get_red() / 255, bg_n.get_green() / 255, bg_n.get_blue() / 255));
					}
				}
				this->previewEntryTitles_mutex.unlock();

				int vpos = 0;
				for (std::list<Glib::RefPtr<Pango::Layout> >::iterator iter = exampleTexts.begin(); iter != exampleTexts.end(); iter++) {
					context->move_to(0, vpos);
					(*iter)->show_in_cairo_context(context);
					vpos += (*iter)->get_height();
					int x,y;
					(*iter)->get_pixel_size(x,y);
					vpos += y;
				}
			} else {
				throw Glib::Error();
			}
		}
		catch (Glib::Error const& e){
			Cairo::RefPtr<Cairo::Context> context = cr ? *cr : drwBackgroundPreview.get_window()->create_cairo_context();
			Glib::RefPtr<Gdk::Pixbuf> buf = drwBackgroundPreview.render_icon(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_DIALOG);

			Gdk::Cairo::set_source_pixbuf(context, buf, 0, 0);
			context->rectangle(0, 0, buf->get_width(), buf->get_height());
			context->fill();
		}

		bttRemoveBackground.show();
		imgRemoveBackground.show();
		lblBackgroundRequiredInfo.hide();
		fcBackgroundImage.set_filename(menuPicturePath);
	}
	else {
		fcBackgroundImage.unselect_all();
		bttRemoveBackground.hide();
		imgRemoveBackground.hide();
		drwBackgroundPreview.hide();
		lblBackgroundRequiredInfo.show();
	}

	bttCopyBackground.set_sensitive(!isInGrubDir);
	this->event_lock = false;
}


std::string View_Gtk_Settings::getSelectedDefaultGrubValue(){
	return this->defEntryValueMapping[cbDefEntry.get_active_row_number()];
}

View_Gtk_Settings::CustomOption View_Gtk_Settings::getCustomOption(std::string const& name){
	for (Gtk::TreeModel::iterator iter = this->refAsListStore->children().begin(); iter != this->refAsListStore->children().end(); iter++){
		if ((*iter)[asTreeModel.old_name] == name)
			return CustomOption_obj(Glib::ustring((*iter)[asTreeModel.name]), Glib::ustring((*iter)[asTreeModel.old_name]), Glib::ustring((*iter)[asTreeModel.value]), (*iter)[asTreeModel.active]);
	}
	throw ItemNotFoundException("requested custom option not found", __FILE__, __LINE__);
}

void View_Gtk_Settings::signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (!event_lock){
		this->eventListener->updateCustomSettingAction((Glib::ustring)(*iter)[asTreeModel.old_name]);
	}
}

void View_Gtk_Settings::signal_default_entry_predefined_toggeled(){
	if (!event_lock){
		this->eventListener->updateDefaultSystemAction();
	}
}

View_Gtk_Settings::DefEntryType View_Gtk_Settings::getActiveDefEntryOption(){
	return rbDefSaved.get_active() ? DEF_ENTRY_SAVED : DEF_ENTRY_PREDEFINED;
}

void View_Gtk_Settings::signal_default_entry_saved_toggeled(){
	if (!event_lock){
		this->eventListener->updateDefaultSystemAction();
	}
}

void View_Gtk_Settings::signal_default_entry_changed(){
	if (!event_lock){
		this->eventListener->updateDefaultSystemAction();
	}
}

bool View_Gtk_Settings::getShowMenuCheckboxState(){
	return chkShowMenu.get_active();
}

bool View_Gtk_Settings::getOsProberCheckboxState(){
	return chkOsProber.get_active();
}

void View_Gtk_Settings::showHiddenMenuOsProberConflictMessage(){
	Gtk::MessageDialog(Glib::ustring::compose(gettext("This option doesn't work when the \"os-prober\" script finds other operating systems. Disable \"%1\" if you don't need to boot other operating systems."), chkOsProber.get_label())).run();
}


void View_Gtk_Settings::signal_showMenu_toggled(){
	if (!event_lock){
		this->eventListener->updateShowMenuSettingAction();
	}
}


void View_Gtk_Settings::signal_osProber_toggled(){
	if (!event_lock){
		this->eventListener->updateOsProberSettingAction();
	}
}

int View_Gtk_Settings::getTimeoutValue(){
	return spTimeout.get_value_as_int();
}

std::string View_Gtk_Settings::getTimeoutValueString() {
	return Glib::ustring::format(this->getTimeoutValue());
}

void View_Gtk_Settings::signal_timeout_changed(){
	if (!event_lock){
		this->eventListener->updateTimeoutSettingAction();
	}
}

std::string View_Gtk_Settings::getKernelParams(){
	return txtKernelParams.get_text();
}


void View_Gtk_Settings::signal_kernelparams_changed(){
	if (!event_lock){
		this->eventListener->updateKernelParamsAction();
	}
}

bool View_Gtk_Settings::getRecoveryCheckboxState(){
	return chkGenerateRecovery.get_active();
}


void View_Gtk_Settings::signal_recovery_toggled(){
	if (!event_lock){
		this->eventListener->updateRecoverySettingAction();
	}
}

bool View_Gtk_Settings::getResolutionCheckboxState(){
	return chkResolution.get_active();
}


void View_Gtk_Settings::signal_chkResolution_toggled(){
	if (!event_lock){
		this->eventListener->updateUseCustomResolutionAction();
	}
}

std::string View_Gtk_Settings::getResolution(){
	return cbResolution.get_entry()->get_text();
}


void View_Gtk_Settings::signal_resolution_selected(){
	if (!event_lock){
		this->eventListener->updateCustomResolutionAction();
	}
}


void View_Gtk_Settings::signal_color_changed(View_Gtk_Settings_ColorChooser& caller){
	if (!event_lock && !caller.event_lock){
		this->eventListener->updateColorSettingsAction();
	}
}

void View_Gtk_Settings::signal_font_changed() {
	if (!event_lock) {
		this->eventListener->updateFontSettingsAction(false);
	}
}

void View_Gtk_Settings::signal_font_removed() {
	if (!event_lock) {
		this->eventListener->updateFontSettingsAction(true);
	}
}

std::string View_Gtk_Settings::getBackgroundImagePath(){
	return fcBackgroundImage.get_filename();
}


void View_Gtk_Settings::signal_bttRemoveBackground_clicked(){
	if (!event_lock){
		this->eventListener->removeBackgroundImageAction();
	}
}


void View_Gtk_Settings::signal_other_image_chosen(){
	if (!event_lock){
		this->eventListener->updateBackgroundImageAction();
	}
}

void View_Gtk_Settings::signal_add_row_button_clicked(){
	if (!event_lock)
		this->eventListener->addCustomSettingAction();
}
void View_Gtk_Settings::signal_remove_row_button_clicked(){
	if (!event_lock)
		this->eventListener->removeCustomSettingAction((Glib::ustring)(*tvAllEntries.get_selection()->get_selected())[asTreeModel.name]);
}

bool View_Gtk_Settings::signal_redraw_preview(GdkEventExpose* event) {
	if (!event_lock) {
		this->redraw(this->backgroundImagePath, false);
	}
	return true;
}

void View_Gtk_Settings::setPreviewEntryTitles(std::list<std::string> const& entries) {
	this->previewEntryTitles_mutex.lock();
	this->previewEntryTitles = entries;
	this->previewEntryTitles_mutex.unlock();
}



