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
	if (currentStep.isTimed()){
		return false;
	}
	
	//non-timed tasks finish when the water level and temperature are within a
	//given tolerance. Assume that +/- 1 degree or +/-1 % water level is the
	//acceptable tolerance.
	
}

void washingCycleTask::notifyListeners(){
	std::vector<cycleStateListener>::iterator listen = this->listeners.begin();
	
	switch(this->state){
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
		while(this->state != cycleState.RUN){ 
			//channel.read blocks until something can be read.
			this->state = this->cycleStateChannel.read();
		}
		//State: Waiting. Fetch the washing cycle as soon as it becomes
		//available.
		this->ongoing = this->loadCycleChannel.read();
		//State: Running. Check for the existence of a next cycle step, update 
		//state, confirm the program does not need to be paused or stopped, 
		//provide washing machine with current-step instructions.
		while(this->ongoing.hasNext()){
			cycleStep currentStep = this->ongoing.next();
			notifyListeners();
			if (currentStep.isTimed()){
				this->currentStepTimer.set(currentStep.getDuration() S);
			}
			RTOS::event progress = wait(cycleStateChannel + 
										machineStateChannel +
										currentStepTimer);
			if (progress == cycleStateChannel){
				//Guaranteed to be at least 1 item waiting in the channel,
				//so this will not block.
				this->state = this->cycleStateChannel.read();
			}
			
			bool brake = false;
			
			switch(this->state){
				case cycleState.RUN:
				break; //nothing to see here, move along.
				case cycleState.STOP:
				//TODO: figure out how to properly break out of the outer loop. 
				//Hack for now. Hope that labeled loop constructs were something
				//Java copied from c++...
				brake = true;
				break;
				case cycleState.PAUSE:
				//TODO: figure out a good way to handle this. Uuuuuuugh...
				break;
			}
			
			if (brake){
				break;
			}
			
			//TODO: Once the relevant interface is known, instruct the Washing
			//machine in regards to the current step.
			
			//TODO: handle the current state of the machine, based on the most
			//recent events.
			
			if (progress == currentStepTimer){
				//some time-limited step has expired, move along.
				continue;
			}
			
			if (assesProgress()){
				//Current machine state is close enough to a given state re.
				//temperature/water level, and the current step is not time-
				//limited. Move along.
				continue;
			}
		}
	}
}


