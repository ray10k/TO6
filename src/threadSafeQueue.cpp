#include "threadSafeQueue.h"

template <class T> 
threadSafeQueue::threadSafeQueue(T& defaultVal):
	fallback(defaultVal),
	internal(),
	synchronization()
{}

template <class T>
const T& threadSafeQueue::getNext(){
	T retval = this->fallback;
	if (this->synchronization.try_lock()&&this->internal.size() > 0){
		retval = this->internal.front();
		this->internal.pop_front();
		this->synchronization.unlock();
	}
	return retval;
}

template <class T>
void threadSafeQueue::push(const T& toPush){
	this->synchronization.lock();
	this->internal.push(toPush);
	this->synchronization.unlock();
	return;
}

template <class T>
bool threadSafeQueue::isEmpty() const{
	return this->internal.size() == 0;
}