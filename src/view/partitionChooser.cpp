#include "partitionChooser.h"

PartitionChooser::PartitionChooser(bool isLiveCD)
	: lvRootPartitionSelection(3),
	chkCustomPartition(gettext("_use another partition: "), true),
	bttMountFs(gettext("Mount selected Filesystem")),
	bttUmountFs(gettext("Unmount mounted Filesystem")),
	submountpoint_toggle_run_event(true),
	is_cancelled(false),
	lblSubmountpointDescription(gettext("These are the mountpoints of your fstab file.\nPlease select every grub/boot related partition."), Gtk::ALIGN_LEFT),
	isMounted(false), assistant(NULL)
{
	Glib::ustring message;
	if (isLiveCD)
		message = gettext("You started Grub Customizer using the live CD.");
	else
		message = gettext("You selected the option for choosing another partition.\nPlease note: The os-prober may not find your actually running system.\nSo run Grub Customizer on the target system\nagain and save the configuration (or run update-grub/update-burg)\nto get the entry back!");
	
	lblMessage.set_text(message+"\n\n"+gettext("Before you can edit your grub configuration we have to\nmount the required partitions.\n\nThis assistant will help you doing this.\n\n\nPlease ensure the target system is based on the same cpu architecture\nas the actually running one.\nIf not, you will get an error message when trying to load the configuration."));
	vbIntroPage.add(lblMessage);
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

	vbAdditionalMountSelectionPageList.set_border_width(10);
}

void PartitionChooser::setEventListener(EventListenerView_iface& eventListener){
	this->eventListener = &eventListener;
}

void PartitionChooser::init(bool useExisting){
	if (this->assistant && !this->assistant->is_visible() && !useExisting){
		delete this->assistant;
		this->assistant = NULL;
	}
	if (!this->assistant){
		this->assistant = new Gtk::Assistant;

		this->assistant->append_page(vbIntroPage);
		this->assistant->set_page_title(vbIntroPage, gettext("Grub Customizer: Partition chooser"));
		this->assistant->set_icon_name("grub-customizer");

		this->assistant->append_page(vbRootSelectPage);
		this->assistant->set_page_title(vbRootSelectPage, gettext("Select and mount your root partition"));
		this->assistant->append_page(vbAdditionalMountSelectionPage);
		this->assistant->set_page_title(vbAdditionalMountSelectionPage, gettext("Select required submountpoints"));
		this->assistant->set_page_type(vbAdditionalMountSelectionPage, Gtk::ASSISTANT_PAGE_CONFIRM);

		this->assistant->signal_cancel().connect(sigc::mem_fun(this, &PartitionChooser::on_cancel));
		this->assistant->signal_apply().connect(sigc::mem_fun(this, &PartitionChooser::on_apply));

		this->assistant->set_title(Glib::ustring("Grub Customizer: ")+gettext("Partition Chooser"));
		this->assistant->set_icon_name("grub-customizer");
		this->assistant->set_default_size(640, 480);
	}
}

Gtk::Assistant& PartitionChooser::getWindow(){
	this->init();
	return *this->assistant;
}

void PartitionChooser::addPartitionSelectorItem(Glib::ustring const& device, Glib::ustring const& type, Glib::ustring const& label){
	guint index = lvRootPartitionSelection.append_text(device);
	lvRootPartitionSelection.set_text(index, 1, type);
	lvRootPartitionSelection.set_text(index, 2, label);
}

void PartitionChooser::clearPartitionSelector(){
	lvRootPartitionSelection.clear_items();
}

void PartitionChooser::hide(){
	this->getWindow().hide();
}
void PartitionChooser::show(){
	this->getWindow().show_all();
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
	Glib::ListHandle< Gtk::Widget* > allChilds = vbAdditionalMountSelectionPageList.get_children();
	for (Glib::ListHandle<Gtk::Widget*>::iterator iter = allChilds.begin(); iter != allChilds.end(); iter++){
		vbAdditionalMountSelectionPageList.remove(**iter);
		delete &**iter;
	}
	this->getWindow().set_page_type(vbRootSelectPage, Gtk::ASSISTANT_PAGE_CONFIRM);
}

