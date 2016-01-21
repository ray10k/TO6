#include "displayTask.h"

displayTask::displayTask(washingCycleTask* WCT, loadCycleTask* LCT):
  machineStateChannel (this, "machine_State_Channel"),
  washingCycleStateChannel (this, "washing_Cycle_State_Channel"),
{
	this->WCT = WCT;
	this->LCT = LCT;
}

void displayTask::main()
{
	for(;;)
	{
		RTOS::event ev = RTOS::wait(this->washingCycleStateChannel + 
			this->machineStateChannel);
		if(ev == washingCycleStateChannel)
		{
			std::string washingCycleState = washingCycleStateChannel.read();
		}
		if(ev == machineStateChannel)
		{
			currentState = machineStateChannel.read();
		}
	}
}

void displayTask::stateChanged(MachineState currentState)
{
	this->machineStateChannel.write(currentState);
}

void displayTask::setCycleState(std::string state)
{
	WCT.setCycleState(state);
}

void displayTask::loadWashingCycle(std::string userName, std::string washingCycleName)
{
	LCT.loadWashingCycle(userName, washingCycleName);
}

void displayTask::cycleStateChanged(unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName){
			
}

void displayTask::cyclePaused(const std::string& cycleName,
		const std::string& stepName){
			
}

void displayTask::cycleEnded(bool finished,
		const std::string& cycleName,
		const std::string& stepName){
			
}
