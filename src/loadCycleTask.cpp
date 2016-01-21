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
	std::vector<UserWashingCycle>::iterator cycle = this->washingCycles.begin();
	for(;cycle != this->washingCycles.end(); ++cycle)
	{
		if(cycle.user.userName == userName && cycle.cycle.getName() == washingCycleName)
		{
			this->loadWashingCycleChannel.write(cycle);
		}
	}
}

void loadCycleTask::addWashingCycle(std::string userName, washingCycle cycle)
{
	washingCycles.push_back({userName, cycle});
}

