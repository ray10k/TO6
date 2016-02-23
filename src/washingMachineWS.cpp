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
	this->myParent->command(s);
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
	queuedCommands(std::string("")),
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

void appendBool(std::stringstream & in, const std::string & name, bool value){
	in << name << ":";
	value ? in<<"true," : in<<"false,"; 
}

void washingMachineWS::passAlongCommands(){
	//Periodically check to see if any new commands came in, and let the system
	//know what it should do.
	std::chrono::milliseconds sleepTime(250);
	while (1==1){
		std::this_thread::sleep_for(sleepTime);
		std::string latestCommand = this->queuedCommands.getNext();
		//parse here, once formatting is known.
		//handle the parsed command.
	
		if (this->machineUpdated){
			this->machineUpdated = false;
			std::stringstream builder;
			builder << "{temperature:"<<this->latest.temperature<<",water:"
				<<this->latest.waterLevel<<",drum:"<<this->latest.drumRPM;
			builder << ",";
			appendBool(builder,"soap",this->latest.soapDispenser);
			appendBool(builder,"clockwise",this->latest.drumClockwise);
			appendBool(builder,"lock",this->latest.doorLock);
			appendBool(builder,"water",this->latest.waterValve);
			appendBool(builder,"pump",this->latest.pump);
			appendBool(builder,"heater",this->latest.heatingUnit);
			builder << "signal:";
			this->latest.signalLed ? builder << "true}" : builder << "false}";
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
	} else if (command.compare("pause") == 0){
		this->buddy->setCycleState(cycleState::PAUSE);
	} else if (command.compare("resume") == 0){
		this->buddy->setCycleState(cycleState::RUN);
	} else if (command.compare("start") == 0){
		unsigned int spacer = rawInput.find_last_of(' ');
		
		std::string userName(rawInput,commandLength+2,(spacer-commandLength));
		std::string cycleName(rawInput,spacer,std::string::npos);
		this->buddy->loadCycle(userName,cycleName);
	}
	
} 

void washingMachineWS::disposeConnection(socketConnection * toClose){
	//connection was closed; time to clean up.
	delete toClose;
}

washingMachineWS * washingMachineWS::newWebSocket(int portNr, 
		userInteractionTask* myBuddy){
	washingMachineWS  *washmachine=new washingMachineWS(portNr,myBuddy);
	std::thread listener(&washingMachineWS::acceptConnections, washmachine);
	std::thread commands(&washingMachineWS::passAlongCommands, washmachine);
	return washmachine;
}

