#include <washingCycleTask.h>
#include <loadCycleTask.h>
#include <machineInteractionTask.h>
#include <displayTask.h>

int main(int argc, char* argv[])
{
	machineInteractionTask MIT = new machineInteractionTask();
    washingCycleTask WCT = new washingCycleTask(MIT);
	loadCycleTask LCT = new loadCycleTask(WCT);
	displayTask DT = new displayTask(WCT, LCT);

	MIT.addMachineStateListener(WCT);
	MIT.addMachineStateListener(DT);

	WCT.addCycleStateListener(DT);
    return 0;
}

#endif
