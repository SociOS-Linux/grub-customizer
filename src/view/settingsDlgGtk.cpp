#include "settingsDlgGtk.h"

ColorChooserGtk::Columns::Columns(){
	this->add(this->idName);
	this->add(this->name);
	this->add(this->colorCode_background);
	this->add(this->colorCode_foreground);
}

ColorChooserGtk::ColorChooserGtk(){
	refListStore = Gtk::ListStore::create(columns);
	this->set_model(refListStore);
	
	this->pack_start(columns.name);
	
	Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = this->get_cells();
	Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();
	
	this->add_attribute(*cellRenderer, "background", columns.colorCode_background);
	this->add_attribute(*cellRenderer, "foreground", columns.colorCode_foreground);
}
void ColorChooserGtk::addColor(std::string const& codeName, std::string const& outputName, std::string const& cell_background, std::string const& cell_foreground){
	Gtk::TreeModel::iterator iter = refListStore->append();
	(*iter)[columns.idName] = codeName;
	(*iter)[columns.name] = outputName;
	(*iter)[columns.colorCode_background] = cell_background;
	(*iter)[columns.colorCode_foreground] = cell_foreground;
}
void ColorChooserGtk::selectColor(std::string const& codeName){
	this->set_active(0);
	for (Gtk::TreeModel::iterator iter = this->get_active(); iter; iter++){
		if ((*iter)[columns.idName] == codeName){
			this->set_active(iter);
			break;
		}
	}
}
std::string ColorChooserGtk::getSelectedColor() const {
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter)
		return (Glib::ustring)(*iter)[columns.idName];
	else
		return "";
}

GrubColorChooser::GrubColorChooser(bool blackIsTransparent) : ColorChooserGtk() {
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

GrubSettingsDlgGtk::CustomOption_obj::CustomOption_obj(std::string name, std::string old_name, std::string value, bool isActive){
	this->name = name;
	this->old_name = old_name;
	this->value = value;
	this->isActive = isActive;
}

GrubSettingsDlgGtk::GrubSettingsDlgGtk()
	: event_lock(false), bttAddCustomEntry(Gtk::Stock::ADD), bttRemoveCustomEntry(Gtk::Stock::REMOVE),
	rbDefPredefined(gettext("pre_defined: "), true), rbDefSaved(gettext("previously _booted entry"), true),
	lblDefaultEntry(gettext("default entry")), lblView(gettext("visibility")), chkShowMenu(gettext("show menu")),
	lblTimeout(gettext("Timeout")), lblTimeout2(gettext("Seconds")), lblKernelParams(gettext("kernel parameters")),
	chkGenerateRecovery(gettext("generate recovery entries")), chkOsProber(gettext("look for other operating systems")),
	chkResolution(gettext("custom resolution: ")),
	lblforegroundColor(gettext("font color")), lblBackgroundColor(gettext("background")),
	lblNormalColor(gettext("normal:"), Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER), lblHighlightColor(gettext("highlight:"), Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER),
	lblColorChooser(gettext("menu colors")), lblBackgroundImage(gettext("background image")),
	bttCopyBackground(gettext("copy to grub directory")), bttRemoveBackground(gettext("remove background")),
	lblBackgroundRequiredInfo(gettext("To get the colors above working,\nyou have to select a background image!")),
	gccNormalBackground(true), gccHighlightBackground(true), defEntry_item_count(0)
{
	this->set_title("Grub Customizer - "+Glib::ustring(gettext("settings")));
	this->set_icon_name("grub-customizer");
	Gtk::VBox* winBox = this->get_vbox();
	winBox->add(tabbox);
	tabbox.append_page(vbCommonSettings, gettext("_General"), true);
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
	refAsListStore->signal_row_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_setting_row_changed));
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

	//background image
	vbAppearanceSettings.pack_start(groupBackgroundImage);
	groupBackgroundImage.set_shadow_type(Gtk::SHADOW_NONE);
	groupBackgroundImage.add(alignBackgroundImage);
	groupBackgroundImage.set_label_widget(lblBackgroundImage);
	lblBackgroundImage.set_attributes(attrDefaultEntry);
	alignBackgroundImage.add(vbBackgroundImage);
	vbBackgroundImage.pack_start(fcBackgroundImage, Gtk::PACK_SHRINK);
	fcBackgroundImage.set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);

	vbBackgroundImage.pack_start(hbImgBtts);
	vbBackgroundImage.pack_start(lblBackgroundRequiredInfo);
	hbImgBtts.pack_start(vbButtons);
	vbButtons.add(bttCopyBackground);
	vbButtons.add(bttRemoveBackground);
	hbImgBtts.pack_start(imgBackgroundImage, Gtk::PACK_SHRINK);

	vbBackgroundImage.set_spacing(5);
	hbImgBtts.set_spacing(5);
	vbButtons.set_spacing(5);

	bttCopyBackground.set_no_show_all(true);
	bttRemoveBackground.set_no_show_all(true);
	imgBackgroundImage.set_no_show_all(true);
	
	//<signals>
	rbDefPredefined.signal_toggled().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_default_entry_predefined_toggeled));
	rbDefSaved.signal_toggled().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_default_entry_saved_toggeled));
	cbDefEntry.signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_default_entry_changed));
	chkShowMenu.signal_toggled().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_showMenu_toggled));
	chkOsProber.signal_toggled().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_osProber_toggled));
	spTimeout.signal_value_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_timeout_changed));
	txtKernelParams.signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_kernelparams_changed));
	chkGenerateRecovery.signal_toggled().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_recovery_toggled));
	chkResolution.signal_toggled().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_chkResolution_toggled));
	cbResolution.get_entry()->signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_resolution_selected));
	gccNormalForeground.signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_color_changed));
	gccNormalBackground.signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_color_changed));
	gccHighlightForeground.signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_color_changed));
	gccHighlightBackground.signal_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_color_changed));
	fcBackgroundImage.signal_file_set().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_other_image_chosen));
	bttCopyBackground.signal_clicked().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_bttCopyBackground_clicked));
	bttRemoveBackground.signal_clicked().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_bttRemoveBackground_clicked));
	bttAddCustomEntry.signal_clicked().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_add_row_button_clicked));
	bttRemoveCustomEntry.signal_clicked().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_remove_row_button_clicked));

	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	this->set_default_size(300, 400);
}

