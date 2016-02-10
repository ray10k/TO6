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
#include <string>
#include "cycleID.h"

class cycleStep;

class washingCycle
{
public:
	//! Creates an empty washing cycle.
	washingCycle (cycleID& ID);
	//! Advances the list to the next step, and returns the new current step.
	const cycleStep& next();
	//! returns the current step.
	const cycleStep& getCurrent() const;
	//! True unless either the current step is marked as a final step, or the
	//! current step is at the end of the list.
	bool hasNext() const;
	//! Moves one step back into the cycle.
	void back();
	//! Returns the current step number, ranging 0 - totalSteps().
	unsigned int currentStepNumber() const;
	//! returns the total number of steps in this cycle.
	unsigned int totalSteps() const;
	//! Adds a new step to the cycle, at the end. If there is already a final
	//! step added, calling this function will not add a new step.
	void addStep(cycleStep toAdd);
	//! returns the name of the cycle as a whole.
	const std::string& getName() const;
	//! returns the name of the cycle's owner.
	const std::string& getUser() const;
	//! Assignment operator.
	washingCycle& operator= (const washingCycle& other);
	//! comparison operator for loading.
	bool operator== (const cycleID& lhs) const;

private:
	cycleID myID;
	std::vector<cycleStep> steps;
	int current;
	const static cycleStep end;
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
	//! Creates a cycle step that ends as soon as the required temperature and
	//! water level are reached.
	cycleStep(const std::string& name,
		unsigned short int temp,
		unsigned short int water,
		bool detergent,
		signed int speed);
	//! Creates a cycle step that runs for a given amount of time before
	//! finishing.
	cycleStep(const std::string& name,
		unsigned short int temp,
		unsigned short int water,
		bool detergent,
		signed int speed,
		unsigned int duration,
		bool flush);
	//! Creates a finishing step that has no specific settings (returns false or
	//! 0 for all data access functions,) but will return true for isFinalStep()
	//! .
	cycleStep();

	//! true if this step is time-based, false if the step is water level, and
	//! -temperature based instead.
	bool isTimed() const;
	//! returns the duration of this step in seconds, or 0 if the step is not
	//! time-based.
	unsigned int getDuration() const;
	//! returns true if detergent must be added during this step.
	bool getAddDetergent() const;
	//! returns true if the pump and tap must both be running during this step.
	bool mustFlush() const;
	//! returns the target water temperature for this step.
	unsigned short int getTemperature() const;
	//! returns the target water level for this step.
	unsigned short int getWaterLevel() const;
	//! returns the speed of the drum as a multiplier of 25, without direction.
	unsigned short int getDrumSpeed() const;
	//! returns true if the drum should rotate clockwise, false otherwise.
	bool isDrumClockwise() const;
	//! returns true if this is the final step of a cycle. If so, the machine
	//! should be brought to a safe state to open it, and no further steps will
	//! be executed.
	bool isFinal() const;
	//! returns the name of this step, as it should be displayed.
	const std::string& getName() const;


private:
	//no state for door lock; implied as closed until after the last step of
	//the cycle.
	//! name of the step, for display purposes.
	std::string stepName;

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
	signed short int drumSpeed;

	//! if true, the step ends when the set amount of time has passed, else the
	//! step ends when the water level and temperature have reached the expected
	//! values.
	bool timed;
	//! Duration of the step in seconds.
	unsigned int duration;
	//! The last step in a cycle must have this set to true, any following steps
	//! are ignored and the door must be unlocked.
	bool finalStep;
};

#endif
