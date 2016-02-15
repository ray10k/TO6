#include "readBlockingQueue.h"

template <class T> 
readBlockingQueue::readBlockingQueue():
	fallback(defaultVal),
	internal(),
	synchronization()
{}

template <class T>
const T& readBlockingQueue::getNext(){
	T retval = this->fallback;
	if (! this->isEmpty()){
		this->synchronization.lock();
		retval = this->internal.front();
		this->internal.pop_front();
		this->synchronization.unlock();
	}
	return retval;
}

template <class T>
void readBlockingQueue::push(const T& toPush){
	if( this->synchronization.try_lock()){
		this->internal.push(toPush);
		this->synchronization.unlock();
		return true;
	}
	return false;
}

template <class T>
bool readBlockingQueue::isEmpty() const{
	return this->internal.size() == 0;
}
