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

#include "Theme.h"

View_Gtk_Theme::View_Gtk_Theme()
	: lvFiles(1, true), lblFileSelection(gettext("_Load file: "), true),
	  tbttAdd(Gtk::Stock::ADD), tbttRemove(Gtk::Stock::REMOVE), event_lock(false),
	  lblTheme(gettext("_Theme:"), true),
	  lblforegroundColor(gettext("font color")), lblBackgroundColor(gettext("background")),
	  lblNormalColor(gettext("normal:"), Pango::ALIGN_RIGHT, Pango::ALIGN_CENTER), lblHighlightColor(gettext("highlight:"), Pango::ALIGN_RIGHT, Pango::ALIGN_CENTER),
	  lblColorChooser(gettext("menu colors")), lblBackgroundImage(gettext("background image")),
	  imgRemoveBackground(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON), imgRemoveFont(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON),
	  lblBackgroundRequiredInfo(gettext("To get the colors above working,\nyou have to select a background image!")),
	  gccNormalBackground(true), gccHighlightBackground(true), lblFont(gettext("_Font"), true),
	  imgAddTheme(Gtk::Stock::ADD, Gtk::ICON_SIZE_BUTTON),
	  fcThemeFileChooser(*this, gettext("choose theme file"), Gtk::FILE_CHOOSER_ACTION_OPEN)
{
	Gtk::Box& dlgVBox = *this->get_vbox();

	dlgVBox.pack_start(hbTheme, Gtk::PACK_SHRINK);
	hbTheme.pack_start(lblTheme, Gtk::PACK_SHRINK);
	hbTheme.pack_start(cbTheme);
	hbTheme.pack_start(bttAddTheme, Gtk::PACK_SHRINK);
	bttAddTheme.add(imgAddTheme);

	dlgVBox.pack_start(vbMain);

	vbMain.pack_start(hpThemeEditor);
	vbMain.pack_start(vbCustomTheme);

	toolbar.add(tbttAdd);
	toolbar.add(tbttRemove);

	hpThemeEditor.pack1(vbFiles, Gtk::FILL);
	hpThemeEditor.pack2(vbEdit, Gtk::FILL, Gtk::EXPAND);

	vbFiles.pack_start(scrFiles);
	vbFiles.pack_start(toolbar, Gtk::PACK_SHRINK);

	vbEdit.pack_start(scrEdit);
	vbEdit.pack_start(hbFileSelection, Gtk::PACK_SHRINK);

	scrFiles.add(lvFiles);
	scrEdit.add(vbEditInner);
	vbEditInner.pack_start(txtEdit);
	vbEditInner.pack_start(imgPreview);

	hbFileSelection.pack_start(lblFileSelection, Gtk::PACK_SHRINK);
	hbFileSelection.pack_start(fcFileSelection);

	sizeGroupFooter = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_VERTICAL);
	sizeGroupFooter->add_widget(toolbar);
	sizeGroupFooter->add_widget(hbFileSelection);


	lblTheme.set_mnemonic_widget(cbTheme);

	tbttAdd.set_is_important(true);

	txtEdit.set_no_show_all(true);

	lblFileSelection.set_mnemonic_widget(fcFileSelection);

	lvFiles.set_column_title(0, gettext("File"));

	scrEdit.set_shadow_type(Gtk::SHADOW_IN);
	scrEdit.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	hbFileSelection.set_border_width(5);
	hbFileSelection.set_spacing(5);

	scrFiles.set_min_content_width(200);
	scrFiles.set_shadow_type(Gtk::SHADOW_IN);
	scrFiles.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	//theme selection
	hbTheme.set_spacing(5);
	fcThemeFileChooser.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	fcThemeFileChooser.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_APPLY);

	//color chooser
	vbCustomTheme.pack_start(groupColorChooser, Gtk::PACK_SHRINK);
	groupColorChooser.add(alignColorChooser);
	groupColorChooser.set_label_widget(lblColorChooser);
	groupColorChooser.set_shadow_type(Gtk::SHADOW_NONE);
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

	//font selection and background image group
	vbCustomTheme.pack_start(hbFontAndBgImage, Gtk::PACK_SHRINK);
	hbFontAndBgImage.set_spacing(5);
	hbFontAndBgImage.set_homogeneous(true);

	//font selection
	hbFontAndBgImage.pack_start(groupFont);
	groupFont.add(alignFont);
	groupFont.set_label_widget(lblFont);
	groupFont.set_shadow_type(Gtk::SHADOW_NONE);
	lblFont.set_mnemonic_widget(bttFont);
	alignFont.add(hbFont);
	hbFont.pack_start(bttFont);
	hbFont.pack_start(bttRemoveFont, Gtk::PACK_SHRINK);
	bttRemoveFont.add(imgRemoveFont);
	bttRemoveFont.set_tooltip_text(gettext("remove font"));
	bttRemoveFont.set_no_show_all(true);

	//background selection
	hbFontAndBgImage.pack_start(groupBackgroundImage);
	groupBackgroundImage.set_shadow_type(Gtk::SHADOW_NONE);
	groupBackgroundImage.add(alignBackgroundImage);
	groupBackgroundImage.set_label_widget(lblBackgroundImage);
	alignBackgroundImage.add(vbBackgroundImage);
	vbBackgroundImage.pack_start(hbBackgroundImage, Gtk::PACK_SHRINK);
	hbBackgroundImage.pack_start(fcBackgroundImage);
	hbBackgroundImage.pack_start(bttRemoveBackground, Gtk::PACK_SHRINK);
	fcBackgroundImage.set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);

	vbCustomTheme.pack_start(hbImgBtts);
	vbCustomTheme.pack_start(lblBackgroundRequiredInfo);
	hbImgBtts.pack_start(drwBackgroundPreview);

	hbImgBtts.set_spacing(5);
	vbButtons.set_spacing(5);

	bttRemoveBackground.set_tooltip_text(gettext("remove background"));
	bttRemoveBackground.add(imgRemoveBackground);
	bttRemoveBackground.set_no_show_all(true);
	lblBackgroundRequiredInfo.set_no_show_all(true);



	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	this->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_APPLY);

	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileAddClick));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileRemoveClick));
	lvFiles.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileSelected));
	lvFiles.get_model()->signal_row_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileRenamed));
	fcFileSelection.signal_file_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileChosen));
	txtEdit.get_buffer()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_textChanged));
	cbTheme.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_themeChosen));
	bttAddTheme.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_addThemeClicked));
	fcThemeFileChooser.signal_response().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_themeFileChooserResponse));
	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_dialogResponse));

	gccNormalForeground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccNormalForeground));
	gccNormalBackground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccNormalBackground));
	gccHighlightForeground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccHighlightForeground));
	gccHighlightBackground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccHighlightBackground));
	bttFont.signal_font_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_font_changed));
	bttRemoveFont.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_font_removed));
	fcBackgroundImage.signal_file_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_other_image_chosen));
	bttRemoveBackground.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_bttRemoveBackground_clicked));

	drwBackgroundPreview.signal_draw().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_redraw_preview));
}

