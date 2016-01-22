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
#include "prtos/pRTOS.h"
#include <string>
#include <vector>

//! A struct dat combines a request and his command.
struct RequestStruct
{
	int request;
	int command = NONE_CMD;
};

//! A struct that combines a request with its response 
//! and the read value of this response.
struct ResponseStruct
{
	RequestStruct request;
	std::string response;
	int value;
};

//! A enum containing all possible requests, 
//! this is used for switch statements
enum requestEnum
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
}

//! A enum containing all possible request commands, 
//! this is used for switch statements
enum commandEnum
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
}

public class machineInteractionTask : public RTOS::Task
{
public:
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
	main(void);
		
private:
	
	void notifyListeners();
	void update();
	void readChannel();
	
	void setDoorLock(bool lock);
	void getState(std::string request);
	void getWaterLevel();
	void setWaterValve(bool open);
	void setSoapDispenser(bool open);
	void setPump(bool on);
	void getTemperature();
	void setHeater(bool on);
	void getRPM();
	void setSignalLed(bool on);
	
	//! Translates a string request and a string command to one or two hex values and returns this in a vector.
	std::vector<std::uint8_t> requestTranslate(std::string request, std::string command);
	//! Translates one hex value to a understandable string response and returns this.
	//! (this function uses the request string to know where the response is comming from).
	std::string responseTranslate(std::vector<std::uint8_t> response, std::string request);

	uart Uart;
	std::vector<cycleStateListener&> listeners;
	MachineState currentState;
	MachineState setState;
	
	RTOS::clock::clock() clock;
	RTOS::channel<RequestStruct,16> setMachineStateChannel;
	
}