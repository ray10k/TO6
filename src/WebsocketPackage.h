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
class WebsocketPackage {

    public:

        //! Lege constructor van WebsocketPackage. De gemaakte WebsocketPackage is een invalid 
        //! WebsocketPackage die niet verstuurd kan worden.
        WebsocketPackage() : message(""), id(-2) {}

        //! Constructor van WebsocketPackage
        /*!
         \param id_var De id van de socket waar hij heen moet worden gestuurd of 
         vandaan komt.
         \param message_var Het bericht dat moet worden verstuurd of is ontvangen
         */
        WebsocketPackage(int id_var, std::string message_var) :
        message(message_var),
        id(id_var)
        {}

        //! Getter voor de ID vd socket als integer.
        int get_socket() {
            return id;
        }
        //! Getter voor het bericht als std::string.
        std::string get_message() {
            return message;
        }

    private:
        std::string message;
        int id;
};
