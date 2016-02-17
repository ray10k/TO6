#ifndef __WASHINGMACHINEWS_H
#define __WASHINGMACHINEWS_H

#include "webserver/PracticalSocket.h"
#include "webserver/websocket.h"
#include "userInteractionTask.h"
#include "readBlockingQueue.h"
#include "machineStateListener.h"
#include <string>
#include <deque>
#include <vector>
#include <mutex>
#include <thread>

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
	~socketConnection();
	socketConnection(const socketConnection& other);
	
	void machineUpdateHappened(MachineState current);
	void onTextMessage(const string& s, WebSocket* ws);
	void onClose(WebSocket* ws);
	
private:
	//you know what, let's just tempt fate and do this the thread-non-safe way.
	MachineState currentState;
	bool updated;
	washingMachineWS * myParent;
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

//seriously, c++ threads are a major headache.

class washingMachineWS{
public:
	friend washingMachineWS * newWebSocket(int portNr, 
		userInteractionTask* myBuddy);
	void run();
	void updateMachineState(MachineState current);
	void disposeConnection(socketConnection * toClose);
private:
	washingMachineWS(int portNr, userInteractionTask * talkTo);
	userInteractionTask* buddy;
};


#endif
