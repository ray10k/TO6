//******************************************************************************
//! Simple thread-safe queue that can potentially block on read, but will never
//! block on write.
//! \authors
//! 	- Wouter van den Heuvel
//! \context
//! 	Part of TO6 2015-2016.
//******************************************************************************

#ifndef __READBLOCKINGQUEUE_H
#define __READBLOCKINGQUEUE_H

#include <mutex>
#include <deque>

template <class T> 
class readBlockingQueue{
public:
	//! Constructs a new queue, with the given default.
	readBlockingQueue(const T& defaultVal):
	fallback(defaultVal),
	internal(),
	synchronization()
	{}
	//! Pops the frontmost value from this queue, or returns the default 
	//! supplied during construction if the queue is empty. Will block if
	//! another thread is accessing the queue.
	const T& getNext(){
		T * retval = this->fallback;
		if (! this->isEmpty()){
			this->synchronization.lock();
			retval = &(this->internal.front());
			this->internal.pop_front();
			this->synchronization.unlock();
		}
		return *retval;
	}

	//! Attempts to push the given item to the back of the queue. Returns true
	//! when the value was successfully pushed, false if the queue was in use by
	//! another thread, in which case nothing was added to the queue.
	bool push(const T& toPush){
		if( this->synchronization.try_lock()){
			this->internal.push(toPush);
			this->synchronization.unlock();
			return true;
		}
		return false;
	}
	//! Returns true if there are no items in the queue. Note that this is not
	//! thread-safe.
	bool isEmpty() const{
		return this->internal.size() == 0;
	}
private:
	T fallback;
	std::deque<T> internal;
	std::mutex synchronization;
};

#endif
