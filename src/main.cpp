#include "washingCycleTask.h"
#include "machineInteractionTask.h"
#include "userInteractionTask.h"
#include "washingMachineWS.h"
#include <cstdlib>

#define PORTNO 25565

int main(int argc, char* argv[])
{
#ifdef DEBUG
	cout << "starting program." << std::endl;
#endif
	machineInteractionTask * MIT = new machineInteractionTask();
    washingCycleTask * WCT = new washingCycleTask(*MIT);
	userInteractionTask * UIT = new userInteractionTask(WCT);

	MIT->addMachineStateListener(WCT);
	MIT->addMachineStateListener(UIT);

	WCT->addCycleStateListener(UIT);
	
	washingMachineWS * wmws = washingMachineWS::newWebSocket(PORTNO,UIT);
	UIT->setWebsocket(wmws);
#ifdef DEBUG
	cout << "setup complete, entering running phase." << std::endl;
#endif
	RTOS::run();

    return 0;
}
