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
#include "cycleState.h"
#include "loadCycleTask.h"
#include "washingCycleTask.h"
#include "cycleStateListener.h"
#include "machineStateListener.h"
#include <string>
#include <vector>

public class displayTask : public RTOS::Task, public machineStateListener, 
						   public cycleStateListener
{
	//! Constructor.
	displayTask(washingCycleTask* WCT, loadCycleTask* LCT);
	//! See machineStateListener.h
	void stateChanged(machineState currentState) override;
	//! see cycleStateListener.h
	void cycleStateChanged(unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName) override;
	//! see cycleStateListener.h
	void cyclePaused(const std::string& cycleName,
		const std::string& stepName) override;
	//! see cycleStateListener.h
	void cycleEnded(bool finished,
		const std::string& cycleName,
		const std::string& stepName) override;
	//! TODO: document this.
	void setCycleState(int state);
	//! TODO: document this.
	void loadWashingCycle(std::string userName, std::string washingCycleName);
	
	protected:
		main(void);
		
	private:
	
	RTOS::channel<machineState,1> machineStateChannel;
	RTOS::channel<std::string,16> washingCycleStateChannel;
	washingCycleTask WCT;
	loadCycleTask LCT;
	
	machineState currentState;
};



#endif
