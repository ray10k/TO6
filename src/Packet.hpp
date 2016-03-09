#pragma once
#include <string>

//! Een entity class die een datapacket opslaat, waarin de textmessage zit en de id vd socket waar deze vandaan komt
/*!
 * Packet is een entity class die een datapacket opslaat, waarin de textmessage zit en de id vd socket waar deze vandaan komt.
 * Deze wordt gebruikt om deze data door te geven via classes
 */
class Packet {

    public:

        //! Lege constructor van packet. De gemaakte packet is een invalid packet die niet verstuurd kan worden.
        Packet() : msg(""), id(-2) {}

        //! Constructor van packet
        /*!
         \param id_var De id van de socket waar hij heen moet worden gestuurd of vandaan komt.
         \param msg_var Het bericht dat moet worden verstuurd of is ontvangen
         */
        Packet(int id_var, std::string msg_var) :
        msg(msg_var),
        id(id_var)
        {}

        //! Getter voor de ID vd socket als integer.
        int get_socket() {
            return id;
        }
        //! Getter voor het bericht als std::string.
        std::string get_message() {
            return msg;
        }

    private:
        std::string msg;
        int id;
};
