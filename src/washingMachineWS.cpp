#include "washingMachineWS.h"

socketConnection::socketConnection(WebSocket* thisIsMe, 
	washingMachineWS * parent):
	currentState(),
	updated(false),
	myParent(parent),
	connection(thisIsMe){
		connection->setListener(this);
}

socketConnection::~socketConnection(){
	delete this->connection;
}

socketConnection::socketConnection(const socketConnection& other):
	currentState(other.currentState),
	updated(true),
	myParent(other.myParent){
	//doodle
}

void socketConnection::machineUpdateHappened(const std::string& current){
	//inform connected client. (got to figure out the comm. protocol here)
	this->connection->sendTextMessage(current);
}

void socketConnection::onTextMessage(const std::string& s, WebSocket* ws){
	//user wants something now.
	this->parent->command(s);
}

void socketConnection::onClose(WebSocket* ws){
	//time to die.
	this->myParent->disposeConnection(this);
}

std::string socketConnection::getAddress(){
	return connection->getForeignAddress();
}

washingMachineWS::washingMachineWS(int portNr, userInteractionTask * talkTo):
	machineUpdated(false),
	buddy(talkTo),
	queuedCommands(""),
	liveConnections()
	{}

void washingMachineWS::acceptConnections(){
	//wait for a connection to come in, and make an object to handle 
	//communication between the server and client.
	TCPServerSocket servSock(25565);
	
	while (1==1){
		try{
			TCPSocket * sock = servSock.accept();
			WebSocket * wSock = new WebSocket(sock);
			socketConnection * actual = new socketConnection(wSock,this);
			this->liveConnections.insert(actual);
		} catch(SocketException &e) {
			cerr << e.what() << endl;
		}
	}
}

void washingMachineWS::passAlongCommands(){
	//Periodically check to see if any new commands came in, and let the system
	//know what it should do.
	std::chrono::milliseconds sleepTime(250);
	while (1==1){
		std::this_thread::sleep_for(sleepTime);
		std::string latest = this->queuedCommands.getNext();
		//parse here, once formatting is known.
		//handle the parsed command.
		//check the current machine state, and let everyone know.
		
	//let everyone know the new state of things.
	std::stringstream builder;
	builder << "{temperature:"<<current.temperature<<",water:"
		<<current.waterLevel<<",drum:"<<current.drumRPM;
	builder << ",";
	appendBool(builder,"soap",current.soapDispenser);
	appendBool(builder,"clockwise",current.drumClockwise);
	appendBool(builder,"lock",current.doorLock);
	appendBool(builder,"water",current.waterValve);
	appendBool(builder,"pump",current.pump);
	appendBool(builder,"heater",current.heatingUnit);
	builder << "signal:";
	current.signalLed ? builder << "true}" : builder << "false}";
	std::string message = builder.str();
	
	std::set<socketConnection*>::iterator 
			start = this->liveConnections.begin();
	std::set<socketConnection*>::iterator 
			end = this->liveConnections.end();
	for (;start != end; ++start){
		(*start)->machineUpdateHappened(message);
	}
}
	}
}

void appendBool(std::stringstream & in, const std::string & name, bool value){
	in << name << ":";
	value ? in<<"true," : in<<"false,"; 
}

void washingMachineWS::updateMachineState(MachineState current){
	this->machineUpdated = true;
	this->latest  = current;
}

void washingMachineWS::command(const std::string& rawInput){
	//command and operands are space-separated.
	unsigned int commandLength = rawInput.find_first_of(' ');
	std::string command(rawInput,0,commandLength);
	
	if (command.compare("stop") == 0){
		this->buddy->setCycleState(cycleState::STOP);
	} else if (command.compare("start") == 0){
		//doodle
	}
	
}

void washingMachineWS::disposeConnection(socketConnection * toClose){
	//connection was closed; time to clean up.
	delete toClose;
}

washingMachineWS * newWebSocket(int portNr, userInteractionTask* myBuddy){
	washingMachineWS  *washmachine=new washingMachineWS(portNr,myBuddy);
	std::thread listener(&washingMachineWS::acceptConnections, washmachine);
	std::thread commands(&washingMachineWS::passAlongCommands, washmachine);
	return washmachine;
}
