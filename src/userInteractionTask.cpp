#include "userInteractionTask.h"

userInteractionTask::userInteractionTask(washingCycleTask* WCT, loadCycleTask* LCT):
  machineStateChannel (this, "machine_State_Channel"),
  washingCycleStateChannel (this, "washing_Cycle_State_Channel"),
  users(),
  WCT(WCT),
  LCT(LCT)
{
	addUser({"Admin", "0"});
}

void userInteractionTask::main()
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

void userInteractionTask::stateChanged(MachineState currentState)
{
	this->machineStateChannel.write(currentState);
}

void userInteractionTask::cycleStateChanged(
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

void userInteractionTask::cyclePaused(
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

void userInteractionTask::cycleEnded(
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


void userInteractionTask::setCycleState(int state)
{
	switch(state)
	{
		case RUN:	WCT.run(); 		break;
		case PAUSE: WCT.pause(); 	break;
		case STOP:  WCT.stop();		break;
	}
}

void userInteractionTask::loadWashingCycle(std::string userName, std::string washingCycleName)
{
	if(loggedIn)
	{
		if(currentUser.userName = userName)
		{
			LCT.loadWashingCycle(userName, washingCycleName);
		}
	}
}

std::vector<std::string> userInteractionTask::loadWashingCycleNames()
{
	return LCT.getWashingCycleNames(currentUser.userName);
}

int userInteractionTask::getTotalCycleSteps(std::string washingCycleName)
{
	return LCT.getTotalCycleSteps(washingCycleName);
}

void userInteractionTask::addUser(User user)
{
	users.push_back(user);
}

User userInteractionTask::findUser(std::string userName)
{
	std::vector<User>::iterator user = this->users.begin();
	for(;user != this->users.end(); ++user)
	{
		if(user.userName == userName)
		{
			return user;
		}
	}
	return {"", ""};
}

bool userInteractionTask::checkUserName(std::string userName)
{
	if(findUser(userName).userName == userName)
	{
		return true;
	}
	return false;
}

bool userInteractionTask::checkPassword(std::string userName, std::string password)
{
	if(findUser(userName).password == password)
	{
		return true;
	}
	return false;
}

void userInteractionTask::login(std::string userName)
{
	loggedIn = true;
	currentUser = findUser(userName);
}

void userInteractionTask::logout()
{
	loggedIn = false;
	currentUser = {"",""};
}

bool userInteractionTask::getLoggedIn()
{
	return loggedIn;
}

std::string userInteractionTask::getCurrentUserPassword()
{
	if(loggedIn)
	{
		return currentUser.password;
	}	
}

void userInteractionTask::changeCurrentUserPassword(std::string password)
{
	if(loggedIn)
	{
		findUser(currentUser.userName).password = password;
	}
}