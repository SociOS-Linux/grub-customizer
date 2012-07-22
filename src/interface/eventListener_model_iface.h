#ifndef EVENTLISTENER_MODEL_IFACE
#define EVENTLISTENER_MODEL_IFACE
class EventListenerModel_iface {
	public:
	virtual void entryListUpdate()=0;
	virtual void saveProgressChanged()=0;
	virtual void threadDied()=0;
	virtual void grubInstallCompleted(std::string const& msg)=0;
};
#endif
