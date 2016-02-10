//**************************************
//! Task responsible for executing a washing cycle.
//! \authors
//! 	- Wouter van den Heuvel
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __WASHING_CYCLE_TASK
#define __WASHING_CYCLE_TASK

#include "prtos/pRTOS.h"
#include "washingCycle.h"
#include "cycleState.h"
#include "cycleStateListener.h"
#include "machineStateListener.h"
#include "machineInteractionTask.h"
#include <vector>

struct UserWashingCycle
{
	std::string userName;
	washingCycle cycle;
};

class washingCycleTask : public RTOS::task, public machineStateListener
{
public:
	washingCycleTask(machineInteractionTask& machine);
	//! As defined by the interface machineStateListener.
	void stateChanged(MachineState currentState) override;
	//! Registers a new listener for events describing the progress through the
	//! currently ongoing cycle.
	void addCycleStateListener(cycleStateListener& listener);
	//! Provide a washingCycle to be performed when there is no ongoing cycle,
	//! and the system is in a running state.
	void loadCycle(std::string userName, std::string washingCycleName);
	void addWashingCycle(std::string userName, washingCycle cycle);
	std::vector<std::string> getWashingCycleNames(std::string userName);
	int getTotalCycleSteps(std::string washingCycleName);

	//! Pauses execution of the current cycle.
	void pause();
	//! Starts or resumes the current ongoing cycle.
	void run();
	//! abruptly halts the current cycle, if any.
	void stop();


protected:
	//because the Task interface demands it, and because this task needs to do
	//things.
	void main(void);



private:
	//! contains the relevant data from the last handled machine state event,
	//! and the state for the current step.
	struct internalMachineState {
		internalMachineState(): temperature(20), waterLevel(0) {}
		unsigned short int temperature;
		unsigned short int waterLevel;
	} knownState;

	//! notify all listeners of the current state. Should interpret current
	//! state and adjust what functions are called.
	void notifyListeners();
	//! compare the latest known state of the machine to what's expected for the
	//! current step. Returns false if the current step is time-based or the
	//! state of the machine is not close enough to the expected state, true
	//! otherwise.
	bool assessProgress();
	//! instructs the physical machine to go into a stand-by state.
	void toStandBy();
	//! instructs the physical machine to match what is expected from the
	//! current step in the cycle.
	void updateMachine();
	UserWashingCycle findUserWashingCycle(std::string userName, std::string washingCycleName);

	//more than one washing cycle waiting is a serious error; more than one
	//should never occur.
	RTOS::flag pauseFlag;
	RTOS::flag stopFlag;
	RTOS::flag runFlag;
	//if the user spams too hard, their problem.
	RTOS::pool<washingCycle&> newCyclePool;
	RTOS::flag newCycleflag;
	//no easy way of knowing how much of these we'll get; assign big and hope
	//for the best.
	//TODO: implement machine state listener stuff.
	RTOS::pool<internalMachineState> machineStatePool;
	RTOS::flag updateFlag;
	RTOS::timer currentStepTimer;

	std::vector<cycleStateListener&> listeners;
	std::vector<UserWashingCycle&> washingCycles;

	washingCycle ongoing;
	cycleStep currentStep;

	cycleState runState;

	machineInteractionTask& machine;
};

#endif
