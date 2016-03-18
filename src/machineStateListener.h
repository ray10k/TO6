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

//! A struct that can contain all possible information of the washing machine.
struct MachineState
{
	short unsigned int temperature;
	short unsigned int waterLevel;
	short unsigned int drumRPM;
	bool soapDispenser;
	bool drumClockwise;
	bool doorLock;
	bool waterValve;
	bool pump;
	bool heatingUnit;
	bool signalLed;
	
	bool operator != (const MachineState& other) const
	{
		return !(this->temperature==other.temperature
			&& this->waterLevel == other.waterLevel
			&& this->drumRPM == other.drumRPM
			&& this->soapDispenser == other.soapDispenser
			&& this->drumClockwise == other.drumClockwise
			&& this->doorLock == other.doorLock
			&& this->waterValve == other.waterValve
			&& this->pump == other.pump
			&& this->heatingUnit == other.heatingUnit
			&& this->signalLed == other.signalLed);
	}
};

class machineStateListener
{
public:
	//! A function called whenever the state of the washing machine is changed.
	virtual void stateChanged(const MachineState& currentState){};
};

#endif
