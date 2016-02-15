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
	//! Constructs a new queue, with the given default.
	readBlockingQueue(T& defaultVal);
	//! Pops the frontmost value from this queue, or returns the default 
	//! supplied during construction if the queue is empty. Will block if
	//! another thread is accessing the queue.
	T getNext();
	//! Attempts to push the given item to the back of the queue. Returns true
	//! when the value was successfully pushed, false if the queue was in use by
	//! another thread, in which case nothing was added to the queue.
	bool push(T& toPush);
	//! Returns true if there are no items in the queue. Note that this is not
	//! thread-safe.
	bool isEmpty() const;
private:
	T fallback;
	std::deque<T> internal;
	std::mutex synchronization;
};

#endif
