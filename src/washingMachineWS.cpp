#include "washingMachineWS.h"

washingMachineWS::washingMachineWS(int portNr, userInteractionTask * talkTo):
	task(talkTo),
	internalLock(),
	incomingMessages(),
	listeners(),
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
		internalListener * listen = new internalListener( webSock);
		listeners.push_back(listen);
	}
}

void washingMachineWS::startQueuePassthrough(){
	//this thread is supposed to push stuff to the userInteractionTask.
	//Figure out what can come in, and what the UIT expects.
}	

washingMachineWS::internalListener::internalListener(WebSocket * client, 
	washingMachineWS * parent):
	myClient(client),
	myParent(parent){
	client->setListener(this);
}

washingMachineWS::internalListener::~internalListener(){
	delete myClient;
}

void washingMachineWS::internalListener::onTextMessage(const std::string& s, 
	WebSocket* ws){
	//doodle doodle doodle
	//(no idea what the (...) should even happen here.)
}

void washingMachineWS::internalListener::onClose(WebSocket* ws){
	//time to clean up.
	myParent->unregisterConnection(this);
}
