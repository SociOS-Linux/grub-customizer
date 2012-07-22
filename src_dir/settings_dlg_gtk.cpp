#include "settings_dlg_gtk.h"

ColorChooser::Columns::Columns(){
	this->add(this->idName);
	this->add(this->name);
	this->add(this->colorCode_background);
	this->add(this->colorCode_foreground);
}

ColorChooser::ColorChooser(){
	refListStore = Gtk::ListStore::create(columns);
	this->set_model(refListStore);
	
	this->pack_start(columns.name);
	
	Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = this->get_cells();
	Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();
	
	this->add_attribute(*cellRenderer, "background", columns.colorCode_background);
	this->add_attribute(*cellRenderer, "foreground", columns.colorCode_foreground);
}
void ColorChooser::addColor(Glib::ustring const& codeName, Glib::ustring const& outputName, Glib::ustring const& cell_background, Glib::ustring const& cell_foreground){
	Gtk::TreeModel::iterator iter = refListStore->append();
	(*iter)[columns.idName] = codeName;
	(*iter)[columns.name] = outputName;
	(*iter)[columns.colorCode_background] = cell_background;
	(*iter)[columns.colorCode_foreground] = cell_foreground;
}
void ColorChooser::selectColor(Glib::ustring const& codeName){
	this->set_active(0);
	for (Gtk::TreeModel::iterator iter = this->get_active(); iter; iter++){
		if ((*iter)[columns.idName] == codeName){
			this->set_active(iter);
			break;
		}
	}
}
Glib::ustring ColorChooser::getSelectedColor() const {
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter)
		return (*iter)[columns.idName];
	else
		return "";
}

GrubColorChooser::GrubColorChooser() : ColorChooser() {
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
	this->addColor("black",          gettext("black"),         "#000000", "#ffffff");
}


GrubSettingsDlgGtk::GrubSettingsDlgGtk(SettingsManagerDataStore& dataStore, std::list<std::string> const& entryList, bool burgMode, std::string const& output_config_dir)
	: dataStore(&dataStore), event_lock(false), output_config_dir(output_config_dir),
	rbDefPredefined(gettext("pre_defined: "), true), rbDefSaved(gettext("previously _booted entry"), true),
	lblDefaultEntry(gettext("default entry")), lblView(gettext("visibility")), chkShowMenu(gettext("show menu")),
	lblTimeout(gettext("Timeout")), lblTimeout2(gettext("Seconds")), lblKernelParams(gettext("kernel parameters")),
	chkGenerateRecovery(gettext("generate recovery entries")), chkOsProber(gettext("look for other operating systems")),
	chkResolution(gettext("custom resolution: ")),
	lblforegroundColor(gettext("font color")), lblBackgroundColor(gettext("background")),
	lblNormalColor(gettext("normal:"), Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER), lblHighlightColor(gettext("highlight:"), Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER),
	lblColorChooser(gettext("menu colors")), lblBackgroundImage(gettext("background image")),
	bttCopyBackground(gettext("copy to grub directory")), bttRemoveBackground(gettext("remove background")),
	lblBackgroundRequiredInfo(gettext("To get the colors above working,\nyou have to select a background image!"))
{
	this->set_title("Grub Customizer - "+Glib::ustring(gettext("settings")));
	this->set_icon_name("grub-customizer");
	Gtk::VBox* winBox = this->get_vbox();
	winBox->add(tabbox);
	tabbox.append_page(vbCommonSettings, gettext("_General"), true);
	tabbox.append_page(vbAppearanceSettings, gettext("A_ppearance"), true);
	tabbox.append_page(scrAllEntries, gettext("_Advanced"), true);
	
	scrAllEntries.add(tvAllEntries);
	scrAllEntries.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	refAsListStore = Gtk::ListStore::create(asTreeModel);
	tvAllEntries.set_model(refAsListStore);
	tvAllEntries.append_column_editable(gettext("is active"), asTreeModel.active);
	tvAllEntries.append_column(gettext("name"), asTreeModel.name);
	tvAllEntries.append_column_editable(gettext("value"), asTreeModel.value);
	refAsListStore->signal_row_changed().connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::signal_setting_row_changed));
	vbCommonSettings.set_spacing(15);
	vbAppearanceSettings.set_spacing(15);
	
	//default entry group
	vbCommonSettings.pack_start(groupDefaultEntry, Gtk::PACK_SHRINK);
	groupDefaultEntry.add(alignDefaultEntry);
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
	guint index = 0;
	for (std::list<std::string>::const_iterator iter = entryList.begin(); iter != entryList.end(); iter++){
		//cbDefEntry.append_text(iter->length() > 25 ? iter->substr(0,22)+"…" : *iter);
		cbDefEntry.append_text(Glib::ustring::compose(gettext("Entry %1 (by position)"), ++index));
		cbDefEntry.append_text(*iter);
	}
	if (entryList.size() > 0)
		cbDefEntry.set_active(0);
	
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
	
	if (!burgMode){
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
		fcBackgroundImage.set_local_only(false);
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
	}
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
	
	//get framebuffer resolutions - threaded
	disp_fb_resolutions_loaded.connect(sigc::mem_fun(this, &GrubSettingsDlgGtk::disp_func_fb_resolutions_loaded));
	fbResolutions.connectUI(*this);
	Glib::Thread::create(sigc::mem_fun(&fbResolutions, &FbResolutionsGetter::load), false);
	
	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	this->set_default_size(300, 400);
	this->show_all();
}

