#include "washingMachineWS.h"

socketConnection::socketConnection(WebSocket* thisIsMe, 
	washingMachineWS * parent):
	currentState(),
	updated(false),
	myParent(parent){
	//doodle
}

socketConnection::~socketConnection(){
	//doodle
}

socketConnection::socketConnection(const socketConnection& other):
	currentState(other.currentState),
	updated(true),
	myParent(other.myParent){
	//doodle
}

void socketConnection::machineUpdateHappened(MachineState current){
	//inform connected client. (got to figure out the comm. protocol here)
}

void socketConnection::onTextMessage(const string& s, WebSocket* ws){
	//user wants something now.
}

void socketConnection::onClose(WebSocket* ws){
	//time to die.
	this->myParent->disposeConnection(this);
}

void washingMachineWS::run(){
	
}

void washingMachineWS::updateMachineState(MachineState current){
	
}

void washingMachineWS::disposeConnection(socketConnection * toClose){
	
}

washingMachineWS * newWebSocket(int portNr, userInteractionTask* myBuddy){
	washingMachineWS washmachine(portNr,myBuddy);
	std::thread listener(&washingMachineWS::run(), &washmachine);
	return &washmachine;
}