void GrubSettingsDlgGtk::setEventListener(EventListener_settings& eventListener){
	this->eventListener = &eventListener;
}

void GrubSettingsDlgGtk::show(bool burgMode) {
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

void GrubSettingsDlgGtk::hide(){
	Gtk::Dialog::hide();
}


void GrubSettingsDlgGtk::on_response(int response_id) {
	this->eventListener->settings_dialog_hide_request();
}

void GrubSettingsDlgGtk::addEntryToDefaultEntryChooser(std::string const& entryTitle){
	event_lock = true;
	cbDefEntry.append_text(Glib::ustring::compose(gettext("Entry %1 (by position)"), ++defEntry_item_count));
	cbDefEntry.append_text(entryTitle);
	cbDefEntry.set_active(0);
	this->groupDefaultEntry.set_sensitive(true);
	event_lock = false;
}

void GrubSettingsDlgGtk::clearDefaultEntryChooser(){
	event_lock = true;
	cbDefEntry.clear_items();
	this->defEntry_item_count = 0;
	this->groupDefaultEntry.set_sensitive(false); //if there's no entry to select, disable this area
	event_lock = false;
}


void GrubSettingsDlgGtk::clearResolutionChooser(){
	this->cbResolution.clear_items();
}
void GrubSettingsDlgGtk::addResolution(std::string const& resolution){
	this->cbResolution.append_text(resolution);
}

GrubSettingsDlgGtk::AdvancedSettingsTreeModel::AdvancedSettingsTreeModel(){
	this->add(active);
	this->add(name);
	this->add(old_name);
	this->add(value);
}

void GrubSettingsDlgGtk::addCustomOption(bool isActive, Glib::ustring const& name, Glib::ustring const& value){
	this->event_lock = true;
	Gtk::TreeModel::iterator newItemIter = refAsListStore->append();
	(*newItemIter)[asTreeModel.active] = isActive;
	(*newItemIter)[asTreeModel.name] = name;
	(*newItemIter)[asTreeModel.old_name] = name;
	(*newItemIter)[asTreeModel.value] = value;
	this->event_lock = false;
}

void GrubSettingsDlgGtk::selectCustomOption(std::string const& name){
	for (Gtk::TreeModel::iterator iter = refAsListStore->children().begin(); iter != refAsListStore->children().end(); iter++){
		if ((*iter)[asTreeModel.old_name] == name){
			tvAllEntries.set_cursor(refAsListStore->get_path(iter), *tvAllEntries.get_column(name == "" ? 1 : 2), name == "");
			break;
		}
	}
}

std::string GrubSettingsDlgGtk::getSelectedCustomOption(){
	std::list<Gtk::TreeModel::Path> p = tvAllEntries.get_selection()->get_selected_rows();
	if (p.size() == 1)
		return (Glib::ustring)(*refAsListStore->get_iter(p.front()))[asTreeModel.old_name];
	else
		return "";
}

void GrubSettingsDlgGtk::removeAllSettingRows(){
	this->event_lock = true;
	this->refAsListStore->clear();
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setActiveDefEntryOption(DefEntryType option){
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

void GrubSettingsDlgGtk::setDefEntry(Glib::ustring const& defEntry){
	this->event_lock = true;
	if (defEntry.find_first_not_of("0123456789") == std::string::npos){
		std::istringstream in(defEntry);
		int defEntryNum;
		in >> defEntryNum;

		cbDefEntry.set_active(defEntryNum*2); //set active element by index, "*2" is required because there are two columns
	}
	else {
		cbDefEntry.set_active_text(defEntry);
	}
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setShowMenuCheckboxState(bool isActive){
	this->event_lock = true;
	chkShowMenu.set_active(isActive);
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setOsProberCheckboxState(bool isActive){
	this->event_lock = true;
	chkOsProber.set_active(isActive);
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setTimeoutValue(int value){
	this->event_lock = true;
	spTimeout.set_value(value);
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setKernelParams(Glib::ustring const& params){
	this->event_lock = true;
	txtKernelParams.set_text(params);
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setRecoveryCheckboxState(bool isActive){
	this->event_lock = true;
	chkGenerateRecovery.set_active(isActive);
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setResolutionCheckboxState(bool isActive){
	this->event_lock = true;
	chkResolution.set_active(isActive);
	cbResolution.set_sensitive(isActive);
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setResolution(Glib::ustring const& resolution){
	this->event_lock = true;
	cbResolution.get_entry()->set_text(resolution);
	this->event_lock = false;
}

ColorChooser& GrubSettingsDlgGtk::getColorChooser(ColorChooserType type){
	this->event_lock = true;
	switch (type){
		case COLOR_CHOOSER_DEFAULT_BACKGROUND: return this->gccNormalBackground;
		case COLOR_CHOOSER_DEFAULT_FONT: return this->gccNormalForeground;
		case COLOR_CHOOSER_HIGHLIGHT_BACKGROUND: return this->gccHighlightBackground;
		case COLOR_CHOOSER_HIGHLIGHT_FONT: return this->gccHighlightForeground;
	}
	this->event_lock = false;
}

void GrubSettingsDlgGtk::setBackgroundImagePreviewPath(Glib::ustring const& menuPicturePath, bool isInGrubDir){
	this->event_lock = true;
	if (menuPicturePath != ""){
		try {
			Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_file(menuPicturePath, 150, 100, true);
			if (buf)
				imgBackgroundImage.set(buf);
		}
		catch (Glib::Error e){
			imgBackgroundImage.set(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_DIALOG);
		}

		bttCopyBackground.show();
		bttRemoveBackground.show();
		imgBackgroundImage.show();
		lblBackgroundRequiredInfo.hide();
		fcBackgroundImage.set_filename(menuPicturePath);
	}
	else {
		fcBackgroundImage.unselect_all();
		imgBackgroundImage.clear();
		bttCopyBackground.hide();
		bttRemoveBackground.hide();
		imgBackgroundImage.hide();
		lblBackgroundRequiredInfo.show();
	}

	bttCopyBackground.set_sensitive(!isInGrubDir);
	this->event_lock = false;
}


Glib::ustring GrubSettingsDlgGtk::getSelectedDefaultGrubValue(){
	Glib::ustring val;
	if ((cbDefEntry.get_active_row_number() % 2) == 0) //index selected
		val = Glib::ustring::format(cbDefEntry.get_active_row_number() / 2);
	else //name selected
		val = cbDefEntry.get_active_text();
	return val;
}

GrubSettingsDlgGtk::CustomOption GrubSettingsDlgGtk::getCustomOption(Glib::ustring const& name){
	for (Gtk::TreeModel::iterator iter = this->refAsListStore->children().begin(); iter != this->refAsListStore->children().end(); iter++){
		if ((*iter)[asTreeModel.old_name] == name)
			return CustomOption_obj(Glib::ustring((*iter)[asTreeModel.name]), Glib::ustring((*iter)[asTreeModel.old_name]), Glib::ustring((*iter)[asTreeModel.value]), (*iter)[asTreeModel.active]);
	}
	throw REQUESTED_CUSTOM_OPTION_NOT_FOUND;
}

void GrubSettingsDlgGtk::signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (!event_lock){
		this->eventListener->setting_row_changed((Glib::ustring)(*iter)[asTreeModel.old_name]);
	}
}

void GrubSettingsDlgGtk::signal_default_entry_predefined_toggeled(){
	if (!event_lock){
		this->eventListener->default_entry_predefined_toggeled();
	}
}

GrubSettingsDlgGtk::DefEntryType GrubSettingsDlgGtk::getActiveDefEntryOption(){
	return rbDefSaved.get_active() ? DEF_ENTRY_SAVED : DEF_ENTRY_PREDEFINED;
}

void GrubSettingsDlgGtk::signal_default_entry_saved_toggeled(){
	if (!event_lock){
		this->eventListener->default_entry_saved_toggeled();
	}
}

void GrubSettingsDlgGtk::signal_default_entry_changed(){
	if (!event_lock){
		this->eventListener->default_entry_changed();
	}
}

bool GrubSettingsDlgGtk::getShowMenuCheckboxState(){
	return chkShowMenu.get_active();
}

bool GrubSettingsDlgGtk::getOsProberCheckboxState(){
	return chkOsProber.get_active();
}

void GrubSettingsDlgGtk::showHiddenMenuOsProberConflictMessage(){
	Gtk::MessageDialog(Glib::ustring::compose(gettext("This option doesn't work when the \"os-prober\" script finds other operating systems. Disable \"%1\" if you don't need to boot other operating systems."), chkOsProber.get_label())).run();
}


void GrubSettingsDlgGtk::signal_showMenu_toggled(){
	if (!event_lock){
		this->eventListener->showMenu_toggled();
	}
}


void GrubSettingsDlgGtk::signal_osProber_toggled(){
	if (!event_lock){
		this->eventListener->osProber_toggled();
	}
}

int GrubSettingsDlgGtk::getTimeoutValue(){
	return spTimeout.get_value_as_int();
}

void GrubSettingsDlgGtk::signal_timeout_changed(){
	if (!event_lock){
		this->eventListener->timeout_changed();
	}
}

Glib::ustring GrubSettingsDlgGtk::getKernelParams(){
	return txtKernelParams.get_text();
}


void GrubSettingsDlgGtk::signal_kernelparams_changed(){
	if (!event_lock){
		this->eventListener->kernelparams_changed();
	}
}

bool GrubSettingsDlgGtk::getRecoveryCheckboxState(){
	return chkGenerateRecovery.get_active();
}


void GrubSettingsDlgGtk::signal_recovery_toggled(){
	if (!event_lock){
		this->eventListener->generateRecovery_toggled();
	}
}

bool GrubSettingsDlgGtk::getResolutionCheckboxState(){
	return chkResolution.get_active();
}


void GrubSettingsDlgGtk::signal_chkResolution_toggled(){
	if (!event_lock){
		this->eventListener->useCustomResolution_toggled();
	}
}

Glib::ustring GrubSettingsDlgGtk::getResolution(){
	cbResolution.get_entry()->get_text();
}


void GrubSettingsDlgGtk::signal_resolution_selected(){
	if (!event_lock){
		this->eventListener->resolution_changed();
	}
}


void GrubSettingsDlgGtk::signal_color_changed(){
	if (!event_lock){
		this->eventListener->colorChange_requested();
	}
}

Glib::ustring GrubSettingsDlgGtk::getBackgroundImagePath(){
	return fcBackgroundImage.get_filename();
}


void GrubSettingsDlgGtk::signal_bttCopyBackground_clicked(){
	if (!event_lock){
		this->eventListener->backgroundCopy_requested();
	}
}


void GrubSettingsDlgGtk::signal_bttRemoveBackground_clicked(){
	if (!event_lock){
		this->eventListener->backgroundRemove_requested();
	}
}


void GrubSettingsDlgGtk::signal_other_image_chosen(){
	if (!event_lock){
		this->eventListener->backgroundChange_requested();
	}
}

void GrubSettingsDlgGtk::signal_add_row_button_clicked(){
	if (!event_lock)
		this->eventListener->customRow_insert_requested();
}
void GrubSettingsDlgGtk::signal_remove_row_button_clicked(){
	if (!event_lock)
		this->eventListener->customRow_remove_requested((Glib::ustring)(*tvAllEntries.get_selection()->get_selected())[asTreeModel.name]);
}


