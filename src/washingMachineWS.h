#ifndef __WASHINGMACHINEWS_H
#define __WASHINGMACHINEWS_H

#include "webserver/PracticalSocket.h"
#include "webserver/websocket.h"
#include "userInteractionTask.h"
#include "readBlockingQueue.h"
#include <string>
#include <deque>
#include <vector>
#include <mutex>
#include <thread>

class washingMachineWS{
public:
	washingMachineWS(int portNr, userInteractionTask * talkTo);

	class internalListener: public WebSocketListener{
	public:
		internalListener(WebSocket * client, washingMachineWS * parent);
		~internalListener();
		
		WebSocket* myClient;
		washingMachineWS * myParent;
		void onTextMessage(const std::string& s, WebSocket* ws);
		void onClose(WebSocket* ws);
	};
	
private:
	void startListener(int portNr);
	void unregisterConnection(internalListener * dead);
	void startQueuePassthrough();
	userInteractionTask * task;
	std::mutex internalLock;
	std::deque<std::string> incomingMessages;
	std::vector<internalListener*> listeners;
	readBlockingQueue<std::string> outgoingMessages;
	std::thread listener, queuePassthrough;
};

#endif
