#include "glibMutex.h"

void GlibMutex::lock() {
	this->mutex.lock();
}

bool GlibMutex::trylock() {
	return this->mutex.trylock();
}

void GlibMutex::unlock() {
	this->mutex.unlock();
}
