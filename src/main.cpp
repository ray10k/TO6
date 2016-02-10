#include "washingCycleTask.h"
#include "loadCycleTask.h"
#include "machineInteractionTask.h"
#include "userInteractionTask.h"
#include <thread>
#include <cstdlib>

void webSocket(userInteractionTask * user);

int main(int argc, char* argv[])
{
	machineInteractionTask MIT = new machineInteractionTask();
    washingCycleTask WCT = new washingCycleTask(MIT);
	loadCycleTask LCT = new loadCycleTask(WCT);
	userInteractionTask UIT = new userInteractionTask(WCT, LCT);

	MIT.addMachineStateListener(WCT);
	MIT.addMachineStateListener(UIT);

	WCT.addCycleStateListener(UIT);
	
	std::thread webSocketThread(webSocket, &UIT);
	
	rtos::run();
	
    return 0;
}

void webSocket(userInteractionTask * user){
	
}

#endif