View_Gtk_Theme_ColorChooser::Columns::Columns(){
	this->add(this->idName);
	this->add(this->name);
	this->add(this->colorCode_background);
	this->add(this->colorCode_foreground);
}

View_Gtk_Theme_ColorChooser::View_Gtk_Theme_ColorChooser()
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
void View_Gtk_Theme_ColorChooser::addColor(std::string const& codeName, std::string const& outputName, std::string const& cell_background, std::string const& cell_foreground){
	this->event_lock = true;
	Gtk::TreeModel::iterator iter = refListStore->append();
	(*iter)[columns.idName] = codeName;
	(*iter)[columns.name] = outputName;
	(*iter)[columns.colorCode_background] = cell_background;
	(*iter)[columns.colorCode_foreground] = cell_foreground;
	this->event_lock = false;
}
void View_Gtk_Theme_ColorChooser::selectColor(std::string const& codeName){
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
std::string View_Gtk_Theme_ColorChooser::getSelectedColor() const {
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter)
		return (Glib::ustring)(*iter)[columns.idName];
	else
		return "";
}

Pango::Color View_Gtk_Theme_ColorChooser::getSelectedColorAsPangoObject() const {
	Pango::Color color;
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter) {
		color.parse((Glib::ustring)(*iter)[columns.colorCode_background]);
	}
	return color;
}

