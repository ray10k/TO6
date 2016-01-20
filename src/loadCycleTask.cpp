#include "loadCycleTask.h"

loadCycleTask::loadCycleTask(washingCycleTask* WCT):
  loadWashingCycleChannel (this, "load_Washing_Cycle_Channel")
{
	this->WCT = WCT;
}

void loadCycleTask::main()
{
	for(;;)
	{
		RTOS::wait(this->loadWashingCycleChannel);
		UserCycleName loadedWashingCycle = loadWashingCycleChannel.read();
		WCT.loadCycle(loadedWashingCycle.cycleName);
	}
}

void loadCycleTask::loadWashingCycle(std::string userName, std::string washingCycleName)
{
	this->loadWashingCycleChannel.write({userName, washingCycleName});
}