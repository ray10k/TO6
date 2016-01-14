#include "washingCycle.h"

washingCycle::washingCycle():
	steps(),
	current(0)
{}

const cycleStep& washingCycle::next(){
	
	if (this->hasNext()) {
		++current;
		return steps[current];
	}else{
		return end;
	}
}

bool washingCycle::hasNext() const {
	return (current < steps.size() && !(steps[current].isLast()));
}

void washingCycle::back() {
	if (current != 0){
		--current;
	}
}

unsigned int washingCycle::currentStep() const {
	return this->current;
}

unsigned int washingCycle::totalSteps() const {
	return this->steps.size();
}


cycleStep::cycleStep(unsigned short int temp,
	unsigned short int water,
	bool detergent,
	signed int speed):
		addDetergent(detergent),
		flush(false),
		temperature(temp),
		waterLevel(water),
		drumSpeed(speed),
		timed(false),
		duration(0),
		finalStep(false)
	{}

cycleStep::cycleStep(unsigned short int temp,
	unsigned short int water,
	bool detergent,
	signed int speed,
	unsigned int duration,
	bool flush):
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

bool cycleStep::addDetergent() const {
	return this->addDetergent;
}

bool cycleStep::mustFlush() const {
	return this->flush;
}

unsigned short int cycleStep::getTemperature() const {
	return this->temperature;
}

unsigned short int cycleStep::getDrumSpeed() const {
	//speed is saved as rpm with the sign bit deciding direction,
	//this function must only return the speed as rpm/25, to match the format
	//expected by the device.
	unsigned int retval = this->drumSpeed; 
	return retval / 25;
}

bool cycleStep::isDrumClockwise() const {
	return this->drumSpeed > 0;
}

bool cycleStep::isFinal() const {
	return this->finalStep;
}
