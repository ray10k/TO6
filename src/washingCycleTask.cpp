#include "washingCycleTask.h"

washingCycleTask::washingCycleTask():
	loadCycleChannel(this,"WCT_loadCycleChnl"),
	cycleStateChannel(this,"WCT_cycleStateChnl"),
	machineStateChannel(this,"WCT_machineStateChnl"),
	currentStepTimer(this,"WCT_currentStepTmr"),
	listeners(),
	ongoing(),
	runState(cycleState.STOP),
	machineState()
{}

void washingCycleTask::addCycleStateListener(cycleStateListener& listener){
	listeners.push_back(listener);
}

bool washingCycleTask::assessProgress(cycleStep& currentStep){
	//timed steps are handled by the timer, and don't need further confirmation.
	if (currentStep.isTimed()){
		return false;
	}
	//non-timed tasks finish when the water level and temperature are equal to,
	//or greater than the expected level. Assume the MIT is responsible for
	//keeping the balance.
	if(currentStep.getTemperature() < this->machineState.temperature ||
		currentStep.getWaterLevel() < this->machineState.waterLevel){
		return false;
	}
	return true;
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

void washingCycleTask::runCycle(washingCycle& toRun){
	this->loadCycleChannel.write(toRun);
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
				while(this->state == cycleState.PAUSE){
					this->state = this->cycleStateChannel.read();
					if (this->state == cycleState.STOP){
						brake = true;
					}
				}
				break;
			}
			
			if (brake){
				break;
			}
			
			//TODO: Once the relevant interface is known, instruct the Washing
			//machine in regards to the current step.
			
			//TODO: handle the current state of the machine, based on the most
			//recent events.
			
			if (progress == currentStepTimer||assessProgress()){
				//some time-limited step has expired, or the latest state of the
				//machine is sufficient. Move to the next step.
				cycleStep currentStep = this->ongoing.next();
				notifyListeners();
				continue;
			}
		}
	}
}


