#pragma once
#include "pRTOS.h"
#include "websocket_start.h"
#include "Packet.hpp"
#include "WebsocketAccepter.hpp"
#include <string>

class WashInterfaceController;

//! Regelt de verbinding tussen het programma en de websockets.
/*!
 * De WebsocketController regelt de verbinding tussen het programma en de websockets. Op dit moment is dat alleen het doorgeven van berichten tussen de
 * MessageQueue en de WashInterfaceController. De class is echter nuttig in de toekomst. De security kan hier makkelijk worden geimplementeerd en ook
 * dingen als latency hoeven niet perse in de controller die de echte interface bestuurt.
 */
class WebsocketController : public RTOS::task {

    public:
		//! Constructor
		/*!
		 \param priority is een integer om de prioriteit van de RTOS task te zetten
		 \param wic referentie naar de WashInterfaceController
		 */
        WebsocketController( unsigned int priority, userInteractionTask* uit);

		//! Zend een bericht naar de socket met het gegeven id
		/*!
		 \param id De id van de socket
		 \param msg Het bericht om te verzenden als std::string
		 */
        void send_message(int id, std::string msg);

		//! Stuurt een bericht naar alle sockets.
		/*!
		 \param msg Het bericht om te verzenden
		 */
        void broadcast(std::string msg);

    private:

		void main();
        RTOS::channel<Packet, 10> send;
		RTOS::mutex socket_write;
		RTOS::clock poll_clock;

        userInteractionTask* uit;
		WebsocketAccepter wsa;
		MessageQueue &mqueue;
};
