#include "WebsocketController.h"
#include "userInteractionTask.h"

WebsocketController::WebsocketController(userInteractionTask* uit) :
    task{100, "WebsocketController"},
    send{this, "send"},
    socket_write{"socket write mutex"},
    poll_clock{ this, 100 MS, "poll"},
    uit(uit),
    wsa{2000},
    mqueue(wsa.get_message_queue())
    {

    }

void WebsocketController::main() {
    for (;;) {

        auto event = wait(send + poll_clock);

        if (event == send) {
            Packet p = send.read();
            mqueue.send_packet(p);
        }
        else { 
			while (mqueue.contains_received_packets()) {
				Packet p = mqueue.pop_packet();
				uit->packet_received(p);
			}
        }
    }
}

void WebsocketController::send_message(int id, std::string msg) {
    send.write(Packet(id, msg));
}

void WebsocketController::broadcast(std::string msg) {
    send.write(Packet(-1, msg));
}
