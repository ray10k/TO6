#include "userInteractionTask.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

userInteractionTask::userInteractionTask(washingCycleTask* WCT):
  RTOS::task(10,"user interaction"),
  machineStatePool ("machineStatePool"),
  stateUpdateFlag (this,"stateUpdateFlag"),
  cycleStatePool ("cycleStatePool"),
  users(),
  WCT(WCT),
  webcon(nullptr)
{
	addUser({"Admin", "0"});
	currentCycleStep = {-1,-1,cycleState::STOP," ", " ", false};
}

void userInteractionTask::main()
{
	for(;;)
	{
#ifdef DEBUG
		std::cout << "UIT running..." << std::endl;
#endif
		
		this->wait(this->stateUpdateFlag);
		
#ifdef DEBUG
		std::cout << "UIT wait passed. " << std::endl;
#endif

		currentCycleStep = cycleStatePool.read();
		//send currentStep-> websocket-> website

		currentState = machineStatePool.read();
		//send currentState-> websocket-> website
		
		rapidjson::StringBuffer buff;
		rapidjson::Writer<rapidjson::StringBuffer> mStat(buff);
		
		mStat.StartObject();
		mStat.Key("type");
		mStat.String("machine");
		mStat.Key("temperature");
		mStat.Uint(currentState.temperature);
		mStat.Key("water");
		mStat.Uint(currentState.waterLevel);
		mStat.Key("RPM");
		mStat.Uint(currentState.drumRPM);
		mStat.Key("soap");
		mStat.Bool(currentState.soapDispenser);
		mStat.Key("clockwise");
		mStat.Bool(currentState.drumClockwise);
		mStat.Key("lock");
		mStat.Bool(currentState.doorLock);
		mStat.Key("valve");
		mStat.Bool(currentState.waterValve);
		mStat.Key("pump");
		mStat.Bool(currentState.pump);
		mStat.Key("heater");
		mStat.Bool(currentState.heatingUnit);	
		mStat.Key("signal");
		mStat.Bool(currentState.signalLed);
		mStat.EndObject();	
		
		this->webcon->broadcast(buff.GetString());
#ifdef DEBUG
		std::cout << "mstat:" << buff.GetString() << std::endl;
#endif
		
		//since the above looks kinda time consuming, let's yeild and let other
		//tasks do some work if they need to before we continue.
		
		this->release();
		
		//rather safe than sorry...
		
		rapidjson::StringBuffer buffest;
		rapidjson::Writer<rapidjson::StringBuffer> cStat(buffest);
		
		cStat.StartObject();
		cStat.Key("type");
		cStat.String("cycle");
		cStat.Key("name");
		cStat.String(currentCycleStep.cycleName.c_str());
		cStat.Key("currentStep");
		cStat.Uint(currentCycleStep.currentStep);
		cStat.Key("totalSteps");
		cStat.Uint(currentCycleStep.totalSteps);
		cStat.Key("stepName");
		cStat.String(currentCycleStep.stepName.c_str());
		cStat.Key("state");
		switch(currentCycleStep.state){
			case cycleState::RUN:
				cStat.String("run");
				break;
			case cycleState::PAUSE:
				cStat.String("pause");
				break;
			case cycleState::STOP:
				cStat.String("stop");
				break;
		}
		cStat.EndObject();
		
		this->webcon->broadcast(buffest.GetString());
#ifdef DEBUG
		std::cout << "cstat:" << buffest.GetString() << std::endl;
#endif
	}
}

void userInteractionTask::stateChanged(MachineState currentState)
{
	this->machineStatePool.write(currentState);
	this->stateUpdateFlag.set();
}

void userInteractionTask::cycleStateChanged(
		unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName)
{
	
	CycleStep sendStep =
	{
		(int)totalSteps,
		(int)currentStep,
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
		(int)currentCycleStep.totalSteps,
		(int)currentCycleStep.currentStep,
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
		(int)currentCycleStep.totalSteps,
		(int)currentCycleStep.currentStep,
		cycleState::STOP,
		cycleName,
		stepName,
		finished
	};
	this->cycleStatePool.write(sendStep);
}


void userInteractionTask::setCycleState(cycleState state)
{
	switch(state)
	{
		case cycleState::RUN:	WCT->run();			break;
		case cycleState::PAUSE: WCT->pause();		break;
		case cycleState::STOP:  WCT->stop();		break;
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
	return WCT->getWashingCycleNames(currentUser.userName);
}

int userInteractionTask::getTotalCycleSteps(cycleID id)
{
	return WCT->getTotalCycleSteps(id);
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
	return "";
}

void userInteractionTask::changeCurrentUserPassword(std::string password)
{
	if(loggedIn)
	{
		findUser(currentUser.userName).password = password;
	}
}

void userInteractionTask::setWebsocket(WebsocketController* out){
	this->webcon = out;
}

void userInteractionTask::packet_received(Packet& p){
	std::cout <<  p.get_message() << std::endl;
}
