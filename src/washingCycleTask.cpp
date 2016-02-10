#include "washingCycleTask.h"

washingCycleTask::washingCycleTask(machineInteractionTask& machine):
	newCyclePool(this,"WCT_newCyclePool"),
	newCycleFlag(this,"WCT_newCycleFlag"),
	machineStatePool(this,"WCT_machineStatePool"),
	pauseFlag(this, "WCT_pauseFlag"),
	runFlag(this, "WCT_runFlag"),
	stopFlag(this, "WCT_stopFlag"),
	currentStepTimer(this,"WCT_currentStepTmr"),
	listeners(),
	ongoing(),
	currentStep(),
	runState(cycleState.STOP),
	machine(machine)
{
	washingCycle cycle = new washingCycle("cycle1");
	cycle.addStep({"step1",60,75,true,10});
	addWashingCycle("Admin", cycle);
}

void washingCycleTask::stateChanged(MachineState currentState){
	internalMachineState toWrite;
	toWrite.temperature = currentState.temperature;
	toWrite.waterLevel = currentState.waterLevel;
	this->machineStatePool.write(toWrite);
}

void washingCycleTask::addCycleStateListener(cycleStateListener& listener){
	this->listeners.push_back(listener);
}

void washingCycleTask::loadCycle(std::string userName, std::string washingCycleName)
{
	UserWashingCycle cycle = findUserWashingCycle(userName,"");
	if(cycle.userName == userName && cycle.cycle.getName() == washingCycleName)
	{
		this->newCycleFlag.set();
		this->newCyclePool.write(cycle.cycle);
	}
}

void washingCycleTask::addWashingCycle(std::string userName, washingCycle cycle)
{
	washingCycles.push_back({userName, cycle});
}

std::vector<std::string> washingCycleTask::getWashingCycleNames(std::string userName)
{
	std::vector<std::string> cycleNames;
	std::vector<UserWashingCycle>::iterator cycle = this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if(cycle.userName == userName)
		{
			cycleNames.push_back(cycle.cycle.getName());
		}
	}
	return cycleNames;
}

int washingCycleTask::getTotalCycleSteps(std::string washingCycleName)
{
	if(findUserWashingCycle("",washingCycleName).cycle.getName() == washingCycleName)
	{
		return cycle.cycle.totalSteps();
	}
	return 0;
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

void washingCycleTask::stateChanged (MachineState currentState){
	internalMachineState temp();
	temp.temperature = currentState.temperature;
	temp.waterLevel = currentState.waterLevel;
	this->machineStatePool.write(temp);
}

void washingCycleTask::notifyListeners(){
	std::vector<cycleStateListener>::iterator listen = this->listeners.begin();

	switch(this->runState){
	case cycleState.STOP:
		for(;listen != this->listeners.end(); ++listen){
			bool properEnd = this->currentStep.isFinal();
			*listen.cycleEnded(properEnd,
				this->ongoing.getName(),
				this->currentStep.getName();
		}
		break;
	case cycleState.PAUSE:
		for (;listen != this->listeners.end(); ++listen){
			*listen.cyclePaused(this->ongoing.getName(),
				this->currentStep.getName());
		}
		break;
	case cycleState.RUN:
		for (;listen != this->listeners.end(); ++listen){
			*listen.cycleStateChanged(this->ongoing.totalSteps(),
				this->ongoing.currentStepNumber(),
				this->ongoing.getName(),
				this->currentStep.getName());
		}
		break;
	}
}

void washingCycleTask::toStandBy(){
	this->machine.setTemperature(20);
	this->machine.setWaterLevel(0);
	this->machine.setRPM(false,0);
	this->machine.setDetergent(false);
}

void washingCycleTask::updateMachine(){
	if(currentStep.isTimed()){
		this->currentStepTimer.set(
			this->currentStep.getDuration() S);
	}

	if (this->currentStep.mustFlush()){
		this->machine.flush();
	}else{
		this->machine.setTemperature(
			this->currentStep.getTemperature());

		this->machine.setWaterLevel(
			this->currentStep.getWaterLevel());
	}

	this->machine.setRPM(
		this->currentStep.isDrumClockwise(),
		this->currentStep.getDrumSpeed());

	this->machine.setDetergent(
		this->currentStep.addDetergent());

	this->machine.setMachineState(true);
}

UserWashingCycle washingCycleTask::findUserWashingCycle(
	std::string userName, std::string washingCycleName)
{
	std::vector<UserWashingCycle>::iterator cycle = this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if(cycle.userName == userName || cycle.cycle.getName() == washingCycleName)
		{
			return cycle;
		}
	}
	washingCycle emptyCycle;
	return {"", emptyCycle};
}

void washingCycleTask::main(){
	while (1==1){
		//State: Stopped. Wait until instructed to run.
		while(this->runState != cycleState.RUN){
			this->runState = this->cycleStateChannel.read();
			RTOS::wait(500);
		}
		//State: Waiting. Fetch the washing cycle as soon as it becomes
		//available.
		this->ongoing = this->loadCyclePool.read();
		//State: Running. Check for the existence of a next cycle step, update
		//state, confirm the program does not need to be paused or stopped,
		//provide washing machine with current-step instructions.
		while(this->ongoing.hasNext()){
			RTOS::event progress = RTOS::wait(cycleStateChannel +
										machineStatePool +
										currentStepTimer);

			if (progress == cycleStateChannel){
				//Guaranteed to be at least 1 item waiting in the channel,
				//so this will not block.
				this->runState = this->cycleStateChannel.read();
			}

			bool brake = false;

			switch(this->runState){
				case cycleState.RUN:
				break; //nothing to see here, move along.
				case cycleState.STOP:
				//kinda wish c++ would allow for labled breaks, like Java does.
				brake = true;
				break;
				case cycleState.PAUSE:
				//take a break until you're needed again.
				toStandBy();
				this->currentStepTimer.cancel();
				notifyListeners();
				//State: paused. Wait until execution of washing program resumes
				//or a shutdown command was sent.
				while(this->runState == cycleState.PAUSE){
					this->runState = this->cycleStateChannel.read();
					if (this->runState == cycleState.STOP){
						brake = true;
					}else if (this->runState == cycleState.RUN){
						updateMachine();
						notifyListeners();
					}
					RTOS::wait(500);
				}
				break;
			}

			if (brake){
				notifyListeners();
				break;
			}

			if (progress == machineStatePool){
				this->knownState = this->machineStatePool.read();
			}

			if (progress == currentStepTimer||assessProgress()){
				//independent of weather the last step was time- or machine-
				//constrained, it's finished now. Move on to the next step.
				this->currentStep = this->ongoing.next();
				updateMachine();

				notifyListeners();
				continue;
			}
		}
	}
}
