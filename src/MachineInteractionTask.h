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

public class MachineInteractionTask : public RTOS::Task{
	MachineInteractionTask();
	void addMachineStateListener(machineStateListener& listener);
	
	void setTemperature(unsigned int temperature);
	void setWaterLevel(unsigned int waterLevel);
	void setRPM(bool clockwise, unsigned int rpm);
	void setMachineState(bool start);
	
	protected:
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
	RTOS::channel<char*,16> setMachineStateChannel; // RTOS::channel<RequestStruct, ...>¿
	
}