//**************************************
//! Task responsible for ...
//! \authors
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __display_TASK
#define __display_TASK

#include "prtos/pRTOS.h"
#include "loadCycleTask.h"
#include "washingCycleTask.h"
#include "cycleStateListener.h"
#include "machineStateListener.h"
#include <string>
#include <vector>

public class displayTask : public RTOS::Task, public machineStateListener, 
						   public cycleStateListener
{
	displayTask(washingCycleTask* WCT, loadCycleTask* LCT);
	void stateChanged(MachineState currentState) override;
	void cycleStateChanged(unsigned int totalSteps,
		unsigned int currentStep,
		const string& cycleName,
		const string& stepName)=0 override;
	void cyclePaused(const string& cycleName,
		const string& stepName)=0 override;
	void cycleEnded(bool finished,
		const string& cycleName,
		const string& stepName)=0 override;
		
	void setCycleState(std::string state);
	void loadWashingCycle(std::string userName, std::string washingCycleName);
	
	protected:
		main(void);
		
	private:
	
	RTOS::channel<MachineState,1> machineStateChannel;
	RTOS::channel<std::string,16> washingCycleStateChannel;
	washingCycleTask WCT;
	loadCycleTask LCT;
	
	MachineState currentState;
};



#endif