GrubSettingsDlgGtk::AdvancedSettingsTreeModel::AdvancedSettingsTreeModel(){
	this->add(active);
	this->add(name);
	this->add(value);
}

void GrubSettingsDlgGtk::loadData(){
	if (!event_lock){
		event_lock = true;
		refAsListStore->clear();
		for (std::list<SettingRow>::iterator iter = dataStore->begin(); iter != dataStore->end(); dataStore->iter_to_next_setting(iter)){
			Gtk::TreeModel::iterator newItemIter = refAsListStore->append();
			(*newItemIter)[asTreeModel.active] = iter->isActive;
			(*newItemIter)[asTreeModel.name] = iter->name;
			(*newItemIter)[asTreeModel.value] = iter->value;
		}
		std::string defEntry = dataStore->getValue("GRUB_DEFAULT");
		if (defEntry == "saved"){
			rbDefSaved.set_active(true);
		} /*else if (defEntry.find_first_not_of("0123456789") == std::string::npos){ //test if every character is numeric…
			rbDefByPosition.set_active(true);
			std::istringstream in(defEntry);
			int defEntryNum;
			in >> defEntryNum;
			spDefPosition.set_value(defEntryNum);
		} */else {
			if (defEntry.find_first_not_of("0123456789") == std::string::npos){
				std::istringstream in(defEntry);
				int defEntryNum;
				in >> defEntryNum;
				
				cbDefEntry.set_active(defEntryNum*2); //set active element by index, "*2" is required because there are two columns
			}
			else
				cbDefEntry.set_active_text(defEntry);
			rbDefPredefined.set_active(true);
		}
		
		chkShowMenu.set_active(!dataStore->isActive("GRUB_HIDDEN_TIMEOUT", true));
		chkOsProber.set_active(!dataStore->isActive("GRUB_DISABLE_OS_PROBER", true));
		
		std::string timeoutStr = dataStore->getValue("GRUB_TIMEOUT");
		std::istringstream in(timeoutStr);
		int timeout;
		in >> timeout;
		spTimeout.set_value(timeout);
		
		txtKernelParams.set_text(dataStore->getValue("GRUB_CMDLINE_LINUX_DEFAULT"));
		chkGenerateRecovery.set_active(!dataStore->isActive("GRUB_DISABLE_LINUX_RECOVERY", true));
		
		cbResolution.set_sensitive(dataStore->isActive("GRUB_GFXMODE", true));
		chkResolution.set_active(dataStore->isActive("GRUB_GFXMODE", true));
		cbResolution.get_entry()->set_text(dataStore->getValue("GRUB_GFXMODE"));

		Glib::ustring nColor = dataStore->getValue("GRUB_COLOR_NORMAL");
		Glib::ustring hColor = dataStore->getValue("GRUB_COLOR_HIGHLIGHT");
		if (nColor != ""){
			gccNormalForeground.selectColor(nColor.substr(0, nColor.find('/')));
			gccNormalBackground.selectColor(nColor.substr(nColor.find('/')+1));
		}
		else {
			//default grub menu colors
			gccNormalForeground.selectColor("white");
			gccNormalBackground.selectColor("black");
		}
		if (hColor != ""){
			gccHighlightForeground.selectColor(hColor.substr(0, hColor.find('/')));
			gccHighlightBackground.selectColor(hColor.substr(hColor.find('/')+1));
		}
		else {
			//default grub menu colors
			gccHighlightForeground.selectColor("black");
			gccHighlightBackground.selectColor("light-gray");
		}

		std::string menuPicturePath = this->dataStore->getValue("GRUB_MENU_PICTURE");
		bool menuPicIsInGrubDir = false;
		if (menuPicturePath != "" && menuPicturePath[0] != '/'){
			menuPicturePath = output_config_dir + "/" + menuPicturePath;
			menuPicIsInGrubDir = true;
		}
		
		if (dataStore->isActive("GRUB_MENU_PICTURE") && menuPicturePath != ""){
		
			FILE* img_test = fopen(menuPicturePath.c_str(), "r");
			if (img_test){
				fclose(img_test);
				
				
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
		}
		else {
			fcBackgroundImage.unselect_all();
			imgBackgroundImage.clear();
			bttCopyBackground.hide();
			bttRemoveBackground.hide();
			imgBackgroundImage.hide();
			lblBackgroundRequiredInfo.show();
		}
		
		bttCopyBackground.set_sensitive(!menuPicIsInGrubDir);
		

		event_lock = false;
	}
}

void GrubSettingsDlgGtk::event_fb_resolutions_loaded(){
	disp_fb_resolutions_loaded();
}

void GrubSettingsDlgGtk::disp_func_fb_resolutions_loaded(){
	const std::list<std::string>& resolutions = fbResolutions.getData();
	for (std::list<std::string>::const_iterator iter = resolutions.begin(); iter != resolutions.end(); iter++)
		cbResolution.append_text(*iter);
}

void GrubSettingsDlgGtk::signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (!event_lock){
		event_lock = true;
		dataStore->setValue((Glib::ustring)(*iter)[asTreeModel.name], (Glib::ustring)(*iter)[asTreeModel.value]);
		dataStore->setIsActive((Glib::ustring)(*iter)[asTreeModel.name], (*iter)[asTreeModel.active]);
		event_lock = false;
		loadData();
	}
}

