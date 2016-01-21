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
			//send currentState-> websocket-> website
		}
	}
}

void displayTask::stateChanged(MachineState currentState)
{
	this->machineStateChannel.write(currentState);
}

void displayTask::setCycleState(int state)
{
	switch(state)
	{
		case RUN:	WCT.run(); 		break;
		case PAUSE: WCT.pause(); 	break;
		case STOP:  WCT.stop();		break;
	}
}

void displayTask::loadWashingCycle(std::string userName, std::string washingCycleName)
{
	LCT.loadWashingCycle(userName, washingCycleName);
}

void displayTask::cycleStateChanged(
		unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName)
{
	//send cycle state ^info-> websocket-> website
}

void displayTask::cyclePaused(
		const std::string& cycleName,
		const std::string& stepName)
{
	//send cycle paused-> websocket-> website
}

void displayTask::cycleEnded(
		bool finished,
		const std::string& cycleName,
		const std::string& stepName)
{
	//send cycle ended-> websocket-> website
}
