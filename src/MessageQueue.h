//******************************************************************************
//! MessageQueue is a class which receives and sends dataWebsocketPackages from and to WebSockets. 
//! 
//! \authors
//! 	- Daniel Klomp
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//******************************************************************************

#pragma once

#include "webserver/websocket.h"
#include "WebsocketPackage.h"
#include <queue>
#include <algorithm>

/*!
 * MessageQueue is a class which receives and sends dataWebsocketPackages from and to WebSockets. 
 * these WebsocketPackages are saved in a queue and other classes can read those
 */
 
class MessageQueue : public WebSocketListener {
public:

	MessageQueue();
	~MessageQueue();

	//! Returns a boolean which is True if the messagequeue contains received WebsocketPackages
	bool contains_received_WebsocketPackages();

	//! Reads and returns the first WebsocketPackage out of all the received WebsocketPackages
	WebsocketPackage pop_WebsocketPackage();

	//! Sends a WebsocketPackage.
	/*!
	 \param p The WebsocketPackage that will be send
	 */
	void send_WebsocketPackage(WebsocketPackage p);

	//! Adds a socket to the internal list
	/*!
	 \param The socket that will be added
	 */
	void add_socket(WebSocket * socket);

	//! A callback that can be called when a websocket receives something
	/*!
	 \param message A const string which represents the message
	 \param ws The WebSocket that received the message
	 */
	void onTextMessage(const string& message, WebSocket* ws);

	//! A callback that can be called when a websocket closes
	/*!
	 \param ws The websocket that closes.
	 */
	void onClose(WebSocket * ws);


private:
	std::queue<WebsocketPackage> send, receive;
	std::vector<WebSocket *> sockets;
};


