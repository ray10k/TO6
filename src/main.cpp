#include <washingCycleTask.h>
#include <loadCycleTask.h>
#include <machineInteractionTask.h>
#include <userInteractionTask.h>

int main(int argc, char* argv[])
{
	machineInteractionTask MIT = new machineInteractionTask();
    washingCycleTask WCT = new washingCycleTask(MIT);
	loadCycleTask LCT = new loadCycleTask(WCT);
	userInteractionTask UIT = new userInteractionTask(WCT, LCT);

	MIT.addMachineStateListener(WCT);
	MIT.addMachineStateListener(UIT);

	WCT.addCycleStateListener(UIT);
    return 0;
}

#endif
