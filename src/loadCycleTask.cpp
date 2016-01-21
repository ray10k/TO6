#include "loadCycleTask.h"

loadCycleTask::loadCycleTask(washingCycleTask* WCT):
  loadWashingCycleChannel (this, "load_Washing_Cycle_Channel"),
  washingCycles(),
  WCT(WCT)
{
	washingCycle cycle = new washingCycle("cycle1");
	cycle.addStep({"step1",60,75,true,10})
	addWashingCycle("Admin", cycle);
}

void loadCycleTask::main()
{
	for(;;)
	{
		RTOS::wait(this->loadWashingCycleChannel);
		UserWashingCycle loadedWashingCycle = loadWashingCycleChannel.read();
		WCT.loadCycle(loadedWashingCycle.cycle);
	}
}

void loadCycleTask::loadWashingCycle(std::string userName, std::string washingCycleName)
{
	UserWashingCycle cycle = findUserWashingCycle(userName,"");
	if(cycle.userName == userName && cycle.cycle.getName() == washingCycleName)
	{
		this->loadWashingCycleChannel.write(cycle);
	}
}

void loadCycleTask::addWashingCycle(std::string userName, washingCycle cycle)
{
	washingCycles.push_back({userName, cycle});
}

UserWashingCycle loadCycleTask::findUserWashingCycle(
	std::string userName, std::string washingCycleName)
{
	std::vector<UserWashingCycle>::iterator cycle = this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if(cycle.userName == userName || cycle.cycle.getName() == washingCycleName)
		{
			return cycle;
		}
	}
	washingCycle emptyCycle;
	return {"", emptyCycle};
}

std::vector<std::string> loadCycleTask::getWashingCycleNames(std::string userName)
{
	std::vector<std::string> cycleNames;
	std::vector<UserWashingCycle>::iterator cycle = this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if(cycle.userName == userName)
		{
			cycleNames.push_back(cycle.cycle.getName());
		}
	}
	return cycleNames;
}

int loadCycleTask::getTotalCycleSteps(std::string washingCycleName)
{
	if(findUserWashingCycle("",washingCycleName).cycle.getName() == washingCycleName)
	{
		return cycle.cycle.totalSteps();
	}
	return 0;
}