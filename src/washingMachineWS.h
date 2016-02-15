#ifndef __WASHINGMACHINEWS_H
#define __WASHINGMACHINEWS_H

#include "webserver/PracticalSocket.h"
#include "webserver/websocket.h"
#include "userInteractionTask.h"
#include "readBlockingQueue.h"
#include <string>
#include <deque>
#include <mutex>
#include <thread>

class washingMachineWS:public WebSocketListener{
public:
	washingMachineWS(int portNr, userInteractionTask * talkTo);
	void startListener();
	void startQueuePassthrough();
	
	void onTextMessage(const std::string& s, WebSocket* ws);
	void onClose(WebSocket* ws);
	
private:
	userInteractionTask * task;
	std::mutex internalLock;
	std::deque<std::string> incomingMessages;
	std::thread listener, queuePassThrough;
	readBlockingQueue<std::string> outgoingMessages;
};

#endif
