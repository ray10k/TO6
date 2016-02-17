#include "washingCycleTask.h"
#include "machineInteractionTask.h"
#include "userInteractionTask.h"
#include <cstdlib>

#define PORTNO 25565

void webSocket(userInteractionTask * user);

int main(int argc, char* argv[])
{
	machineInteractionTask * MIT = new machineInteractionTask();
    washingCycleTask * WCT = new washingCycleTask(*MIT);
	userInteractionTask * UIT = new userInteractionTask(WCT);

	MIT->addMachineStateListener(*WCT);
	MIT->addMachineStateListener(*UIT);

	WCT->addCycleStateListener(UIT);
	
	washingMachineWS websock(PORTNO, UIT);

	RTOS::run();

    return 0;
}
