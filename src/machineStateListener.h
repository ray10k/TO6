//**************************************
//! Interface for classes that need information on the state of the washing machine.
//!
//! \authors
//! 	- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __MACHINE_STATE_LISTENER
#define __MACHINE_STATE_LISTENER

struct RequestStruct
{
	std::string request;
	std::string command = "";
}

struct ResponseStruct
{
	RequestStruct request;
	std::string response;
	int value;
}

struct MachineState
{
	int temperature;
	int waterLevel;
	int drumRPM;
}

class machineStateListener
{
public:
	//! Called whenever the state of the washing machine is changed.
	virtual void stateChanged(MachineState currentState);
	virtual void response(ResponseStruct response);
};

#endif
