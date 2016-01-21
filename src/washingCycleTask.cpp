#include "washingCycleTask.h"

washingCycleTask::washingCycleTask():
	loadCycleChannel(this,"WCT_loadCycleChnl"),
	cycleStateChannel(this,"WCT_cycleStateChnl"),
	machineStateChannel(this,"WCT_machineStateChnl"),
	currentStepTimer(this,"WCT_currentStepTmr"),
	listeners(),
	ongoing(),
	runState(cycleState.STOP)
{

}

void washingCycleTask::stateChanged(MachineState currentState){
	internalMachineState toWrite;
	toWrite.temperature = currentState.temperature;
	toWrite.waterLevel = currentState.waterLevel;
	this->machineStateChannel.write(toWrite);
}

void washingCycleTask::addCycleStateListener(cycleStateListener& listener){
	listeners.push_back(listener);
}

void washingCycleTask::loadCycle(washingCycle& cycle)
{
	this->loadCycleChannel.read(cycle);
}

bool washingCycleTask::assessProgress(cycleStep& currentStep){
	//timed steps are handled by the timer, and don't need further confirmation.
	if (currentStep.isTimed()){
		return false;
	}
	//non-timed tasks finish when the water level and temperature are equal to,
	//or greater than the expected level. Assume the MIT is responsible for
	//keeping the balance.
	if(currentStep.getTemperature() < currentState.temperature ||//this->machineState.temperature ||
		currentStep.getWaterLevel() < currentState.waterLevel){//this->machineState.waterLevel){
		return false;
	}
	return true;
}

void washingCycleTask::stateChanged (MachineState currentState){
	internalMachineState temp();
	temp.temperature = currentState.temperature;
	temp.waterLevel = currentState.waterLevel;
	this->machineStateChannel.write(temp);
}

void washingCycleTask::notifyListeners(){
	std::vector<cycleStateListener>::iterator listen = this->listeners.begin();
	
	switch(this->runState){
	case cycleState.STOP:
		for(;listen != this->listeners.end(); ++listen){
			bool properEnd = !this->current.hasNext(); //assume that if there is
			//no next item in the cycle, the cycle has ended properly.
			*listen.cycleEnded(properEnd,
				this->current.getName(),
				this->current.current().getName());
		}
		break;
	case cycleState.PAUSE:
		for (;listen != this->listeners.end(); ++listen){
			*listen.cyclePaused(this->current.getName(),
				this->current.current().getName());
		}
		break;
	case cycleState.RUN:
		for (;listen != this->listeners.end(); ++listen){
			*listen.cycleStateChanged(this->current.totalSteps(),
				this->current.currentStepNumber(),
				this->current.getName(),
				this->current.current().getName());
		}
		break;
	}
}

void washingCycleTask::setCycleState(std::string state)
{
	switch(state)
	{
		case "RUN": 	this->cycleStateChannel.write(cycleState.RUN); 	 break;
		case "PAUSE": 	this->cycleStateChannel.write(cycleState.PAUSE); break;
		case "STOP": 	this->cycleStateChannel.write(cycleState.STOP);  break;
	}
}

void washingCycleTask::main(){
	while (1==1){
		//State: Stopped. Wait until instructed to run.
		while(this->runState != cycleState.RUN){ 
			//channel.read blocks until something can be read.
			this->runState = this->cycleStateChannel.read();
		}
		//State: Waiting. Fetch the washing cycle as soon as it becomes
		//available.
		this->ongoing = this->loadCycleChannel.read();
		//State: Running. Check for the existence of a next cycle step, update 
		//state, confirm the program does not need to be paused or stopped, 
		//provide washing machine with current-step instructions.
		while(this->ongoing.hasNext()){
			RTOS::event progress = wait(cycleStateChannel + 
										machineStateChannel +
										currentStepTimer);
			
			if (currentStep.isTimed()){
				this->currentStepTimer.set(currentStep.getDuration() S);
			}
			if (progress == cycleStateChannel){
				//Guaranteed to be at least 1 item waiting in the channel,
				//so this will not block.
				this->state = this->cycleStateChannel.read();
			}
			else if(progress == machineStateChannel)
			{
				currentState = machineStateChannel.read();
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
				//TODO: tell the washing machine to go to a stand-by state.
				this->currentStepTimer.cancel();
				notifyListeners();
				//State: paused. Wait until execution of washing program resumes
				//or a shutdown command was sent.
				while(this->runState == cycleState.PAUSE){
					this->runState = this->cycleStateChannel.read();
					if (this->runState == cycleState.STOP){
						brake = true;
					}else if (this->runState == cycleState.RUN){
						notifyListeners();
					}
				}
				break;
			}
			
			if (brake){
				notifyListeners();
				break;
			}
			
			//TODO: handle the current state of the machine, based on the most
			//recent events.
			if (progress == machineStateChannel){
				internalMachineState latest = this->machineStateChannel.read();
				this->machineState.temperature = latest.temperature;
				this->machineState.waterLevel = latest.waterLevel;
			}
			
			//TODO: Once the relevant interface is known, instruct the Washing
			//machine in regards to the current step.
			
			if (progress == currentStepTimer||assessProgress()){
				//independent of weather the last step was time- or machine-
				//constrained, it's finished now. Move on to the next step.
				cycleStep currentStep = this->ongoing.next();
				notifyListeners();
				continue;
			}
		}
	}
}