void PartitionChooser::addSubmountpoint(std::string const& mountpoint, bool isMounted){
	Gtk::CheckButton* cb = new Gtk::CheckButton(mountpoint);
	cb->set_active(isMounted);
	cb->signal_toggled().connect(sigc::bind<Gtk::CheckButton&>(sigc::mem_fun(this, &PartitionChooser::submountpoint_toggle), *cb));

	vbAdditionalMountSelectionPageList.pack_start(*cb, Gtk::PACK_SHRINK);
	vbAdditionalMountSelectionPageList.hide(); //is required to see the checkboxes… I don't know why (rendering problem of gtk)
	vbAdditionalMountSelectionPageList.show_all();
	this->getWindow().set_page_type(vbRootSelectPage, Gtk::ASSISTANT_PAGE_CONTENT);
}

std::string PartitionChooser::getSelectedDevice(){
	return chkCustomPartition.get_active() ? txtCustomPartition.get_text() : lvRootPartitionSelection.get_text(lvRootPartitionSelection.get_selected()[0],0);
}

void PartitionChooser::showErrorMessage(MountExceptionType type){
	switch (type){
		case MOUNT_FAILED:       Gtk::MessageDialog(gettext("Mount failed!")).run(); break;
		case UMOUNT_FAILED:      Gtk::MessageDialog(gettext("umount failed!")).run(); break;
		case MOUNT_ERR_NO_FSTAB: Gtk::MessageDialog(gettext("This seems not to be a root file system (no fstab found)")).run(); break;
		case SUB_MOUNT_FAILED:   Gtk::MessageDialog(gettext("Couldn't mount the selected partition")).run(); break;
		case SUB_UMOUNT_FAILED:  Gtk::MessageDialog(gettext("Couldn't umount the selected partition")).run(); break;
	}
}



void PartitionChooser::signal_btt_mount_click(){
	this->eventListener->rootFsMount_request();
}


void PartitionChooser::updateSensitivity(){
	this->getWindow().set_page_complete(vbIntroPage, true);
	this->getWindow().set_page_complete(vbAdditionalMountSelectionPage, true);
	bttUmountFs.set_sensitive(isMounted);
	bttMountFs.set_sensitive(!isMounted && (chkCustomPartition.get_active() && txtCustomPartition.get_text_length() || !chkCustomPartition.get_active() && lvRootPartitionSelection.get_selected().size()));
	lvRootPartitionSelection.set_sensitive(!isMounted && !chkCustomPartition.get_active());
	txtCustomPartition.set_sensitive(!isMounted && chkCustomPartition.get_active());
	chkCustomPartition.set_sensitive(!isMounted);
	this->getWindow().set_page_complete(vbRootSelectPage, isMounted);
}


void PartitionChooser::on_cancel(){
	this->eventListener->partitionChooser_cancelled();
}


void PartitionChooser::on_apply(){
	this->eventListener->partitionChooser_applied();
}

Gtk::CheckButton& PartitionChooser::getSubmountpointCheckboxByLabel(Glib::ustring const& label){
	Glib::ListHandle< Gtk::Widget* > allChilds = vbAdditionalMountSelectionPageList.get_children();
	for (Glib::ListHandle<Gtk::Widget*>::iterator iter = allChilds.begin(); iter != allChilds.end(); iter++){
		if (((Gtk::CheckButton*)*iter)->get_label() == label)
			return (Gtk::CheckButton&)**iter;
	}
	throw ERR_CHKBUTTON_NOT_FOUND;
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
	this->init(false);
	this->updateSensitivity();
	this->show();
	Gtk::Main::run(*assistant);
}

void PartitionChooser::setIsMounted(bool isMounted){
	this->isMounted = isMounted;
}

void PartitionChooser::signal_btt_umount_click(){
	this->eventListener->rootFsUmount_request();
}
