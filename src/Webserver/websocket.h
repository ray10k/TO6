#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "PracticalSocket.h"
#include <thread>


using namespace std;

class WebSocketException : public std::runtime_error {
public:

  WebSocketException(const std::string &message) throw();
  WebSocketException(const std::string &message, const std::string &detail) throw();
};

class WebSocketListener;

/* Gemaakt door Jan Zuurbier, sept 2014
 * Alleen voor onderwijsdoelen.
 * Vrij om te gebruiken en aan te passen.
 */


class WebSocket
{
public:
	WebSocket(TCPSocket*);
	~WebSocket();
	void close();
	void setListener(WebSocketListener*);
	void sendTextMessage(const string &message) throw (WebSocketException, SocketException);
	string getForeignAddress() { return sock->getForeignAddress().getAddress(); }
    void processFrame() throw(WebSocketException, SocketException);
private:
	TCPSocket* sock;
	thread* thr;
	WebSocketListener* theListener = NULL;
	char* data;
	int datalen;
	bool closed;
	bool closing;
	void performHandshake() throw(WebSocketException, SocketException);

	void handleConnection() ;
	void sendClose(const char*, size_t) throw (SocketException);
	void sendPong(const char*, size_t) throw (SocketException);

/*
    string getFileName(string regel);
    bool getConnection(string regel);
    string getContentType(string &fileName);
    void readAndWriteResponse(TCPSocket *sock);
*/
};

class WebSocketListener
{
public:
    string username = "Admin";
	virtual void onTextMessage(const string& s, WebSocket* ws) = 0;
	virtual void onClose(WebSocket* ws) = 0;
};


#endif // WEBSOCKET_H
