#pragma once
#include "prtos/pRTOS.h"
#include <thread>
#include "MessageQueue.h"
#include "webserver/websocket.h"


//! Accepteert websocket verbindingen
/*!
 * De WebsocketAccepter accepteert websocket verbindingen en geeft deze door aan
 * een interne MessageQueue. Andere classes kunnen deze opvragen en via deze 
 * class toegang krijgen tot de datapackets die worden verstuurd.
 */
class WebsocketAccepter {

public:
	//! Constructor, also starts the server
	/*!
	 \param port De poort waarop de websocket server draait
	 */
	WebsocketAccepter(int port);

	//! Getter voor de interne MessageQueue
	MessageQueue & get_message_queue();

private:
	int port;
	MessageQueue mqueue;

	void run();
};
