#include "washingCycleTask.h"

washingCycleTask::washingCycleTask(machineInteractionTask * machine):
	RTOS::task(15,"washing cycle"),
	ongoing(),
	currentStep(),
	state(cycleState::STOP),
	machine(machine),
	CycleFlag((RTOS::task*)this,"WCT_newCycleFlag"),
	runFlag((RTOS::task*)this, "WCT_runFlag"),
	updateFlag((RTOS::task*)this,"WCT_updateFlag"),
	pauseFlag((RTOS::task*)this, "WCT_pauseFlag"),
	stopFlag((RTOS::task*)this, "WCT_stopFlag"),
	machineStatePool("WCT_machineStatePool"),
	newCyclePool("WCT_newCyclePool"),
	currentStepTimer((RTOS::task*)this,"WCT_currentStepTmr"),
	listeners(),
	washingCycles()
{
    cycleID defaultID("cycle1", "Admin");
	washingCycle cycle(defaultID);
	cycleStep step1 = {"step1",20,60,false,0}; cycle.addStep(step1);
	cycleStep step2 = {"step2",95,60,false,0}; cycle.addStep(step2);
	cycleStep step3 = {"step3",95,60,true,0}; cycle.addStep(step3);
	cycleStep step4 = {"step4",95,60,false,1000, 500, false}; cycle.addStep(step4);
	cycleStep step5 = {"step5",20,0,false,0, 100, true}; cycle.addStep(step5);
	addWashingCycle(cycle);
	loadCycle(defaultID);
	this->run();
}

void washingCycleTask::stateChanged(MachineState currentState){
	internalMachineState toWrite;
	toWrite.temperature = currentState.temperature;
	toWrite.waterLevel = currentState.waterLevel;
	this->machineStatePool.write(toWrite);
	this->updateFlag.set();
	
}

void washingCycleTask::addCycleStateListener(cycleStateListener* listener){
	this->listeners.push_back(listener);
}

void washingCycleTask::loadCycle(const cycleID& toLoad)
{
#ifdef DEBUG
	std::cout << "loadCycle called..." << std::endl;
#endif
	washingCycle cycle = findUserWashingCycle(toLoad);
	if(cycle == toLoad)
	{
#ifdef DEBUG
		std::cout << "loading cycle " << toLoad.name << std::endl;
#endif
		this->CycleFlag.set();
		this->newCyclePool.write(cycle);
	}
}

void washingCycleTask::addWashingCycle(washingCycle& cycle)
{
	washingCycles.push_back(cycle);
}

std::vector<std::string> washingCycleTask::getWashingCycleNames(std::string userName)
{
	std::vector<std::string> cycleNames;
	std::vector<washingCycle>::iterator cycle = this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if(cycle->getUser() == userName)
		{
			cycleNames.push_back(cycle->getName());
		}
	}
	return cycleNames;
}

int washingCycleTask::getTotalCycleSteps(const cycleID& toFind)const
{
	washingCycle temp = this->findUserWashingCycle(toFind);
	if(temp == toFind)
	{
		return temp.totalSteps();
	}
	return -1;
}

washingCycle washingCycleTask::findUserWashingCycle(const cycleID& toFind)const
{
	std::vector<washingCycle>::const_iterator cycle =
		this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if((*cycle) == toFind)
		{
			return *cycle;
		}
	}
	washingCycle defaultVal;
	return defaultVal;
}

void washingCycleTask::pause(){
	this->pauseFlag.set();
}

void washingCycleTask::run(){
	this->runFlag.set();
}

void washingCycleTask::stop(){
	this->stopFlag.set();
}

bool washingCycleTask::assessProgress(){
	//timed steps are handled by the timer, and don't need further confirmation.
	if (currentStep.isTimed()){
		return false;
	}
	//non-timed tasks finish when the water level and temperature are equal to,
	//or greater than the expected level. Assume the MIT is responsible for
	//keeping the balance.
	if(this->knownState.temperature < this->currentStep.getTemperature() ||
		this->knownState.waterLevel < this->currentStep.getWaterLevel()){
		return false;
	}
	return true;
}

void washingCycleTask::notifyListeners(){
	std::vector<cycleStateListener*>::iterator listen = this->listeners.begin();

	switch(this->state){
		case cycleState::STOP:{
			bool properEnd = this->currentStep.isFinal();
			for(;listen != this->listeners.end(); ++listen){
				(*listen)->cycleEnded(properEnd,
					this->ongoing.getName(),
					this->currentStep.getName());
			}
			break;
		}
		case cycleState::PAUSE:{
			for (;listen != this->listeners.end(); ++listen){
				(*listen)->cyclePaused(this->ongoing.getName(),
					this->currentStep.getName());
			}
			break;
		}
		case cycleState::RUN:{
			for (;listen != this->listeners.end(); ++listen){
				(*listen)->cycleStateChanged(this->ongoing.totalSteps(),
					this->ongoing.currentStepNumber(),
					this->ongoing.getName(),
					this->currentStep.getName());
			}
			break;
		}
	}
}

void washingCycleTask::toStandBy(){
	this->machine->setTemperature(20);
	this->machine->setWaterLevel(0);
	this->machine->setRPM(false,0);
	this->machine->setDetergent(false);
}

void washingCycleTask::updateMachine(){
	if(currentStep.isTimed()){
		this->currentStepTimer.set(
			this->currentStep.getDuration() S);
	}

	if (this->currentStep.mustFlush()){
		this->machine->flush();
	}else{
		this->machine->setTemperature(
			this->currentStep.getTemperature());

		this->machine->setWaterLevel(
			this->currentStep.getWaterLevel());
	}

	this->machine->setRPM(
		this->currentStep.isDrumClockwise(),
		this->currentStep.getDrumSpeed());

	this->machine->setDetergent(
		this->currentStep.getAddDetergent());

	this->machine->setMachineState(true);
}

void washingCycleTask::main(){
	while (true)
	{
#ifdef DEBUG
		std::cout << "WCT running... " << std::endl;
#endif
		//State: Stopped. Wait until instructed to run.
		this->wait(runFlag);
		state = cycleState::RUN;
		
#ifdef DEBUG
		std::cout << "WCT Passed first wait. " << std::endl;
#endif

		this->wait(CycleFlag);
		
#ifdef DEBUG
		std::cout << "WCT entering regular running state. " << std::endl;
#endif
		this->machine->setMachineState(true);
		ongoing = newCyclePool.read();
		currentStep = ongoing.getCurrent();

		while(this->ongoing.hasNext())
		{
			updateMachine();
			RTOS::event progress = this->wait(runFlag + pauseFlag + stopFlag +
											  updateFlag + currentStepTimer);
			bool brake = false;

			if(progress == runFlag){
				state = cycleState::RUN;
			}else if(progress == stopFlag) {
				state = cycleState::STOP; brake = true;
			}else if(progress == pauseFlag){
				state = cycleState::PAUSE;
				toStandBy();
				currentStepTimer.cancel();
				notifyListeners();
				RTOS::event ev = this->wait(runFlag + stopFlag);
				if(ev == runFlag) {
					updateMachine(); notifyListeners();
				}else if(ev == stopFlag) {
					brake = true;
				}
			}else if(progress == currentStepTimer || assessProgress()){
				currentStep = ongoing.next(); updateMachine(); notifyListeners();
			}
			else if(progress == updateFlag){knownState = machineStatePool.read();}

			if (brake)
			{
				notifyListeners();
				break;
			}
		}
	}
}
