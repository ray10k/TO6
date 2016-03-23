#include "washingCycle.h"

washingCycle::washingCycle(cycleID& ID):
	myID(ID),
	steps(),
	current(0)
{}

const cycleStep washingCycle::last = cycleStep();

washingCycle::washingCycle():
	myID("DEFAULT","DEFAULT"),
	steps(),
	current(0)
{
	steps.push_back(last);
}

const cycleStep& washingCycle::next(){
	if (this->hasNext()) {
		++current;
		return steps[current];
	}else{
		return last;
	}
}

const cycleStep& washingCycle::getCurrent() const{
	return steps[current];
}

bool washingCycle::hasNext() const {
	return (current < (int)steps.size() && !(steps[current].isFinal()));
}

void washingCycle::back() {
	if (current != 0){
		--current;
	}
}

unsigned int washingCycle::currentStepNumber() const {
	return this->current;
}

unsigned int washingCycle::totalSteps() const {
	return this->steps.size();
}

void washingCycle::addStep(cycleStep toAdd){
	if (this->steps.empty())
	{
		this->steps.push_back(last);
	}
	
	std::vector<cycleStep>::iterator position = this->steps.rend();
	++position;
	this->steps.insert(position,toAdd);
	
}

const std::string& washingCycle::getName() const {
	return this->myID.name;
}

const std::string& washingCycle::getUser() const {
	return this->myID.user;
}

washingCycle& washingCycle::operator= (const washingCycle& other) {
	if (this != &other){
		//to copy: name, steps, current step.
		this->myID = other.myID;
		this->steps = other.steps;
		this->current = other.current;
	}
	return *this;
}

bool washingCycle::operator== (const cycleID& lhs) const{
	return this->myID == lhs;
}

cycleStep::cycleStep(const std::string& name,
	unsigned short int temp,
	unsigned short int water,
	bool detergent,
	signed int speed):
		stepName(name),
		addDetergent(detergent),
		flush(false),
		temperature(temp),
		waterLevel(water),
		drumSpeed(speed),
		timed(false),
		duration(0),
		finalStep(false)
	{}

cycleStep::cycleStep(const std::string& name,
	unsigned short int temp,
	unsigned short int water,
	bool detergent,
	signed int speed,
	unsigned int duration,
	bool flush):
		stepName(name),
		addDetergent(detergent),
		flush(flush),
		temperature(temp),
		waterLevel(water),
		drumSpeed(speed),
		timed(true),
		duration(duration),
		finalStep(false)
	{}

cycleStep::cycleStep():
		stepName("end"),
		addDetergent(false),
		flush(false),
		temperature(20),
		waterLevel(0),
		drumSpeed (0),
		timed(false),
		duration(0),
		finalStep(true)
	{}

bool cycleStep::isTimed() const {
	return this->timed;
}

unsigned int cycleStep::getDuration() const {
	return this->duration;
}

bool cycleStep::getAddDetergent() const {
	return this->addDetergent;
}

bool cycleStep::mustFlush() const {
	return this->flush;
}

unsigned short int cycleStep::getTemperature() const {
	return this->temperature;
}

unsigned short int cycleStep::getWaterLevel() const {
	return this->waterLevel;
}

unsigned short int cycleStep::getDrumSpeed() const {
	//speed is saved as rpm with the sign bit deciding direction,
	//this function must only return the speed as rpm/25, to match the format
	//expected by the device.
	unsigned int retval = this->drumSpeed;
	return retval;
}

bool cycleStep::isDrumClockwise() const {
	return this->drumSpeed > 0;
}

bool cycleStep::isFinal() const {
	return this->finalStep;
}

const std::string& cycleStep::getName() const {
	return this->stepName;
}
