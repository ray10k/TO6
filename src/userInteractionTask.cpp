#include "userInteractionTask.h"
#include "rapidjson/allocators.h"

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
	{
		machineUpdateFormat.SetObject();
		
		rapidjson::Value type; type = "machine";
		rapidjson::Value temp; temp = 0;
		rapidjson::Value wate; wate = 0;
		rapidjson::Value drum; drum = 0;
		rapidjson::Value soap; soap = false;
		rapidjson::Value clck; clck = false;
		rapidjson::Value lock; lock = false;
		rapidjson::Value vlve; vlve = false;
		rapidjson::Value pump; pump = false;
		rapidjson::Value heat; heat = false;
		rapidjson::Value LED;  LED  = false;
		
		rapidjson::Document::AllocatorType& allocator = 
			machineUpdateFormat.GetAllocator();
		
		machineUpdateFormat.AddMember("type",type,allocator);
		machineUpdateFormat.AddMember("temperature",temp,allocator);
		machineUpdateFormat.AddMember("water",wate,allocator);
		machineUpdateFormat.AddMember("RPM",drum,allocator);
		machineUpdateFormat.AddMember("soap",soap,allocator);
		machineUpdateFormat.AddMember("clockwise",clck,allocator);
		machineUpdateFormat.AddMember("lock",lock,allocator);
		machineUpdateFormat.AddMember("valve",vlve,allocator);
		machineUpdateFormat.AddMember("pump",pump,allocator);
		machineUpdateFormat.AddMember("heater",heat,allocator);
		machineUpdateFormat.AddMember("signal",LED,allocator);		
	}
	
	{
		cycleUpdateFormat.SetObject();
		
		rapidjson::Value type; type = "cycle";
		rapidjson::Value name; name = "no cycle";
		rapidjson::Value curr; curr = 0;
		rapidjson::Value totl; totl = 0;
		rapidjson::Value snam; snam = "no step";
		rapidjson::Value stat; stat = "stop";
		
		rapidjson::Document::AllocatorType& allocator = 
			cycleUpdateFormat.GetAllocator();
			
		cycleUpdateFormat.AddMember("type",type,allocator);
		cycleUpdateFormat.AddMember("name",name,allocator);
		cycleUpdateFormat.AddMember("currentStep",curr,allocator);
		cycleUpdateFormat.AddMember("totalSteps",totl,allocator);
		cycleUpdateFormat.AddMember("stepName",snam,allocator);
		cycleUpdateFormat.AddMember("state",stat,allocator);
	}
}

void userInteractionTask::main()
{
#ifdef DEBUG
	std::cout << "UIT started."<< std::endl;
#endif
	for(;;)
	{	
		this->wait(this->stateUpdateFlag);


		currentCycleStep = cycleStatePool.read();
		//send currentStep-> websocket-> website

		currentState = machineStatePool.read();
		//send currentState-> websocket-> website
		
		rapidjson::StringBuffer buff;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
		
		machineUpdateFormat["temperature"] = currentState.temperature;
		machineUpdateFormat["water"] = currentState.waterLevel;
		machineUpdateFormat["RPM"] = currentState.drumRPM;
		machineUpdateFormat["soap"] = currentState.soapDispenser;
		machineUpdateFormat["clockwise"] = currentState.drumClockwise;
		machineUpdateFormat["lock"] = currentState.doorLock;
		machineUpdateFormat["valve"] = currentState.waterValve;
		machineUpdateFormat["pump"] = currentState.pump;
		machineUpdateFormat["heater"] = currentState.heatingUnit;
		machineUpdateFormat["signal"] = currentState.signalLed;
		
		machineUpdateFormat.Accept(writer);
#ifdef DEBUG
		std::cout << buff.GetString()<<std::endl;
#endif
		
		this->webcon->broadcast(buff.GetString());
		
		//since the above looks kinda time consuming, let's yeild and let other
		//tasks do some work if they need to before we continue.
		
		this->release();
		
		//rather safe than sorry...
		
		buff.Clear();
		
		cycleUpdateFormat["name"].SetString(currentCycleStep.cycleName.c_str(),
						currentCycleStep.cycleName.length());
		cycleUpdateFormat["currentStep"] = currentCycleStep.currentStep;
		cycleUpdateFormat["totalSteps"] = currentCycleStep.totalSteps;
		cycleUpdateFormat["stepName"].SetString(
			currentCycleStep.stepName.c_str(),
			currentCycleStep.stepName.length());
		switch(currentCycleStep.state){
			case cycleState::RUN:
				cycleUpdateFormat["state"] = "run";
				break;
			case cycleState::PAUSE:
				cycleUpdateFormat["state"] = "pause";
				break;
			default:
			case cycleState::STOP:
				cycleUpdateFormat["state"] = "stop";
				break;
		}
		trace;
		cycleUpdateFormat.Accept(writer);
		trace;
#ifdef DEBUG
		std::cout << buff.GetString()<<std::endl;
#endif
		
		this->webcon->broadcast(buff.GetString());
	}
}

void userInteractionTask::stateChanged(const MachineState& currentState)
{
	MachineState copy(currentState);
	this->machineStatePool.write(copy);
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
