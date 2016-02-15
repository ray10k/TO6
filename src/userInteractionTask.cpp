#include "userInteractionTask.h"

userInteractionTask::userInteractionTask(washingCycleTask* WCT):
  machineStatePool ("machineStatePool"),
  stateUpdateFlag (this,"stateUpdateFlag"),
  cycleStatePool ("cycleStatePool"),
  users(),
  WCT(*WCT)
{
	addUser({"Admin", "0"});
}

void userInteractionTask::main()
{
	for(;;)
	{
		this->wait(this->stateUpdateFlag);

		currentCycleStep = cycleStatePool.read();
		//send currentStep-> websocket-> website

		currentState = machineStatePool.read();
		//send currentState-> websocket-> website
	}
}

void userInteractionTask::stateChanged(MachineState currentState)
{
	this->machineStatePool.write(currentState);
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
		cycleState::RUN,
		cycleName,
		stepName,
		currentCycleStep.finished
	};
	this->cycleStatePool.write(sendStep);
}

void userInteractionTask::cyclePaused(
		const std::string& cycleName,
		const std::string& stepName)
{
	CycleStep sendStep =
	{
		currentCycleStep.totalSteps,
		currentCycleStep.currentStep,
		cycleState::PAUSE,
		cycleName,
		stepName,
		currentCycleStep.finished
	};
	this->cycleStatePool.write(sendStep);
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
		cycleState::STOP,
		cycleName,
		stepName,
		finished
	};
	this->cycleStatePool.write(sendStep);
}


void userInteractionTask::setCycleState(int state)
{
	switch(state)
	{
		case (int)cycleState::RUN:	WCT.run(); 		break;
		case (int)cycleState::PAUSE: WCT.pause(); 	break;
		case (int)cycleState::STOP:  WCT.stop();		break;
	}
}

void userInteractionTask::loadCycle(std::string userName, std::string washingCycleName)
{
	if(loggedIn)
	{
		if(currentUser.userName == userName)
		{
			this->loadCycle(userName, washingCycleName);
		}
	}
}

std::vector<std::string> userInteractionTask::loadWashingCycleNames()
{
	return WCT.getWashingCycleNames(currentUser.userName);
}

int userInteractionTask::getTotalCycleSteps(cycleID id)
{
	return WCT.getTotalCycleSteps(id);
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
		if((*user).userName == userName)
		{
			return *user;
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
