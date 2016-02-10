#include <iostream>
#include "PracticalSocket.h"
#include "websocket.h"
#include <thread>
#include <fstream>
// #include cyclestate.h

using namespace std;

class MyListener:public WebSocketListener{
public:
	void onTextMessage(const string& s, WebSocket* ws){

		if (s == "RUN"){
            //DT.setCycleState(RUN);
            cout<<"RUN"<<endl;
		}
		else if(s == "PAUZE"){
           // DT.setCycleState(PAUZE);
           cout<<"PAUSE"<<endl;
		}
		else if(s == "STOP"){
            //DT.setCycleState(STOP);
            cout<<"STOP"<<endl;
		}
		else{ws->sendTextMessage(s);}
		cout << "Recieved: " << s << endl;
	}

	void onClose(WebSocket* ws){
		delete ws;
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
        cout << "Trying to connect"<< endl;

        TCPSocket *sock = servSock.accept();

        WebSocket* ws = new WebSocket(sock);
		cout<<"connected"<< endl;
        ws->setListener(&	ml);

		cout<<"connected"<< endl;

    }
  } catch (SocketException &e) {
    cerr << e.what() << endl;           // Report errors to the console
  }
}

int main(int argc, char **argv)
{
    thread serverthread(runserver);
	serverthread.join();
 
  return 0;
}
