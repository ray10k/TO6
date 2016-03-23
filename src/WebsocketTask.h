//******************************************************************************
//! Regelt de verbinding tussen het programma en de websockets.
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
#include "webserver/websocket.h"
#include "WebsocketPackage.h"
#include "WebsocketAccepter.h"
#include <string>

class userInteractionTask;

/*!
 * De WebsocketTask regelt de verbinding tussen het programma en de 
 * websockets. Op dit moment is dat alleen het doorgeven van berichten tussen de
 * MessageQueue en de WashInterfaceController. De class is echter nuttig in de 
 * toekomst. De security kan hier makkelijk worden geimplementeerd en ook
 * dingen als latency hoeven niet perse in de controller die de echte interface 
 * bestuurt.
 */
class WebsocketTask : public RTOS::task {

    public:
		//! Constructor
		/*!
		 \param wic referentie naar de WashInterfaceController
		 */
        WebsocketTask( userInteractionTask* uit);

		//! Zend een bericht naar de socket met het gegeven id
		/*!
		 \param id De id van de socket
		 \param message Het bericht om te verzenden als std::string
		 */
        void sendPackageChannel_message(int id, std::string message);

		//! Stuurt een bericht naar alle sockets.
		/*!
		 \param message Het bericht om te verzenden
		 */
        void broadcast(std::string message);

    private:

		void main();
        RTOS::channel<WebsocketPackage, 10> sendPackageChannelPackageChannel;
		RTOS::mutex socket_write;
		RTOS::clock poll_clock;

        userInteractionTask* uit;
		WebsocketAccepter wsa;
		MessageQueue &MQ;
};
