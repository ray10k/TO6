#include "washingCycleTask.h"

washingCycleTask::washingCycleTask():
	loadCycleChannel(this,"WCT_loadCycleChnl"),
	cycleStateChannel(this,"WCT_cycleStateChnl"),
	machineStateChannel(this,"WCT_machineStateChnl"),
	currentStepTimer(this,"WCT_currentStepTmr"),
	listeners(),
	ongoing(string("not started yet")),
	state(cycleState.STOP)
{}

void washingCycleTask::addCycleStateListener(cycleStateListener* listener){
	listeners.push_back(*listener);
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
		do {
			wait(cycleStateChannel);
			this->state = wait.read();
		while (this->state != cycleState.RUN);
		
	}
}
