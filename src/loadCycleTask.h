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
#include "washingCycle.h"
#include <string>
#include <vector>

struct UserWashingCycle
{
	std::string userName;
	washingCycle cycle;
};

class loadCycleTask : public RTOS::task
{
	loadCycleTask(washingCycleTask* WCT);
	void loadWashingCycle(std::userName, std::string washingCycleName);
	void addWashingCycle(std::string userName, washingCycle cycle);
	std::vector<std::string> getWashingCycleNames(std::string userName);
	int getTotalCycleSteps(std::string washingCycleName)

	protected:
		main(void);

	private:
	UserWashingCycle findUserWashingCycle(std::string userName, std::string washingCycleName);

	RTOS::channel<UserWashingCycle,1> loadWashingCycleChannel;
	washingCycleTask WCT;

	std::vector<UserWashingCycle&> washingCycles;
}

#endif // __LOAD_CYCLE_TASK
