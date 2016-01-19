//**************************************
//! Interface for classes that need information on the progression of a cycle.
//!
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __CYCLE_STATE_LISTENER
#define __CYCLE_STATE_LISTENER

class cycleStateListener
{
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
};

#endif
