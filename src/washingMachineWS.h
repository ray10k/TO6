#ifndef __WASHINGMACHINEWS_H
#define __WASHINGMACHINEWS_H

#include "webserver/PracticalSocket.h"
#include "webserver/websocket.h"
#include "userInteractionTask.h"
#include "writeBlockingQueue.h"
#include "readBlockingQueue.h"
#include <string>

class washingMachineWS:public WebSocketListener{
public:
	washingMachineWS(int portNr, userInteractionTask& talkTo);
	void start();
	
	void onTextMessage(const string& s, WebSocket* ws);
	void onClose(WebSocket* ws);
	
private:
	writeBlockingQueue<string> incomingMessages;
	readBlockingQueue<string> outgoingMessages;
};

#endif
