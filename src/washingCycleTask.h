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

#include "./prtos/pRTOS.h"
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
	washingCycleTask(machineInteractionTask * machine);
	//! As defined by the interface machineStateListener.
	void stateChanged(const MachineState& currentState) override;
	//! Registers a new listener for events describing the progress through the
	//! currently ongoing cycle.
	void addCycleStateListener(cycleStateListener* listener);
	//! Provide a washingCycle to be performed when there is no ongoing cycle,
	//! and the system is in a running state.
	void loadCycle(const cycleID& toLoad);

	void addWashingCycle(washingCycle& cycle);
	std::vector<std::string> getWashingCycleNames(std::string userName);
	int getTotalCycleSteps(const cycleID& toFind)const;

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
	//! why does this even exist?
	washingCycle findUserWashingCycle(const cycleID& toFind)const;

	washingCycle ongoing;
	cycleStep currentStep;
	cycleState state;

	machineInteractionTask * machine;

	RTOS::flag CycleFlag;
	RTOS::flag runFlag;
	RTOS::flag updateFlag;
	RTOS::flag pauseFlag;
	RTOS::flag stopFlag;
	RTOS::pool<internalMachineState> machineStatePool;
	RTOS::pool<washingCycle> newCyclePool;
	RTOS::timer currentStepTimer;

	std::vector<cycleStateListener*> listeners;
	std::vector<washingCycle> washingCycles;

};

#endif
