#include "eventListener_model.h"

EventListenerModel::EventListenerModel(GtkClient& presenter)
	: presenter(presenter)
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
