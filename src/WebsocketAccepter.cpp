#include "WebsocketAccepter.h"

WebsocketAccepter::WebsocketAccepter(int port) :
	port(port)
	{
		std::thread websocket_server(&WebsocketAccepter::run, this);
		websocket_server.detach();
	}

MessageQueue & WebsocketAccepter::get_message_queue() {
	return MQ;
}

void WebsocketAccepter::run() {
	TCPServerSocket servSock(port);
	try {
		int i = 0;
		for (; ;) {
			TCPSocket *sock = servSock.accept();
			WebSocket *ws = new WebSocket(i, sock);
			MQ.add_socket(ws);
			ws->setListener(&MQ);
			//ws->sendTextMessage("Welcome to the awesome washing machine");
#ifdef DEBUG
			std::cout << "new websocket" << std::endl;
#endif
			i++;
		}
	}
	catch (SocketException &e) {
		std::cout << e.what() << std::endl;
	}
}


