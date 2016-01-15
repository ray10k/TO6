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
	std::vector<cycleStateListener>::iterator current = this->listeners.begin();
	
	switch(this->state){
	case cycleState.STOP:
		for(;current != this->listeners.end(); current++){
			//bool properEnd = //TODO: figure out how to determine if a cycle
			//ended due to a STOP by the user, or by reaching the end of the
			//cycle.
			*current.cycleEnded(properEnd,);
		}
		break;
	case cycleState.PAUSE:
			
		break;
	case cycleState.RUN:
		
		break;
	}
}

void washingCycleTask::runCycle(washingCycle& toRun){
	this->loadCycleChannel.write(toRun);
}

void washingCycleTask::main(){
	while (1==1){
		wait(loadCycleChannel);
	}
}