void GrubSettingsDlgGtk::signal_default_entry_predefined_toggeled(){
	cbDefEntry.set_sensitive(rbDefPredefined.get_active());
	if (rbDefPredefined.get_active()) //force usage of the combobox value
		signal_default_entry_changed();
}

void GrubSettingsDlgGtk::signal_default_entry_saved_toggeled(){
	if (rbDefSaved.get_active()){
		dataStore->setValue("GRUB_DEFAULT", "saved");
		dataStore->setValue("GRUB_SAVEDEFAULT", "true");
		dataStore->setIsActive("GRUB_SAVEDEFAULT", true);
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_default_entry_changed(){
	if (!event_lock){
		if ((cbDefEntry.get_active_row_number() % 2) == 0) //index selected
			dataStore->setValue("GRUB_DEFAULT", Glib::ustring::format(cbDefEntry.get_active_row_number() / 2));
		else //name selected
			dataStore->setValue("GRUB_DEFAULT", cbDefEntry.get_active_text());
		dataStore->setValue("GRUB_SAVEDEFAULT", "false");
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_showMenu_toggled(){
	if (!event_lock){
		dataStore->setIsActive("GRUB_HIDDEN_TIMEOUT", !chkShowMenu.get_active());
		signal_timeout_changed();
		if (!chkShowMenu.get_active() && chkOsProber.get_active()){
			Gtk::MessageDialog(Glib::ustring::compose(gettext("This option doesn't work when the \"os-prober\" script finds other operating systems. Disable \"%1\" if you don't need to boot other operating systems."), chkOsProber.get_label())).run();
		}
		this->loadData();
	}
}
void GrubSettingsDlgGtk::signal_osProber_toggled(){
	if (!event_lock){
		dataStore->setValue("GRUB_DISABLE_OS_PROBER", chkOsProber.get_active() ? "false" : "true");
		dataStore->setIsActive("GRUB_DISABLE_OS_PROBER", !chkOsProber.get_active());
		this->loadData();
	}
}
void GrubSettingsDlgGtk::signal_timeout_changed(){
	if (!event_lock){
		if (chkShowMenu.get_active()){
			dataStore->setValue("GRUB_TIMEOUT", Glib::ustring::format(spTimeout.get_value_as_int()));
		}
		else {
			dataStore->setValue("GRUB_HIDDEN_TIMEOUT", Glib::ustring::format(spTimeout.get_value_as_int()));
		}
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_kernelparams_changed(){
	if (!event_lock){
		dataStore->setValue("GRUB_CMDLINE_LINUX_DEFAULT", txtKernelParams.get_text());
		this->loadData();
	}
}
void GrubSettingsDlgGtk::signal_recovery_toggled(){
	if (!event_lock){
		dataStore->setIsActive("GRUB_DISABLE_LINUX_RECOVERY", !chkGenerateRecovery.get_active());
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_chkResolution_toggled(){
	if (!event_lock){
		dataStore->setIsActive("GRUB_GFXMODE", chkResolution.get_active());
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_resolution_selected(){
	if (!event_lock){
		dataStore->setValue("GRUB_GFXMODE", cbResolution.get_entry()->get_text());
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_color_changed(){
	if (!event_lock){
		if (gccNormalForeground.getSelectedColor() != "" && gccNormalBackground.getSelectedColor() != ""){
			dataStore->setValue("GRUB_COLOR_NORMAL", gccNormalForeground.getSelectedColor() + "/" + gccNormalBackground.getSelectedColor());
			dataStore->setIsActive("GRUB_COLOR_NORMAL", true);
			dataStore->setIsExport("GRUB_COLOR_NORMAL", true);
		}
		if (gccHighlightForeground.getSelectedColor() != "" && gccHighlightBackground.getSelectedColor() != ""){
			dataStore->setValue("GRUB_COLOR_HIGHLIGHT", gccHighlightForeground.getSelectedColor() + "/" + gccHighlightBackground.getSelectedColor());
			dataStore->setIsActive("GRUB_COLOR_HIGHLIGHT", true);
			dataStore->setIsExport("GRUB_COLOR_HIGHLIGHT", true);
		}
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_bttCopyBackground_clicked(){
	if (!event_lock){
		Glib::RefPtr<Gio::File> file_src = fcBackgroundImage.get_file();
		Glib::RefPtr<Gio::File> file_dest = Gio::File::create_for_path(this->output_config_dir+"/"+file_src->get_basename());
		file_src->copy(file_dest, Gio::FILE_COPY_OVERWRITE);
		
		dataStore->setValue("GRUB_MENU_PICTURE", file_src->get_basename()); //The path isn't required when the image is in grub conf dir
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_bttRemoveBackground_clicked(){
	if (!event_lock){
		dataStore->setIsActive("GRUB_MENU_PICTURE", false);
		this->loadData();
	}
}

void GrubSettingsDlgGtk::signal_other_image_chosen(){
	if (!event_lock){
		dataStore->setValue("GRUB_MENU_PICTURE", fcBackgroundImage.get_filename());
		dataStore->setIsActive("GRUB_MENU_PICTURE", true);
		dataStore->setIsExport("GRUB_MENU_PICTURE", true);
		this->loadData();
	}
}

