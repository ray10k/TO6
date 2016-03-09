#include "MessageQueue.hpp"

MessageQueue::MessageQueue() {
}

MessageQueue::~MessageQueue() {
	for (auto i = sockets.begin(); i != sockets.end(); i++) {
		delete (*i);
		//TODO Some neat cleanup, this is scary.
	}
}

bool MessageQueue::contains_received_packets() {
	return !receive.empty();
}

Packet MessageQueue::pop_packet() {
	auto front = receive.front();
	receive.pop();
	return (Packet) front;
}

void MessageQueue::send_packet(Packet p) {
	if (p.get_socket() != -1) {
		int index = 0;
		for (auto i = sockets.begin(); i != sockets.end(); i++, index++) {
			if ((*i)->getId() == p.get_socket()) {
				if ((*i)->isClosed()) {
					delete (*i);
					sockets.erase(sockets.begin() + index);
				};

				try {
					(*i)->sendTextMessage(p.get_message());
				}
				catch(WebSocketException e) {
					std::cout << e.what() << "\r\n";
					break;
				}
			}
		}
	}
	else {
		for (auto i = sockets.begin(); i != sockets.end(); i++) {
			try {
				(*i)->sendTextMessage(p.get_message());
			}
			catch(WebSocketException e) {
				std::cout << e.what() << "\r\n";
				break;
			}
		}
	}
}

void MessageQueue::onTextMessage(const string& message, WebSocket* ws) {
	//Finding an item in a vector of pointers, using a lamda
	int pos = find_if(sockets.begin(), sockets.end(),
	                  [&ws](const WebSocket* p) {
		                  return p->getId() == ws->getId();
	                  })
	          - sockets.begin();
	if((unsigned int)pos >= sockets.size()) {
		return;
	}
	WebSocket * s_at_index = sockets.at(pos);
	receive.push(Packet(s_at_index->getId(), message));
	std::cout << receive.size() << endl;
}

void MessageQueue::onClose(WebSocket* ws)  {
	//Finding an item in a vector of pointers, using a lamda
	int pos = find_if(sockets.begin(), sockets.end(),
	                  [&ws](const WebSocket* p) {
		                  return p->getId() == ws->getId();
	                  })
	          - sockets.begin();

	if( (unsigned int) pos >= sockets.size()) {
		return;
	}

	delete sockets.at(pos);
	sockets.erase(sockets.begin() + pos);
}

void MessageQueue::add_socket(WebSocket * s) {
	sockets.push_back(s);
}