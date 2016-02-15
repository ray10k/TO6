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

class washingMachineWS{
public:
	washingMachineWS(int portNr, userInteractionTask * talkTo);
	
	void onTextMessage(const std::string& s, WebSocket* ws);
	void onClose(WebSocket* ws);
	
private:
	class internalListener: public WebSocketListener{
		internalListener(Websocket* client);
		~internalListener();
	};
	void startListener(int portNr);
	void startQueuePassthrough();
	userInteractionTask * task;
	std::mutex internalLock;
	std::deque<std::string> incomingMessages;
	std::thread listener, queuePassthrough;
	readBlockingQueue<std::string> outgoingMessages;
};

#endif
