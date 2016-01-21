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

struct MachineState
{
	short unsigned int temperature;
	short unsigned int waterLevel;
	short unsigned int drumRPM;
	bool drumClockwise;
	bool doorLock;
	bool waterValve;
	bool soapDispenser;
	bool pump;
	bool heatingUnit;
	bool signalLed;
};

class machineStateListener
{
public:
	//! Called whenever the state of the washing machine is changed.
	virtual void stateChanged(MachineState currentState);
};

#endif
