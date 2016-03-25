//******************************************************************************
//! Een entity class die een dataWebsocketPackage opslaat, waarin de textmessage zit en de
//! id vd socket waar deze vandaan komt
//! 
//! \authors
//! 	- Daniel Klomp
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//******************************************************************************

#pragma once
#include <string>

/*!
 * WebsocketPackage is een entity class die een dataWebsocketPackage opslaat, waarin de textmessage 
 * zit en de id vd socket waar deze vandaan komt.
 * Deze wordt gebruikt om deze data door te geven via classes
 */
 
 /*!
 * WebsocketPackage saves the dataWebsocketPackage, which holds the textmessage and the socket id.
 */
class WebsocketPackage {

    public:

        //! Empty constructor for WebsocketPackage.
        WebsocketPackage() : message(""), id(-2) {}

        //! Constructor for WebsocketPackage
		 /*!
		 \param id_var The id of the socket to where it will be send or received form.
         \param message_var The recieved message.
         */
        WebsocketPackage(int id_var, std::string message_var) :
        message(message_var),
        id(id_var)
        {}

        //! Gets the id of the socket.
        int get_socket() {
            return id;
        }
        //! Gets the message.
        std::string get_message() {
            return message;
        }

    private:
        std::string message;
        int id;
};
