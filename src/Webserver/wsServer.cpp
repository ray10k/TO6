#include <iostream>
#include "PracticalSocket.h"
#include "websocket.h"
#include <thread>


using namespace std;



class MyListener:public WebSocketListener{
public:
	void onTextMessage(const string& s, WebSocket* ws){
		if(s == "RUN"){
            cout<<"it wordks"<<endl;
		}
        else{
            ws->sendTextMessage(s);
            cout << "Recieved: " << s << endl;
		}
	}

	void onClose(WebSocket* ws){
		delete ws;
	}

	bool CheckUserName(string EnteredUserName){

        cout<< "TeST" <<endl;
        if(EnteredUserName == username){
            cout<< EnteredUserName <<endl;
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
        cout << "Trying to connect"<< endl;
		TCPSocket *sock = servSock.accept();
		cout<<"connected"<< endl;
		WebSocket* ws = new WebSocket(sock);
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
}
