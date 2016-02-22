#ifndef __WASHINGMACHINEWS_H
#define __WASHINGMACHINEWS_H

#include "webserver/PracticalSocket.h"
#include "webserver/websocket.h"
#include "cycleState.h"
#include "userInteractionTask.h"
#include "readBlockingQueue.h"
#include "machineStateListener.h"
#include <string>
#include <sstream>
#include <set>
#include <thread>
#include <chrono>

//******************************************************************************
//! Container for a single live websocket connection.
//! 
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//******************************************************************************

class washingMachineWS;

class socketConnection: public WebSocketListener{
public:
	socketConnection(WebSocket* thisIsMe, washingMachineWS * parent);
	virtual ~socketConnection();
	socketConnection(const socketConnection& other);
	
	void machineUpdateHappened(MachineState current);
	void onTextMessage(const std::string& s, WebSocket* ws);
	void onClose(WebSocket* ws);
	std::string getAddress() ;
	
private:
	void update();
	MachineState currentState;
	bool updated;
	washingMachineWS * myParent;
	WebSocket * connection;
};

//Why can't a socket get its address in a const-safe way?
struct scPointerComp {
	bool operator() ( socketConnection* left, socketConnection* right) const {
		return (left->getAddress().compare(right->getAddress()) > 0);
	}
};


//******************************************************************************
//! Container for all live websocket connections. 
//! 
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//******************************************************************************

class washingMachineWS{
public:
	friend washingMachineWS * newWebSocket(int portNr, 
		userInteractionTask* myBuddy);
	void acceptConnections();
	void passAlongCommands();
	void updateMachineState(MachineState current);
	void command(const std::string& rawInput);
	void disposeConnection(socketConnection * toClose);
private:
	bool machineUpdated;
	MachineState latest;
	washingMachineWS(int portNr, userInteractionTask * talkTo);
	userInteractionTask* buddy;
	readBlockingQueue<std::string> queuedCommands;
	std::set<socketConnection*,scPointerComp> liveConnections;
};


#endif
