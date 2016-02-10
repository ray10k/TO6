#include <washingCycleTask.h>
#include <loadCycleTask.h>
#include <machineInteractionTask.h>
#include <userInteractionTask.h>
#include <pthread.h>

int main(int argc, char* argv[])
{
	//pthread_create (thread, attr, start_routine, arg)
	
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
