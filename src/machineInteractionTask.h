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
enum class requestEnum
{
	MACHINE_REQ,
	DOOR_LOCK_REQ,
	WATER_VALVE_REQ,
	SOAP_DISPENSER_REQ,
	PUMP_REQ,
	WATER_LEVEL_REQ,
	HEATING_UNIT_REQ,
	TEMPERATURE_REQ,
	SET_RPM_REQ,
	GET_RPM_REQ,
	SIGNAL_LED_REQ
};

//! A enum containing all possible request commands,
//! this is used for switch statements
enum class commandEnum
{
	NONE_CMD,
	STATUS_CMD,
	LOCK_CMD,
	UNLOCK_CMD,
	START_CMD,
	STOP_CMD,
	OPEN_CMD,
	CLOSE_CMD,
	ON_CMD,
	OFF_CMD,
	RPM_Clockwise,
	RPM_counterClockwise
};

//! A struct that combines a request and his command. See requestEnum and commandEnum
//! for all possible request and commands. This struct is used for sending requests through the uart.
struct RequestStruct
{
	requestEnum request;
	commandEnum command = commandEnum::NONE_CMD;
	RequestStruct operator=(const RequestStruct& rhs){
        this->request = rhs.request;
        this->command = rhs.command;
        return *this;
	}
};

//! A struct that combines a request with its response
//! and the read value of this response.
struct ResponseStruct
{
	RequestStruct request;
	std::string response;
	int value;
	ResponseStruct operator= (const ResponseStruct& rhs){
        this->request = rhs.request;
        this->response = rhs.response;
        this->value = rhs.value;
        return *this;
	}
};

class machineInteractionTask : public RTOS::task
{
public:
	//! Constructor.
	machineInteractionTask();
	//! Register a new listener that wants to get
	//! the current state of the washing machine when updated.
	void addMachineStateListener(machineStateListener& listener);

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
	//! Starts or stops the entire washing machine.
	void setMachineState(bool start);

protected:
	//because the Task interface demands it, and because this task needs to do
	//things.
	void main(void);

private:
	//! Updates all the registered machineStateListeners,
	//! sending the current state of the washing machine.
	void notifyListeners();
	//! This function is used to check if the current state of the washing machine
	//! needs to be changed according to the setState.
	//! This also updates the current temperature and waterLevel.
	void update();
	//! This function reads the machineInstructionPool and executes the read request
	//! through the uart, after this the response will be read and returned in a ResponseStruct.
	ResponseStruct readPool();

	//! Used to lock or unlock the washing machine door.
	void setDoorLock(bool lock);
	//! Adds to the machineInstructionPool a request to get the state of a given
	//! request/part of the washing machine, when the pool is read this requested state
	//! will be returned and saved as the current state of the washing machine.
	void getState(requestEnum request);
	//! Adds to the machineInstructionPool a request to get the waterLevel, when the
	//! pool is read this value will be returned and saved as the current waterLevel.
	void getWaterLevel();
	//! Used to open or close the water valve.
	void setWaterValve(bool open);
	//! Used to open or close the soap dispenser.
	void setSoapDispenser(bool open);
	//! Used to turn the pump on or off.
	void setPump(bool on);
	//! Adds to the machineInstructionPool a request to get the temperature, when the
	//! pool is read this value will be returned and saved as the current temperature.
	void getTemperature();
	//! Used to turn the heater on or off.
	void setHeater(bool on);
	//! Adds to the machineInstructionPool a request to get the RPM speed, when the
	//! pool is read this value will be returned and saved as the current RPM.
	//! This also returns and saves if the rotation is clockwise or counterclockwise.
	void getRPM();
	//! Used to turn the signal led on or off.
	void setSignalLed(bool on);

	//! Translates a string request and a string command to one or two hex values and returns this in a vector.
	std::uint16_t requestTranslate(RequestStruct reqS);
	//! Translates one hex value to a understandable string response and returns this.
	//! (this function uses the request string to know where the response is comming from).
	ResponseStruct responseTranslate(std::uint8_t response, RequestStruct reqS);

	RTOS::pool<RequestStruct> machineInstructionPool;
	RTOS::clock clock;
	
	MachineState currentState;
	MachineState setState;
	uart Uart;

	std::vector<machineStateListener> listeners;
};

#endif
