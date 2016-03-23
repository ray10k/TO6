//******************************************************************************
//! Regelt het ontvangen en verzenden van dataWebsocketPackages naar en van WebSockets.
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
 * De MessageQueue is een class die dataWebsocketPackages ontvangt en verzendt van en naar
 * WebSockets. Hij slaat de received WebsocketPackages op in een queue's
 * en andere klassen kunnen de received WebsocketPackages lezen. WebsocketPackages die verzonden 
 * worden, worden ook in een queue opgeslagen.
 * Ook kan er worden gebroadcast.
 */
class MessageQueue : public WebSocketListener {
public:

	MessageQueue();
	~MessageQueue();

	//! Geeft een boolean terug die aangeeft of de messagequeue intern WebsocketPackages 
	//! heeft die ontvangen zijn
	bool contains_received_WebsocketPackages();

	//! Leest een WebsocketPackage uit de lijst van ontvangen WebsocketPackages en geeft deze terug.
	//! Haalt de eerste WebsocketPackage uit de lijst.
	WebsocketPackage pop_WebsocketPackage();

	//! Zend een WebsocketPackage.
	/*!
	 \param p De WebsocketPackage die verzonden moet worden.
	 */
	void send_WebsocketPackage(WebsocketPackage p);

	//! Voegt een socket toe aan de interne lijst
	/*!
	 \param socket De socket die toegevoegd moet worden.
	 */
	void add_socket(WebSocket * socket);

	//! Een callback die aangeroepen kan worden als een websocket iets ontvangt.
	/*!
	 \param message Een const string die de message representeert
	 \param ws De websocket die het ontving
	 */
	void onTextMessage(const string& message, WebSocket* ws);

	//! Een callback die kan worden aangeroepen als een websocket sluit
	/*!
	 \param ws De websocket die sluit.
	 */
	void onClose(WebSocket * ws);


private:
	std::queue<WebsocketPackage> send, receive;
	std::vector<WebSocket *> sockets;
};


