//**************************************
//! Task responsible for communicating with the washing machine.
//! \authors
//! 	- Daniel Klomp
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __MACHINE_INTERACTION_TASK
#define __MACHINE_INTERACTION_TASK

#include "uart.h"
#include "machineStateListener.h"
#include "cycleStateListener.h"
#include "prtos/pRTOS.h"
#include <string>
#include <vector>

//! A enum containing all possible requests,
//! this is used for switch statements
enum class requestEnum : std::uint8_t
{
	MACHINE_REQ=0x01,
	DOOR_LOCK_REQ=0x02,
	WATER_VALVE_REQ=0x03,
	SOAP_DISPENSER_REQ=0x04,
	PUMP_REQ=0x05,
	WATER_LEVEL_REQ=0x06,
	HEATING_UNIT_REQ=0x07,
	TEMPERATURE_REQ=0x08,
	SET_RPM_REQ=0x0A,
	GET_RPM_REQ=0x09,
	SIGNAL_LED_REQ=0x0B,
	NONE_REQ=0xFF
};

//! Enum describing the possible replies from the washing machine, following
//! a request.
enum class replyEnum : std::uint8_t
{
	MACHINE_REP=0x81,
	DOOR_LOCK_REP=0x82,
	WATER_VALVE_REP=0x83,
	SOAP_DISPENSER_REP=0x84,
	PUMP_REP=0x85,
	WATER_LEVEL_REP=0x86,
	HEATING_UNIT_REP=0x87,
	TEMPERATURE_REP=0x88,
	SET_RPM_REP=0x8A,
	GET_RPM_REP=0x89,
	SIGNAL_LED_REP=0x8B,
	ERROR_REP=0xFF
};

//! Enum for the fixed-value responses from the washing machine.
enum class stateEnum : std::uint8_t
{
	IMPOSSIBLE = 0x00,
	HALTED = 0x01,
	IDLE = 0x02,
	RUNNING = 0x04,
	STOPPED = 0x08,
	OPENED = 0x01,
	CLOSED = 0x02,
	LOCKED = 0x04,
	ON = 0x08,
	OFF = 0x10,
	ERROR = 0xFF
};

//! A enum containing all possible request commands,
//! this is used for switch statements
enum class commandEnum : std::uint8_t
{
	NONE_CMD=0xFF,
	STATUS_CMD=0x01,
	LOCK_CMD=0x40,
	UNLOCK_CMD=0x80,
	START_CMD=0x10,
	STOP_CMD=0x20,
	OPEN_CMD=0x10,
	CLOSE_CMD=0x20,
	ON_CMD=0x10,
	OFF_CMD=0x20,
	RPM_Clockwise=0x00,
	RPM_counterClockwise=0x80
};

struct MessageStruct
{
	std::uint8_t message = 0;
	std::uint8_t operand = 0;
	MessageStruct operator =(const MessageStruct& rhs){
		this->message = rhs.message;
		this->operand = rhs.operand;
		return *this;
	}
	
	MessageStruct operator =(std::uint16_t rhs){
		this->message = (rhs&0xff);
		this->operand = ((rhs >> 8) &0xff);
		return *this;
	}
	
	operator std::uint16_t() const {
		std::uint16_t retval = this->operand;
		retval = (retval << 8)|this->message;
		return retval;
	}
};

class machineInteractionTask : public RTOS::task
{
public:
	//! Constructor.
	machineInteractionTask();
	//! Register a new listener that wants to get
	//! the current state of the washing machine when updated.
	void addMachineStateListener(machineStateListener* listener);

	//! Sets the wanted temperature to a given value.
	//! (wanted temperature = the value in degrees celcius to which
	//! the temperature will be set by this machineInteractionTask)
	void setTemperature(unsigned int temperature);
	//! Sets the wanted waterLevel to a given value.
	//! (wanted waterLevel = the value in percentage to which
	//! the waterLevel will be set by this machineInteractionTask)
	void setWaterLevel(unsigned int waterLevel);
	//! Sets the speed of the drum to a given value, also sets the rotation
	//! to clockwise or counterclockwise with the given boolean clockwise.
	void setRPM(bool clockwise, unsigned int rpm);
	//! Sets if the detergent should be added.
	void setDetergent(bool add);
	//! Makes the washing machine flush by activating the pump,
	//! this also sets the temperature to a low stand by temperature.
	void flush();

protected:
	//because the Task interface demands it, and because this task needs to do
	//things.
	void main(void);

private:
	//! Updates all the registered machineStateListeners,
	//! sending the current state of the washing machine.
	void notifyListeners();
	//! This function is used to check if the current state of the washing
	//! machine needs to be changed according to the setState.
	//! This also updates the current state in accordance to the replies 
	//! received.
	void update();
	//! Apply the state information in the given response to the internally kept
	//! current state.
	void parseResponse(MessageStruct response);
	
	//! Prepare a message to request the given status.
	MessageStruct getState(requestEnum request);
	
	RTOS::flag machineRequestFlag;
	RTOS::clock clock;
	
	MachineState currentState;
	MachineState targetState;
	uart Uart;

	std::vector<machineStateListener*> listeners;
	bool running;
};

#endif
