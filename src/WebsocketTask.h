//******************************************************************************
//! The WebsocketTask controls the connection between the UserintectionTask and websockets
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
 * The WebsocketTask controls the connection between the UserintectionTask and websockets.
 * This is done by sending messages between the MessageQueue and the UserintectionTask
 */
 
class WebsocketTask : public RTOS::task {

    public:
		//! Constructor
		/*!
		 \param uit Reference to the UserintectionTask
		 */
        WebsocketTask( userInteractionTask* uit);

		//! Sends a message to the socket with the given id
		/*!
		 \param id The id of the Socket
		 \param message The message that will be send
		 */
        void sendPackageChannel_message(int id, std::string message);

		//! Sends a message to all the sockets
		/*!
		 \param message The message that will be send
		 */
        void broadcast(std::string message);

    private:

		void main();
        RTOS::channel<WebsocketPackage, 10> sendPackageChannel;
		RTOS::clock poll_clock;

        userInteractionTask* uit;
		WebsocketAccepter wsa;
		MessageQueue &MQ;
};
