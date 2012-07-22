#ifndef GRUBLISTCFGDLG_H_
#define GRUBLISTCFGDLG_H_

#include "evt_listCfgDlg.h"
#include "glibmm/ustring.h"
class GrublistCfgDlg {
public:
	enum Exception {
		PROXY_ITER_NOT_FOUND,
		RULE_ITER_NOT_FOUND
	};
	virtual void setEventListener(EventListener_listCfgDlg& eventListener)=0;
	virtual void run()=0;
	virtual void close()=0;
	virtual bool requestForRootSelection()=0;
	virtual bool requestForBurgMode()=0;

	virtual void setIsBurgMode(bool isBurgMode)=0;

	virtual void update_move_buttons()=0;
	virtual void update_remove_button()=0;
	virtual void saveConfig()=0;
	virtual void setLockState(int state)=0;
	virtual void updateButtonsState()=0;

	virtual void setProgress(double progress)=0;
	virtual void progress_pulse()=0;
	virtual void hideProgressBar()=0;
	virtual void setStatusText(Glib::ustring const& new_status_text)=0;
	virtual void appendScript(Glib::ustring const& name, bool is_active, void* proxyPtr)=0;
	virtual void appendEntry(Glib::ustring const& name, bool is_active, void* entryPtr, bool editable)=0;
	virtual void showProxyNotFoundMessage()=0;

	virtual void setProxyName(void* proxy, Glib::ustring const& name, bool isModified)=0;

	virtual void swapProxies(void* a, void* b)=0;
	virtual void swapRules(void* a, void* b)=0;

	virtual void setDefaultTitleStatusText(Glib::ustring const& str)=0;

	virtual void removeProxy(void* p)=0;

	virtual int showExitConfirmDialog(int type)=0;
	virtual void showErrorMessage(Glib::ustring const& msg)=0;
	virtual void clear()=0;

	virtual Glib::ustring getRuleName(void* rule)=0;
	virtual void setRuleName(void* rule, Glib::ustring const& newName)=0;
	virtual bool getRuleState(void* rule)=0;
	virtual void setRuleState(void* rule, bool newState)=0;
	virtual bool getProxyState(void* proxy)=0;
	virtual void setProxyState(void* proxy, bool isActive)=0;
};

#endif
