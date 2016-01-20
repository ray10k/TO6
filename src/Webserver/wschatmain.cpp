#include <iostream>
#include "PracticalSocket.h"
#include "websocket.h"
#include <thread>


using namespace std;

string username = "Admin";


class MyListener:public WebSocketListener{
public:
	void onTextMessage(const string& s, WebSocket* ws){
		ws->sendTextMessage(s);
		cout << "Recieved: " << s << endl;
	}

	void onClose(WebSocket* ws){
		delete ws;
	}

	bool CheckUserName(string EnteredUserName){
        if(EnteredUserName == username){
            return true;
        }
        return false;
	}
};

MyListener ml;

void runserver()
{
	try {
    // Make a socket to listen for client connections.
    TCPServerSocket servSock(8080);
	cout << "server running: " << servSock.getLocalAddress().getAddress() << endl;
    for (;;) {
		TCPSocket *sock = servSock.accept();
		WebSocket* ws = new WebSocket(sock);
		ws->setListener(&	ml);

    }
  } catch (SocketException &e) {
    cerr << e.what() << endl;           // Report errors to the console
  }
}

int main(int argc, char **argv)
{
	thread serverthread(runserver);
	serverthread.join();
}
