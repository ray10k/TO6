//******************************************************************************
//! Should-be thread-safe implementation of a queue, simplified.
//! \authors
//! 	- Wouter van den Heuvel
//! \context
//! 	Part of TO6 2015-2016.
//******************************************************************************

#ifndef __THREADSAFEQUEUE_H
#define __THREADSAFEQUEUE_H

#include <mutex>
#include <deque>

template <class T>
class threadSafeQueue{
	//! Constructs a new queue, with the given object as the default value to be
	//! returned when a read fails due to locking.
	threadSafeQueue(T& defaultVal);
	//! Pops the frontmost value from this queue, or returns the previously
	//! given default value if the queue is empty or locked by another thread.
	//! Does not lock the thread.
	T getNext();
	//! Locks the thread until the queue is available, and pushes the given item
	//! to the back of the queue.
	void push(T& toPush);
	//! Returns true if there are no items in the queue. Note that this is not
	//! thread-safe.
	bool isEmpty() const;
private:
	T fallback;
	std::deque<T> internal;
	std::mutex synchronization;
};

#endif
