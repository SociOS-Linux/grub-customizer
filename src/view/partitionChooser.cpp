#include "partitionChooser.h"

PartitionChooser::PartitionChooser(bool isLiveCD)
	: lvRootPartitionSelection(3),
	chkCustomPartition(gettext("_use another partition: "), true),
	bttMountFs(gettext("Mount selected Filesystem")),
	bttUmountFs(gettext("Unmount mounted Filesystem")),
	submountpoint_toggle_run_event(true),
	is_cancelled(false),
	lblSubmountpointDescription(gettext("These are the mountpoints of your fstab file.\nPlease select every grub/boot related partition."), Gtk::ALIGN_LEFT)
{
	this->append_page(vbIntroPage);
	Glib::ustring message;
	if (isLiveCD)
		message = gettext("You started Grub Customizer using the live CD.");
	else
		message = gettext("You selected the option for choosing another partition.\nPlease note: The os-prober may not find your actually running system.\nSo run Grub Customizer on the target system\nagain and save the configuration (or run update-grub/update-burg)\nto get the entry back!");
	
	lblMessage.set_text(message+"\n\n"+gettext("Before you can edit your grub configuration we have to\nmount the required partitions.\n\nThis assistant will help you doing this.\n\n\nPlease ensure the target system is based on the same cpu architecture\nas the actually running one.\nIf not, you will get an error message when trying to load the configuration."));
	vbIntroPage.add(lblMessage);
	set_page_title(vbIntroPage, gettext("Grub Customizer: Partition chooser"));
	set_icon_name("grub-customizer");
	
	this->append_page(vbRootSelectPage);
	set_page_title(vbRootSelectPage, gettext("Select and mount your root partition"));
	vbRootSelectPage.pack_start(scrRootPartitionSelection);
	vbRootSelectPage.pack_start(hbCustomPartition, Gtk::PACK_SHRINK);
	vbRootSelectPage.pack_start(hbMountButtons, Gtk::PACK_SHRINK);
	
	scrRootPartitionSelection.add(lvRootPartitionSelection);
	scrRootPartitionSelection.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrRootPartitionSelection.set_shadow_type(Gtk::SHADOW_IN);
	
	hbCustomPartition.pack_start(chkCustomPartition, Gtk::PACK_SHRINK);
	hbCustomPartition.pack_start(txtCustomPartition);
	
	hbMountButtons.set_spacing(10);
	hbMountButtons.pack_end(bttUmountFs);
	hbMountButtons.pack_end(bttMountFs);
	
	vbRootSelectPage.set_spacing(5);
	
	lvRootPartitionSelection.set_column_title (0, gettext("partition"));
	lvRootPartitionSelection.set_column_title (1, gettext("type"));
	lvRootPartitionSelection.set_column_title (2, gettext("label"));
	
	vbAdditionalMountSelectionPage.pack_start(lblSubmountpointDescription, Gtk::PACK_SHRINK);
	vbAdditionalMountSelectionPage.set_spacing(5);
	vbAdditionalMountSelectionPage.set_border_width(5);
	vbAdditionalMountSelectionPage.pack_start(scrAdditionalMountSelectionPage);
	scrAdditionalMountSelectionPage.add(vbAdditionalMountSelectionPageList);
	scrAdditionalMountSelectionPage.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	chkCustomPartition.signal_toggled().connect(sigc::mem_fun(this, &PartitionChooser::signal_custom_partition_toggled));
	txtCustomPartition.signal_changed().connect(sigc::mem_fun(this, &PartitionChooser::signal_custom_partition_typing));
	bttMountFs.signal_clicked().connect(sigc::mem_fun(this, &PartitionChooser::signal_btt_mount_click));
	bttUmountFs.signal_clicked().connect(sigc::mem_fun(this, &PartitionChooser::signal_btt_umount_click));
	
	lvRootPartitionSelection.get_selection()->signal_changed().connect(sigc::mem_fun(this, &PartitionChooser::signal_lvRootPartitionSelection_changed));
	
	this->append_page(vbAdditionalMountSelectionPage);
	set_page_title(vbAdditionalMountSelectionPage, gettext("Select required submountpoints"));
	vbAdditionalMountSelectionPageList.set_border_width(10);
	set_page_type(vbAdditionalMountSelectionPage, Gtk::ASSISTANT_PAGE_CONFIRM);

	this->set_title(Glib::ustring("Grub Customizer: ")+gettext("Partition Chooser"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(640, 480);
}

void PartitionChooser::setEventListener(EventListenerView_iface& eventListener){
	this->eventListener = &eventListener;
}

void PartitionChooser::readPartitionInfo(){
	FILE* ddl_proc = popen("blkid", "r");
	DeviceDataList ddl(ddl_proc);
	for (DeviceDataList::iterator iter = ddl.begin(); iter != ddl.end(); iter++){
		guint index = lvRootPartitionSelection.append_text(iter->first);
		lvRootPartitionSelection.set_text(index, 1, iter->second["TYPE"]);
		lvRootPartitionSelection.set_text(index, 2, iter->second["LABEL"]);
	}
	pclose(ddl_proc);
}

void PartitionChooser::signal_custom_partition_toggled(){
	updateSensitivity();
}

void PartitionChooser::signal_lvRootPartitionSelection_changed(){
	updateSensitivity();
}

void PartitionChooser::signal_custom_partition_typing(){
	updateSensitivity();
}

//TODO: MOVE TO PRESENTER
void PartitionChooser::generateSubmountpointSelection(std::string const& prefix){
	this->removeAllSubmountpoints();

	//create new submountpoint checkbuttons
	for (MountTable::const_iterator iter = rootFstab.begin(); iter != rootFstab.end(); iter++){
		if (iter->mountpoint.length() > prefix.length()
		 && iter->mountpoint != prefix + "/dev"
		 && iter->mountpoint != prefix + "/proc"
		 && iter->mountpoint != prefix + "/sys"
		) {
			this->addSubmountpoint(iter->mountpoint.substr(prefix.length()), iter->isMounted);
		}
	}
}

void PartitionChooser::removeAllSubmountpoints(){
	//delete all existing submountpoint checkbuttons
	Glib::ListHandle< Widget* > allChilds = vbAdditionalMountSelectionPageList.get_children();
	for (Glib::ListHandle<Widget*>::iterator iter = allChilds.begin(); iter != allChilds.end(); iter++){
		vbAdditionalMountSelectionPageList.remove(**iter);
		delete &**iter;
	}
	this->set_page_type(vbRootSelectPage, Gtk::ASSISTANT_PAGE_CONFIRM);
}

void PartitionChooser::addSubmountpoint(std::string const& mountpoint, bool isMounted){
	Gtk::CheckButton* cb = new Gtk::CheckButton(mountpoint);
	cb->set_active(isMounted);
	cb->signal_toggled().connect(sigc::bind<Gtk::CheckButton&>(sigc::mem_fun(this, &PartitionChooser::submountpoint_toggle), *cb));

	vbAdditionalMountSelectionPageList.pack_start(*cb, Gtk::PACK_SHRINK);
	vbAdditionalMountSelectionPageList.hide(); //is required to see the checkboxes… I don't know why (rendering problem of gtk)
	vbAdditionalMountSelectionPageList.show_all();
	this->set_page_type(vbRootSelectPage, Gtk::ASSISTANT_PAGE_CONTENT);
}

std::string PartitionChooser::getSelectedDevice(){
	return chkCustomPartition.get_active() ? txtCustomPartition.get_text() : lvRootPartitionSelection.get_text(lvRootPartitionSelection.get_selected()[0],0);
}

void PartitionChooser::signal_btt_mount_click(){
	std::string selectedDevice = this->getSelectedDevice();
	mountpoint = PARTCHOOSER_MOUNTPOINT;
	mkdir(mountpoint.c_str(), 0755);
	try {
		rootFstab.clear(mountpoint);
		rootFstab.mountRootFs(selectedDevice, mountpoint);
		this->generateSubmountpointSelection(mountpoint);
	}
	catch (MountException e) {
		if (e.type == MountException::MOUNT_FAILED){
			Gtk::MessageDialog(gettext("Mount failed!")).run();
			mountpoint = "";
		}
		else if (e.type == MountException::MOUNT_ERR_NO_FSTAB){
			Gtk::MessageDialog(gettext("This seems not to be a root file system (no fstab found)")).run();
			rootFstab.getEntryByMountpoint(mountpoint).umount();
			mountpoint = "";
		}
	}
	updateSensitivity();
}

void PartitionChooser::signal_btt_umount_click(){
	try {
		this->rootFstab.umountAll(mountpoint);
		this->rootFstab.clear(mountpoint);
		mountpoint = "";
	}
	catch (MountException e){
		if (e.type == MountException::MOUNT_FAILED)
			Gtk::MessageDialog(gettext("umount failed!")).run();
	}
	updateSensitivity();
}

void PartitionChooser::updateSensitivity(){
	set_page_complete(vbIntroPage, true);
	set_page_complete(vbAdditionalMountSelectionPage, true);
	bttUmountFs.set_sensitive(mountpoint != "");
	bttMountFs.set_sensitive(mountpoint == "" && (chkCustomPartition.get_active() && txtCustomPartition.get_text_length() || !chkCustomPartition.get_active() && lvRootPartitionSelection.get_selected().size()));
	lvRootPartitionSelection.set_sensitive(mountpoint == "" && !chkCustomPartition.get_active());
	txtCustomPartition.set_sensitive(mountpoint == "" && chkCustomPartition.get_active());
	chkCustomPartition.set_sensitive(mountpoint == "");
	set_page_complete(vbRootSelectPage, mountpoint != "");
}


void PartitionChooser::on_cancel(){
	this->is_cancelled = true;
	this->hide();
}

void PartitionChooser::on_apply(){
	this->hide();
}

void PartitionChooser::submountpoint_toggle(Gtk::CheckButton& sender){
	if (submountpoint_toggle_run_event){
		try {
			Mountpoint& m = this->rootFstab.getEntryRefByMountpoint(mountpoint + sender.get_label());
			if (sender.get_active())
				m.mount();
			else
				m.umount();
		}
		catch (MountException e){
			if (e.type == MountException::MOUNT_FAILED){
				if (sender.get_active())
					Gtk::MessageDialog(gettext("Couldn't mount the selected partition")).run();
				else
					Gtk::MessageDialog(gettext("Couldn't umount the selected partition")).run();
			}
			submountpoint_toggle_run_event = false;
			sender.set_active(!sender.get_active()); //reset checkbox
			submountpoint_toggle_run_event = true;
		}
	}
}

std::string PartitionChooser::getRootMountpoint() const {
	return mountpoint;
}

bool PartitionChooser::isCancelled() const {
	return is_cancelled;
}

Glib::ustring PartitionChooser::run(){
	rootFstab.loadData("");
	rootFstab.loadData(PARTCHOOSER_MOUNTPOINT);
	if (rootFstab.getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT)){
		mountpoint = PARTCHOOSER_MOUNTPOINT;
		this->generateSubmountpointSelection(mountpoint);
	}
	this->readPartitionInfo();
	this->updateSensitivity();
	this->show_all();
	Gtk::Main::run(*this);
	if (this->isCancelled())
		return "";
	else
		return this->getRootMountpoint();
}



