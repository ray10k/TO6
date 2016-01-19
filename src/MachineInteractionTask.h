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
	~MachineInteractionTask();
	
	protected:
		Main(void);
		
	private:
	
	//! Translates a string request and a string command to one or two hex values and returns this in a vector.
	std::vector<std::uint8_t> requestTranslate(std::string request, std::string command);
	//! Translates one hex value to a understandable string response and returns this.
	//! (this function uses the request string to know where the response is comming from).
	std::string responseTranslate(std::vector<std::uint8_t> response, std::string request);
	
	//TO DO::
	uart Uart;
	
	RTOS::clock::clock() clock;
	RTOS::channel<char*,16> setMachineStateChannel;
	
}