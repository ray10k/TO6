//**************************************
//! Task responsible for executing a washing cycle.
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __WASHING_CYCLE_TASK
#define __WASHING_CYCLE_TASK

#include "prtos/pRTOS.h"
#include "washingCycle.h"
#include "cycleStateListener.h"
#include <vector>

class machineState;

class washingCycleTask : public RTOS::Task
{
public:
	washingCycleTask();
	//! Registers a new listener for events describing the progress through the
	//! currently ongoing cycle.
	void addCycleStateListener(cycleStateListener* listener);
	//! Accept a washing cycle to execute, as soon as no cycle is currently
	//! executing.
	void runCycle(washingCycle& toRun);

protected:
	//because the Task interface demands it, and because this task needs to do 
	//things.
	main(void);
	
	

private:
	//! notify all listeners of the current state. Should interpret current
	//! state and adjust what functions are called.
	void notifyListeners();
	//! compare the latest known state of the machine to what's expected for the
	//! current step. Returns false if the current step is time-based or the 
	//! state of the machine is not close enough to the expected state, true
	//! otherwise.
	bool assessProgress();

	//more than one washing cycle waiting is a serious error; more than one
	//should never occur.
	RTOS::channel<washingCycle,1> loadCycleChannel;
	//if the user spams too hard, their problem.
	RTOS::channel<cycleState,4> cycleStateChannel;
	//no easy way of knowing how much of these we'll get; assign big and hope
	//for the best.
	//TODO: implement machine state listener stuff.
	RTOS::channel<machineState,16> machineStateChannel;
	RTOS::timer currentStepTimer;
	
	std::vector<cycleStateListener> listeners;
	washingCycle ongoing;
	cycleState state;
};

//**************************************
//! Container for data regarding the state of the physical machine, so it can be
//! grouped and stored in a channel. Written because at the time of creation,
//! the interface of the Machine State Listener was not known yet.
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

class machineState
{
public:
	machineState();
	short unsigned int temperature;
	short unsigned int waterLevel;
	short unsigned int drumSpeed;
	bool doorLock;
	bool drumClockwise;
	bool heater;
	bool pump;
	bool tap;
	bool detergent;
};

#endif
