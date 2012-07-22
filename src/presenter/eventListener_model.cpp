#include "eventListener_model.h"

EventListenerModel::EventListenerModel(GtkClient& presenter)
	: EventListener_abstract(presenter)
{
}

void EventListenerModel::entryListUpdate(){
	presenter.syncEntryList();
}

void EventListenerModel::saveProgressChanged(){
	presenter.updateSaveProgress();
}

void EventListenerModel::threadDied(){
	presenter.showErrorThreadDied();
}

void EventListenerModel::grubInstallCompleted(std::string const& msg){
	presenter.showMessageGrubInstallCompleted(msg);
}


void EventListenerModel::fb_resolutions_loaded(){
	presenter.updateSettingsDlgResolutionList();
}
