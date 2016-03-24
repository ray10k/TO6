#include "WebsocketTask.h"
#include "userInteractionTask.h"

WebsocketTask::WebsocketTask(userInteractionTask* uit) :
    task{100, "WebsocketTask"},
    sendPackageChannel{this, "sendPackageChannel"},
    poll_clock{ this, 100 MS, "poll"},
    uit(uit),
    wsa{2000},
    MQ(wsa.get_message_queue())
    {

    }

void WebsocketTask::main() {
#ifdef DEBUG
	std::cout << "WC started."<< std::endl;
#endif
    for (;;) {

        RTOS::event event = wait(sendPackageChannel + poll_clock);

        if (event == sendPackageChannel) {
            WebsocketPackage p = sendPackageChannel.read();
            MQ.send_WebsocketPackage(p);
        }
        else { 
			while (MQ.contains_received_WebsocketPackages()) {
				WebsocketPackage p = MQ.pop_WebsocketPackage();
				uit->WebsocketPackage_received(p);
			}
        }
    }
}

void WebsocketTask::sendPackageChannel_message(int id, std::string message) {
    sendPackageChannel.write(WebsocketPackage(id, message));
}

void WebsocketTask::broadcast(std::string message) {
    sendPackageChannel.write(WebsocketPackage(-1, message));
}
