#ifndef __WASHINGMACHINEWS_H
#define __WASHINGMACHINEWS_H

#include "webserver/PracticalSocket.h"
#include "webserver/websocket.h"
#include "userInteractionTask.h"

class washingMachineWS:public WebSocketListener{
public:
	washingMachineWS(int portNr, userInteractionTask& talkTo);
	void start();
	
	void onTextMessage(const string& s, WebSocket* ws);
	void onClose(WebSocket* ws);
	
private:
	threadSafeQueue 
}

#endif
