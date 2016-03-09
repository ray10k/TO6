#pragma once

#include "webserver/websocket.h"
#include "Packet.h"
#include <queue>
#include <algorithm>

//! Regelt het ontvangen en verzenden van datapackets naar en van WebSockets.
/*!
 * De MessageQueue is een class die datapackets ontvangt en verzendt van en naar
 * WebSockets. Hij slaat de received packets op in een queue's
 * en andere klassen kunnen de received packets lezen. Packets die verzonden 
 * worden, worden ook in een queue opgeslagen.
 * Ook kan er worden gebroadcast.
 */
class MessageQueue : public WebSocketListener {
public:

	MessageQueue();
	~MessageQueue();

	//! Geeft een boolean terug die aangeeft of de messagequeue intern packets 
	//! heeft die ontvangen zijn
	bool contains_received_packets();

	//! Leest een Packet uit de lijst van ontvangen packets en geeft deze terug.
	//! Haalt de eerste Packet uit de lijst.
	Packet pop_packet();

	//! Zend een packet.
	/*!
	 \param p De packet die verzonden moet worden.
	 */
	void send_packet(Packet p);

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
	std::queue<Packet> send, receive;
	std::vector<WebSocket *> sockets;
};


