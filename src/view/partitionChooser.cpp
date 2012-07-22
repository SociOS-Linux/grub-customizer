#include "partitionChooser.h"

PartitionChooser::PartitionChooser(bool isLiveCD)
	: lvRootPartitionSelection(3),
	chkCustomPartition(gettext("_use another partition: "), true),
	bttMountFs(gettext("Mount selected Filesystem")),
	bttUmountFs(gettext("Unmount mounted Filesystem")),
	submountpoint_toggle_run_event(true),
	is_cancelled(false),
	lblSubmountpointDescription(gettext("These are the mountpoints of your fstab file.\nPlease select every grub/boot related partition."), Gtk::ALIGN_LEFT),
	isMounted(false)
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
	this->set_page_title(vbAdditionalMountSelectionPage, gettext("Select required submountpoints"));
	vbAdditionalMountSelectionPageList.set_border_width(10);
	this->set_page_type(vbAdditionalMountSelectionPage, Gtk::ASSISTANT_PAGE_CONFIRM);

	this->set_title(Glib::ustring("Grub Customizer: ")+gettext("Partition Chooser"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(640, 480);
}

void PartitionChooser::setEventListener(EventListenerView_iface& eventListener){
	this->eventListener = &eventListener;
}

void PartitionChooser::addPartitionSelectorItem(Glib::ustring const& device, Glib::ustring const& type, Glib::ustring const& label){
	guint index = lvRootPartitionSelection.append_text(device);
	lvRootPartitionSelection.set_text(index, 1, type);
	lvRootPartitionSelection.set_text(index, 2, label);
}

void PartitionChooser::clearPartitionSelector(){
	lvRootPartitionSelection.clear_items();
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

void PartitionChooser::showErrorMessage(MountException::Type type){
	switch (type){
		case MountException::MOUNT_FAILED:       Gtk::MessageDialog(gettext("Mount failed!")).run(); break;
		case MountException::MOUNT_ERR_NO_FSTAB: Gtk::MessageDialog(gettext("This seems not to be a root file system (no fstab found)")).run();
	}
}



void PartitionChooser::signal_btt_mount_click(){
	this->eventListener->rootFsMount_request();
}


void PartitionChooser::updateSensitivity(){
	set_page_complete(vbIntroPage, true);
	set_page_complete(vbAdditionalMountSelectionPage, true);
	bttUmountFs.set_sensitive(isMounted);
	bttMountFs.set_sensitive(!isMounted && (chkCustomPartition.get_active() && txtCustomPartition.get_text_length() || !chkCustomPartition.get_active() && lvRootPartitionSelection.get_selected().size()));
	lvRootPartitionSelection.set_sensitive(!isMounted && !chkCustomPartition.get_active());
	txtCustomPartition.set_sensitive(!isMounted && chkCustomPartition.get_active());
	chkCustomPartition.set_sensitive(!isMounted);
	set_page_complete(vbRootSelectPage, isMounted);
}


void PartitionChooser::on_cancel(){
	this->eventListener->partitionChooser_cancelled();
}


void PartitionChooser::on_apply(){
	this->eventListener->partitionChooser_applied();
}

Gtk::CheckButton& PartitionChooser::getSubmountpointCheckboxByLabel(Glib::ustring const& label){
	Glib::ListHandle< Widget* > allChilds = vbAdditionalMountSelectionPageList.get_children();
	for (Glib::ListHandle<Widget*>::iterator iter = allChilds.begin(); iter != allChilds.end(); iter++){
		if (((Gtk::CheckButton*)*iter)->get_label() == label)
			return (Gtk::CheckButton&)**iter;
	}
	throw "Checkbutton not found"; //TODO: use object or enum
}

void PartitionChooser::setSubmountpointSelectionState(Glib::ustring const& submountpoint, bool new_isSelected){
	Gtk::CheckButton& target = this->getSubmountpointCheckboxByLabel(submountpoint);
	submountpoint_toggle_run_event = false;
	target.set_active(new_isSelected);
	submountpoint_toggle_run_event = true;
}



void PartitionChooser::submountpoint_toggle(Gtk::CheckButton& sender){
	if (submountpoint_toggle_run_event){
		if (sender.get_active())
			this->eventListener->submountpoint_mount_request(sender.get_label());
		else
			this->eventListener->submountpoint_umount_request(sender.get_label());
	}
}


void PartitionChooser::run(){
	this->updateSensitivity();
	this->show_all();
	Gtk::Main::run(*this);
}

void PartitionChooser::setIsMounted(bool isMounted){
	this->isMounted = isMounted;
}

void PartitionChooser::signal_btt_umount_click(){
	this->eventListener->rootFsUmount_request();
}
