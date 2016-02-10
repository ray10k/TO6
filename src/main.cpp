#include <washingCycleTask.h>
#include <loadCycleTask.h>
#include <machineInteractionTask.h>
#include <userInteractionTask.h>
#include <pthread.h>
#include <cstdlib>

void *webSocket(void * nothing);

int main(int argc, char* argv[])
{
	pthread_t socketThread;
	int threadStart;
	threadStart = pthread_create (socketThread, NULL, webSocket, NULL);
	
	//couldn't start the thread for the socket, time to panic.
	if(threadStart != 0){
		abort();
	}
	
	machineInteractionTask MIT = new machineInteractionTask();
    washingCycleTask WCT = new washingCycleTask(MIT);
	loadCycleTask LCT = new loadCycleTask(WCT);
	userInteractionTask UIT = new userInteractionTask(WCT, LCT);

	MIT.addMachineStateListener(WCT);
	MIT.addMachineStateListener(UIT);

	WCT.addCycleStateListener(UIT);
    return 0;
}

void *webSocket(void * nothing){
	//doodle doodle doodle
}

#endif
