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
	
	
	FILE* mtabfile = fopen("/etc/mtab", "r");
	if (mtabfile){
		MountTable mtab(mtabfile);
		fclose(mtabfile);
		if (mtab.getEntryByMountpoint("/media/grub-customizer_recovery_root_mountpoint")){
			mountpoint = "/media/grub-customizer_recovery_root_mountpoint";
			FILE* fstab = fopen((mountpoint+"/etc/fstab").c_str(), "r");
			if (fstab){
				generateSubmountpointSelection(fstab);
				fclose(fstab);
			}
		}
	}
	
	this->updateSensitivity();
	this->set_title(Glib::ustring("Grub Customizer: ")+gettext("Partition Chooser"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(640, 480);
	this->show_all();
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

void PartitionChooser::generateSubmountpointSelection(FILE* fstabFile){
	//delete all existing submountpoint checkbuttons
	Glib::ListHandle< Widget* > allChilds = vbAdditionalMountSelectionPageList.get_children();
	for (Glib::ListHandle<Widget*>::iterator iter = allChilds.begin(); iter != allChilds.end(); iter++){
		vbAdditionalMountSelectionPageList.remove(**iter);
		delete &**iter;
	}
	
	MountTable fstab(fstabFile);
	MountTable mtab;
	FILE* mtabfile = fopen((mountpoint+"/etc/mtab").c_str(), "r");
	if (mtabfile){
		mtab = MountTable(mtabfile);
		fclose(mtabfile);
	}
	//create new submountpoint checkbuttons
	int mpointCount = 0;
	for (MountTable::iterator iter = fstab.begin(); iter != fstab.end(); iter++){
		if (iter->mountpoint[0] == '/' && iter->mountpoint.length() > 1 && iter->mountpoint != "/proc"){
			Gtk::CheckButton* cb = new Gtk::CheckButton(iter->mountpoint);
			cb->set_active(mtab.getEntryByMountpoint(iter->mountpoint));
			cb->signal_toggled().connect(sigc::bind<Gtk::CheckButton&>(sigc::mem_fun(this, &PartitionChooser::submountpoint_toggle), *cb));
		
			vbAdditionalMountSelectionPageList.pack_start(*cb, Gtk::PACK_SHRINK);
			mpointCount++;
		}
	}

	vbAdditionalMountSelectionPageList.hide(); //is required to see the checkboxes… I don't know why (rendering problem of gtk)
	vbAdditionalMountSelectionPageList.show_all();
	if (mpointCount == 0){
		set_page_type(vbRootSelectPage, Gtk::ASSISTANT_PAGE_CONFIRM);
	}
}

void PartitionChooser::signal_btt_mount_click(){
	std::string selectedDevice = chkCustomPartition.get_active() ? txtCustomPartition.get_text() : lvRootPartitionSelection.get_text(lvRootPartitionSelection.get_selected()[0],0);
	mountpoint = "/media/grub-customizer_recovery_root_mountpoint";
	mkdir(mountpoint.c_str(), 0755);
	int result = system(("mount "+selectedDevice+" "+mountpoint).c_str());
	if (result != 0){
		Gtk::MessageDialog(gettext("Mount failed!")).run();
		mountpoint = "";
	}
	else {
		FILE* fstabFile = fopen((mountpoint+"/etc/fstab").c_str(), "r");
		int umountErrs = 0; //no used - only to use the return values of system in any way
		if (fstabFile != NULL){
			this->generateSubmountpointSelection(fstabFile);
			fclose(fstabFile);
			
			//binding system mountpoints - should work. If not, this may be no problem…
			system(("mount -o bind /proc '"+mountpoint+"/proc'").c_str()) || umountErrs++;
			system(("mount -o bind /dev '"+mountpoint+"/dev'").c_str()) || umountErrs++;
			system(("mount -o bind /sys '"+mountpoint+"/sys'").c_str()) || umountErrs++;
		}
		else {
			Gtk::MessageDialog(gettext("This seems not to be a root file system (no fstab found)")).run();
			system(("umount "+mountpoint).c_str()) || umountErrs++;
			mountpoint = "";
		}
	}
	updateSensitivity();
}
void PartitionChooser::signal_btt_umount_click(){
	if (!umount_all(mountpoint)){
		Gtk::MessageDialog(gettext("umount failed!")).run();
	}
	else {
		mountpoint = "";
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
		int mountSuccess = system(("chroot '"+mountpoint+"' "+(sender.get_active() ? "mount" : "umount")+" '"+sender.get_label()+"'").c_str());
		if (mountSuccess != 0){
			if (sender.get_active())
				Gtk::MessageDialog(gettext("Couldn't mount the selected partition")).run();
			else
				Gtk::MessageDialog(gettext("Couldn't umount the selected partition")).run();
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

