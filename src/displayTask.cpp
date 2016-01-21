#include "displayTask.h"

displayTask::displayTask(washingCycleTask* WCT, loadCycleTask* LCT):
  machineStateChannel (this, "machine_State_Channel"),
  washingCycleStateChannel (this, "washing_Cycle_State_Channel"),
  users(),
  WCT(WCT),
  LCT(LCT)
{
	addUser({"Admin", "0"});
}

void displayTask::main()
{
	for(;;)
	{
		RTOS::event ev = RTOS::wait(this->washingCycleStateChannel + 
			this->machineStateChannel);
		if(ev == washingCycleStateChannel)
		{
			currentStep = washingCycleStateChannel.read();
			//send currentStep-> websocket-> website
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

void displayTask::cycleStateChanged(
		unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName)
{
	CycleStep sendStep = 
	{
		totalSteps, 
		currentStep, 
		RUN, 
		cycleName, 
		stepName, 
		currentCycleStep.finished
	};
	this->washingCycleStateChannel.write(sendStep);
}

void displayTask::cyclePaused(
		const std::string& cycleName,
		const std::string& stepName)
{
	CycleStep sendStep = 
	{
		currentCycleStep.totalSteps, 
		currentCycleStep.currentStep, 
		PAUSE, 
		cycleName, 
		stepName, 
		currentCycleStep.finished
	};
	this->washingCycleStateChannel.write(sendStep);
}

void displayTask::cycleEnded(
		bool finished,
		const std::string& cycleName,
		const std::string& stepName)
{
	CycleStep sendStep = 
	{
		currentCycleStep.totalSteps, 
		currentCycleStep.currentStep, 
		STOP, 
		cycleName, 
		stepName, 
		finished
	};
	this->washingCycleStateChannel.write(sendStep);
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

void displayTask::addUser(User user)
{
	users.push_back(user);
}

bool displayTask::checkUserName(std::string userName)
{
	std::vector<User>::iterator user = this->users.begin();
	for(;user != this->users.end(); ++user)
	{
		if(user.userName == userName)
		{
			return true;
		}
	}
	return false;
}

bool displayTask::checkPassword(std::string userName, std::string password)
{
	std::vector<User>::iterator user = this->users.begin();
	for(;user != this->users.end(); ++user)
	{
		if(user.userName == userName)
		{
			if(user.password == password)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}