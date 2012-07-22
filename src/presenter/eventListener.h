#ifndef EVENTLISTENER_INCLUDED
#define EVENTLISTENER_INCLUDED
#include "grubCustomizer.h"

#include "../interface/evt_grubInstallDlg.h"
#include "../interface/evt_model.h"
#include "../interface/evt_listCfgDlg.h"
#include "../interface/evt_partitionChooser.h"
#include "../interface/evt_scriptAddDlg.h"
#include "../interface/evt_settings.h"

class EventListener :
	public EventListener_settings,
	public EventListener_partitionChooser,
	public EventListener_grubInstallDlg,
	public EventListener_listCfgDlg,
	public EventListener_model,
	public EventListener_scriptAddDlg
{
	GrubCustomizer& presenter;
public:
	EventListener(GrubCustomizer& presenter);
	void settings_dialog_request();
	void reload_request();
	void save_request();
	void rootSelectorCompleted();
	void rootSelector_request();
	void installDialogRequest();
	void installGrub_request(std::string const& device);
	void scriptAddDlg_requested();
	void scriptAddDlg_applied();
	void scriptSelected();
	void removeProxy_requested(void* p);
	bool exitRequest();

	void signal_script_state_toggled(void* script);
	void signal_entry_state_toggled(void* entry);
	void signal_entry_renamed(void* entry);

	void ruleSwap_requested(void* a, void* b);
	void proxySwap_requested(void* a, void* b);

	void ruleSelected(void* rule);
	void proxySelected(void* proxy);

	void aboutDialog_requested();

	//settings dialog
	void setting_row_changed(std::string const& name);
	void default_entry_predefined_toggeled();
	void default_entry_saved_toggeled();
	void default_entry_changed();
	void showMenu_toggled();
	void osProber_toggled();
	void timeout_changed();
	void kernelparams_changed();
	void generateRecovery_toggled();
	void useCustomResolution_toggled();
	void resolution_changed();
	void colorChange_requested();
	void backgroundChange_requested();
	void backgroundCopy_requested();
	void backgroundRemove_requested();
	void settings_dialog_hide_request();

	//root selector
	void partitionChooser_applied();
	void partitionChooser_cancelled();
	void rootFsMount_request();
	void rootFsUmount_request();
	void submountpoint_mount_request(std::string const& mountpoint);
	void submountpoint_umount_request(std::string const& mountpoint);

	//model
	void loadProgressChanged();
	void saveProgressChanged();
	void grubInstallCompleted(std::string const& msg);
	void fb_resolutions_loaded();
};
#endif
