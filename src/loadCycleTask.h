//**************************************
//! Task responsible for ...
//! \authors
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __LOAD_CYCLE_TASK
#define __LOAD_CYCLE_TASK

#include "prtos/pRTOS.h"
#include "washingCycleTask.h"
#include ""washingCycle.h"
#include <string>
#include <vector>

struct UserWashingCycle
{
	std::string userName;
	washingCycle cycle;
}

public class loadCycleTask : public RTOS::Task
{
	loadCycleTask(washingCycleTask* WCT);
	void loadWashingCycle(std::userName, std::string washingCycleName);
	void addWashingCycle(UserWashingCycle& cycle);
	
	protected:
		main(void);
		
	private:
	
	RTOS::channel<UserWashingCycle,1> loadWashingCycleChannel;
	washingCycleTask WCT;
	
	std::vector<UserWashingCycle&> washingCycles;
}