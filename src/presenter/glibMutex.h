#ifndef GLIBMUTEX_H_
#define GLIBMUTEX_H_
#include "../interface/mutex.h"
#include <glibmm/thread.h>

class GlibMutex : public Mutex {
protected:
	Glib::Mutex mutex;
public:
	void lock();
	bool trylock();
	void unlock();
};

#endif /* GLIBMUTEX_H_ */
