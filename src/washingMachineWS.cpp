#include "washingMachineWS.h"

washingMachineWS::washingMachineWS(int portNr, userInteractionTask * talkTo):
	task(talkTo),
	internalLock(),
	incomingMessages(),
	outgoingMessages(),
	listener(this->startListener,portNr),
	queuePassthrough(this->startQueuePassthrough)
	{}
	
void washingMachineWS::startListener(int portNr){
	//make the connection in a separate thread, even though WebSocket itself
	//also spins off a thread, so that any number of connections can be made
	//without blocking the main thread.
	while (1==1){
		TCPServerSocket serverSock(portNr);
		TCPSocket * actualSock = serverSock.accept();
		WebSocket * webSock = new webSocket(actualSock);
		webSock->setListener(this);
	}
}

void washingMachineWS::startQueuePassthrough(){
	
}	