GrubColorChooser::GrubColorChooser(bool blackIsTransparent) : View_Gtk_Theme_ColorChooser() {
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


std::string View_Gtk_Theme::_getSelectedFileName() {
	std::vector<int> selectedFiles = this->lvFiles.get_selected();
	std::string result;
	if (selectedFiles.size() == 1) {
		result = this->lvFiles.get_text(selectedFiles[0]);
	} else {
		throw ItemNotFoundException("theme editor: invalid selection count", __FILE__, __LINE__);
	}
	return result;
}

void View_Gtk_Theme::setEventListener(ThemeController& eventListener) {
	this->eventListener = &eventListener;
}


void View_Gtk_Theme::addFile(std::string const& fileName) {
	event_lock = true;
	lvFiles.append(fileName);
	event_lock = false;
}

void View_Gtk_Theme::clear() {
	event_lock = true;
	lvFiles.clear_items();
	event_lock = false;
}

void View_Gtk_Theme::setText(std::string const& text) {
	event_lock = true;
	imgPreview.hide();
	txtEdit.show();
	txtEdit.get_buffer()->set_text(text);
	event_lock = false;
}

void View_Gtk_Theme::setImage(std::string const& path) {
	event_lock = true;
	txtEdit.hide();
	imgPreview.show();
	imgPreview.set(path);
	event_lock = false;
}

void View_Gtk_Theme::selectFile(std::string const& fileName, bool startEdit) {
	lvFiles.get_selection()->unselect_all();
	int pos = 0;
	for (Gtk::TreeModel::iterator iter = lvFiles.get_model()->get_iter("0"); iter; iter++) {
		if (lvFiles.get_text(pos) == fileName) {
			lvFiles.set_cursor(lvFiles.get_model()->get_path(iter), *lvFiles.get_column(0), startEdit);
			break;
		}
		pos++;
	}
}

void View_Gtk_Theme::selectTheme(std::string const& name) {
	this->event_lock = true;
	cbTheme.set_active_text(name);
	this->event_lock = false;
}

std::string View_Gtk_Theme::getSelectedTheme() {
	return cbTheme.get_active_text();
}

void View_Gtk_Theme::addTheme(std::string const& name) {
	this->cbTheme.append(name);
}

void View_Gtk_Theme::clearThemeSelection() {
	event_lock = true;
	this->cbTheme.remove_all();

	cbTheme.append(gettext("(Custom Settings)"));
	cbTheme.set_active(0);
	event_lock = false;
}

void View_Gtk_Theme::show(bool burgMode) {
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

void View_Gtk_Theme::setEditorType(EditorType type) {
	this->vbCustomTheme.hide();
	this->hpThemeEditor.hide();

	switch (type) {
	case EDITORTYPE_CUSTOM:
		this->vbCustomTheme.show();
		this->vbCustomTheme.show_all_children(true);
		break;
	case EDITORTYPE_THEME:
		this->hpThemeEditor.show();
		this->hpThemeEditor.show_all_children(true);
		break;
	default:
		throw LogicException("unsupported type given", __FILE__, __LINE__);
	}
}

void View_Gtk_Theme::showThemeFileChooser() {
	fcThemeFileChooser.show_all();
}

void View_Gtk_Theme::showError(Error const& e) {
	switch (e) {
	case ERROR_INVALID_THEME_PACK_FORMAT:
		Gtk::MessageDialog(gettext("The chosen file cannot be loaded as theme")).run();
		break;
	case ERROR_RENAME_CONFLICT:
		Gtk::MessageDialog(gettext("The given filename cannot be used")).run();
		break;
	default:
		throw NotImplementedException("the current value of View_Theme::Error is not processed", __FILE__, __LINE__);
	}
}

void View_Gtk_Theme::setCurrentExternalThemeFilePath(std::string const& fileName) {
	this->event_lock = true;
	if (fileName == "") {
		this->fcFileSelection.unselect_all();
	} else {
		this->fcFileSelection.set_filename(fileName);
	}

	this->event_lock = false;
}

std::string View_Gtk_Theme::getDefaultName() const {
	return "[" + std::string(gettext("filename")) + "]";
}

View_ColorChooser& View_Gtk_Theme::getColorChooser(ColorChooserType type){
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

std::string View_Gtk_Theme::getFontName() {
	return bttFont.get_font_name();
}

int View_Gtk_Theme::getFontSize() {
	Pango::FontDescription desc(bttFont.get_font_name());
	return desc.get_size() / 1024;
}

void View_Gtk_Theme::setFontName(std::string const& value) {
	bttFont.set_font_name(value);
	bttRemoveFont.set_visible(value != "");
	imgRemoveFont.set_visible(value != "");
}

Glib::RefPtr<Pango::Layout> View_Gtk_Theme::createFormattedText(Cairo::RefPtr<Cairo::Context>& context, Glib::ustring const& text, std::string const& format, int r, int g, int b, int r_b, int g_b, int b_b, bool black_bg_is_transparent) {
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

void View_Gtk_Theme::setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir){
	this->redraw(menuPicturePath, isInGrubDir);
}

void View_Gtk_Theme::redraw(std::string const& menuPicturePath, bool isInGrubDir, Cairo::RefPtr<Cairo::Context> const* cr){
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
				Gdk::Cairo::set_source_pixbuf(context, buf);
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
						exampleTexts.push_back(View_Gtk_Theme::createFormattedText(context, *iter, fontName, fg_s.get_red() / 255, fg_s.get_green() / 255, fg_s.get_blue() / 255, bg_s.get_red() / 255, bg_s.get_green() / 255, bg_s.get_blue() / 255));
					} else {
						exampleTexts.push_back(View_Gtk_Theme::createFormattedText(context, *iter, fontName, fg_n.get_red() / 255, fg_n.get_green() / 255, fg_n.get_blue() / 255, bg_n.get_red() / 255, bg_n.get_green() / 255, bg_n.get_blue() / 255));
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
			Glib::RefPtr<Gdk::Pixbuf> buf = drwBackgroundPreview.render_icon_pixbuf(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_DIALOG);

			Gdk::Cairo::set_source_pixbuf(context, buf);
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

std::string View_Gtk_Theme::getBackgroundImagePath(){
	return fcBackgroundImage.get_filename();
}

void View_Gtk_Theme::setPreviewEntryTitles(std::list<std::string> const& entries) {
	this->previewEntryTitles_mutex.lock();
	this->previewEntryTitles = entries;
	this->previewEntryTitles_mutex.unlock();
}

bool View_Gtk_Theme::signal_redraw_preview(const Cairo::RefPtr<Cairo::Context>& cr) {
	if (!event_lock) {
		this->redraw(this->backgroundImagePath, false, &cr);
	}
	return true;
}


void View_Gtk_Theme::signal_fileAddClick() {
	if (!event_lock) {
		this->eventListener->addFileAction();
	}
}

void View_Gtk_Theme::signal_fileRemoveClick() {
	if (!event_lock) {
		try {
			this->eventListener->removeFileAction(this->_getSelectedFileName());
		} catch (ItemNotFoundException const& e) {
			this->log("no file selected - ignoring event", Logger::ERROR);
		}
	}
}

void View_Gtk_Theme::signal_fileSelected() {
	if (!event_lock) {
		try {
			this->eventListener->updateEditAreaAction(this->_getSelectedFileName());
		} catch (ItemNotFoundException const& e) {
			this->log("no file selected - ignoring event", Logger::INFO);
		}
	}
}

void View_Gtk_Theme::signal_fileRenamed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	if (!event_lock) {
		try {
			this->eventListener->renameAction(this->_getSelectedFileName());
		} catch (ItemNotFoundException const& e) {
			this->log("no file selected - ignoring event", Logger::ERROR);
		}
	}
}

void View_Gtk_Theme::signal_fileChosen() {
	if (!event_lock) {
		this->eventListener->loadFileAction(fcFileSelection.get_filename());
	}
}

void View_Gtk_Theme::signal_textChanged() {
	if (!event_lock) {
		this->eventListener->saveTextAction(txtEdit.get_buffer()->get_text());
	}
}

void View_Gtk_Theme::signal_themeChosen() {
	if (!event_lock) {
		if (this->cbTheme.get_active_row_number() == 0) {
			this->eventListener->showSimpleThemeConfigAction();
		} else {
			this->eventListener->loadThemeAction(cbTheme.get_active_text());
		}
	}
}

void View_Gtk_Theme::signal_addThemeClicked() {
	if (!event_lock) {
		this->eventListener->showThemeInstallerAction();
	}
}

void View_Gtk_Theme::signal_themeFileChooserResponse(int response_id) {
	if (!event_lock) {
		this->fcThemeFileChooser.hide();
		if (response_id == Gtk::RESPONSE_APPLY) {
			this->eventListener->addThemePackageAction(fcThemeFileChooser.get_filename());
		}
	}
}

void View_Gtk_Theme::signal_dialogResponse(int response_id) {
	if (!event_lock) {
		switch (response_id) {
		case Gtk::RESPONSE_DELETE_EVENT:
		case Gtk::RESPONSE_CLOSE:
			this->hide();
			break;
		case Gtk::RESPONSE_APPLY:
			this->eventListener->saveAction();
			break;
		default:
			throw NotImplementedException("the given response id is not supported", __FILE__, __LINE__);
		}
	}
}

void View_Gtk_Theme::signal_color_changed(View_Gtk_Theme_ColorChooser& caller){
	if (!event_lock && !caller.event_lock){
		this->eventListener->updateColorSettingsAction();
	}
}

void View_Gtk_Theme::signal_font_changed() {
	if (!event_lock) {
		this->eventListener->updateFontSettingsAction(false);
	}
}

void View_Gtk_Theme::signal_font_removed() {
	if (!event_lock) {
		this->eventListener->updateFontSettingsAction(true);
	}
}

void View_Gtk_Theme::signal_bttRemoveBackground_clicked(){
	if (!event_lock){
		this->eventListener->removeBackgroundImageAction();
	}
}


void View_Gtk_Theme::signal_other_image_chosen(){
	if (!event_lock){
		this->eventListener->updateBackgroundImageAction();
	}
}

