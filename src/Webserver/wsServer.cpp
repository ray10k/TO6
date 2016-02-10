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




/*istream & getline(istream & in, string & out) {
	char c;
	out.clear();

	while(in.get(c).good()) {
		if(c == '\r') {
			c = in.peek();
			if(in.good() && c == '\n' ) {
				in.ignore();
			}
			break;
		}
		if(c == '\n') break;
		out.append(1,c);
	}
	return in;
}

	void onClose(WebSocket* ws){
		delete ws;
	}

//=====================================================================================================================
// Eigen TOEVOEGINGEN!
//=====================================================================================================================


string getFileName(string regel){
	string temp;
	if(regel.substr(0,4) == "GET ")
	{
		cout << "fileName: " << regel.substr(4, (regel.size() - 13))  << endl;
		return regel.substr(4, (regel.size() - 13));
	}
	return "";
}

int sendResponse(iostream &stream, string & fileName, string & contentType, TCPSocket * sock){
	ifstream input("www" + fileName, ifstream::ate | ios::binary);
	char *buffer = new char[1024];
	if (!input.is_open() ){
		cout << "input not open: " << fileName << endl;
		return 1;
	}else{
		stream << "HTTP/1.1 200 OK" << endl;
		stream << "Content-Length: " << input.tellg() << endl;
		stream << "Content-Type: " << contentType << endl;
		stream << endl;
		stream.flush();
		input.seekg(0, ios::beg);
		while (!input.eof()){
			input.read(buffer, 1024);
        	int count = input.gcount();
        	sock->send(buffer, count);
		}
	}
	cout << "close input " << fileName << endl;
	delete[] buffer;
	input.close();
	return 0;
}

void handleConnection(TCPSocket *sock, iostream& stream){
	string file_name, contentType, regel, temp;
	while(getline(stream, regel)){
		cout << regel << endl;
		temp = getFileName(regel);
		if (temp != ""){
			cout << "!= NULL"  << endl;
			file_name = temp;
			if(file_name == "/"){
				cout << "index"  << endl;
				file_name = "/index.html";
			}
			string Type = file_name.substr(file_name.find_last_of(".") + 1);
			if(Type == "png") {
				contentType = "image/png";
  			} else if(Type == "jpg") {
				contentType = "image/jpeg";
  			} else if(Type == "html") {
				contentType = "text/html";
  			}else if(Type == "css") {
				contentType = "text/css";
  			}else if(Type == "js") {
				contentType = "application/javascript";
  			}
		}
		if(regel.empty() ) {
			cout << "empty regel"  << endl;
			if(sendResponse(stream, file_name, contentType, sock)!= 0){
				cout << "404" << endl;
				stream << "HTTP/1.1 404 Not Found" << endl;
				stream << endl;
				stream.flush();
			}
			cout << "sendResponse done"  << endl;
		}
	}
}
*/

//=========================================================================================


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
 /*try {

    // Make a socket to listen for client connections.
    TCPServerSocket servSock(8080);
	cout << "server running: " << servSock.getLocalAddress().getAddress() << endl;
    for (;;) {                          // Repeatedly accept connections
		TCPSocket *sock = servSock.accept();

        WebSocket* ws = new WebSocket(sock);
        ws->setListener(&	ml);

        iostream& stream = sock->getStream();

        while(true){
           ws->handlePerform();
           handleConnection(sock, stream);
           ws->processFrame();
       }

		cout << "Socket accept" << endl;
		}

  } catch (SocketException &e) {
    cerr << e.what() << endl;           // Report errors to the console
  }
    */
  return 0;
}
