#ifndef MUTEX_H_INCLUDED
#define MUTEX_H_INCLUDED

class Mutex {
public:
	virtual void lock() = 0;
	virtual bool trylock() = 0;
	virtual void unlock() = 0;
};

#endif
