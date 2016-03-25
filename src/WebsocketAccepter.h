//******************************************************************************
//! Accepts websocket connections
//! 
//! \authors
//! 	- Daniel Klomp
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//******************************************************************************

#pragma once
#include "prtos/pRTOS.h"
#include <thread>
#include "MessageQueue.h"
#include "webserver/websocket.h"

 
 /*!
 * The WebsocketAccepter accepts websocket connections and gifs them to the MessageQueue. 
 */
class WebsocketAccepter {

public:
	//! Constructor, also starts the server
	/*!
	 \param port The port of the websocket
	 */
	WebsocketAccepter(int port);

	//! Gets the MessageQueue
	MessageQueue & get_message_queue();

private:
	int port;
	MessageQueue MQ;

	void run();
};
