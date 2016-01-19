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

struct ResponseStruct
{
	std::string response;
	std::string request;
	int value;
}

class machineStateListener
{
	/*
public:
	//! Called whenever the next step in a cycle begins execution.
	virtual void cycleStateChanged(unsigned int totalSteps,
		unsigned int currentStep,
		const string& cycleName,
		const string& stepName)=0;
	//! Called when the cycle is temporarily suspended. Upon resuming execution,
	//! cycleStateChanged will be called.
	virtual void cyclePaused(const string& cycleName,
		const string& stepName)=0;
	//! Called when the cycle ends. If the cycle ended by reaching the final
	//! step, finished will be true. If the cycle got terminated before the end,
	//! finished will be false.
	virtual void cycleEnded(bool finished,
		const string& cycleName,
		const string& stepName)=0;
		*/
};

#endif
