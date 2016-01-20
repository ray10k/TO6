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
#include <string>
#include <vector>

struct UserCycleName
{
	std::string userName;
	std::string cycleName;
}

public class loadCycleTask : public RTOS::Task
{
	loadCycleTask(washingCycleTask* WCT);
	void loadWashingCycle(std::userName, std::string washingCycleName);
	
	protected:
		main(void);
		
	private:
	
	RTOS::channel<UserCycleName,1> loadWashingCycleChannel;
	washingCycleTask WCT;
}