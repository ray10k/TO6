#include "washingCycleTask.h"
#include "machineInteractionTask.h"
#include "userInteractionTask.h"
#include "WebsocketController.h"
#include <cstdlib>


int main(int argc, char* argv[])
{
#ifdef DEBUG
	cout << "starting program." << std::endl;
#endif
	machineInteractionTask * MIT = new machineInteractionTask();
    washingCycleTask * WCT = new washingCycleTask(MIT);
	userInteractionTask * UIT = new userInteractionTask(WCT);

	MIT->addMachineStateListener(WCT);
	MIT->addMachineStateListener(UIT);

	WCT->addCycleStateListener(UIT);
	
	WebsocketController WSC = WebsocketController(UIT);
	UIT->setWebsocket(&WSC);
#ifdef DEBUG
	cout << "setup complete, entering running phase." << std::endl;
#endif
	RTOS::run();

    return 0;
}
