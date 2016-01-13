//**************************************
//! Container for the steps of a washing cycle, in the form of a bidirectionally
//! traversable list.
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __WASHING_CYCLE
#define __WASHING_CYCLE 

#include <vector>

class cycleStep;

class washingCycle
{
public:
	washingCycle(int length);
	cycleStep next();
	bool hasNext() const;
	void back();
	int currentStep() const;
	int totalSteps() const;
	void addStep(const cycleStep toAdd);

private:
	std::vector<cycleStep> steps;
	int current;
};

//**************************************
//! State of the washing machine, representing what the state of the machine
//! should look like at the end of the step, or as close as possible before the
//! step runs out of time.
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//! 	- part of TO6 assignment 2015-2016
//**************************************

class cycleStep
{
public:
	cycleStep(arguments);
	~cycleStep();

	//! true if this step is time-based, false if the step is water level, and
	//! -temperature based instead.
	bool isTimed() const;
	//! returns the duration of this step in ms, or 0 if the step is not time-
	//! based.
	int getDuration() const;
	//! returns true if detergent must be added during this step.
	bool addDetergent() const;
	//! returns true if the pump and tap must both be running during this step.
	bool mustFlush() const;
	//! returns the target water temperature for this step.
	unsigned short int getTemperature() const;



private:
	//no state for door lock; implied as closed until after the last step of 
	//the cycle.

	//! detergent will be added for entire duration of step if true, otherwise 
	//! no detergent will be added.
	bool addDetergent;
	//! if true, both the pump and the tap will run and the step *must* be 
	//! timed, ignoring any statement otherwise. 
	bool flush;
	//using unsigned short ints here, since none of these are allowed to be 
	//over 100 by the documentation.
	//! intended water temperature.
	unsigned short int temperature;
	//! intended water level;
	unsigned short int waterLevel;
	//! drum rotational speed, drum will turn counterclockwise if negative. Will
	//! be set to closest multiple of 25 during initialization, up to 1600 in 
	//! either direction.
	signed int drumSpeed;

	//! if true, the step ends when the set amount of time has passed, else the
	//! step ends when the water level and temperature have reached the expected
	//! values.
	bool timed;
	//! Duration of the step.
	int duration;
};

#endif

