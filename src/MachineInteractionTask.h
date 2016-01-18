//**************************************
//! Task responsible for communicating with the washing machine.
//! \authors
//! 	- Daniel Klomp
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __MACHINE_INTERACTION_TASK
#define __MACHINE_INTERACTION_TASK

#include "prtos/pRTOS.h"
#include <string.h>
#include <vector>

public class MachineInteractionTask : public RTOS::Task{
	MachineInteractionTask();
	~MachineInteractionTask();
	
	protected:
		Main(void);
		
	private:
	RTOS::clock::clock() clock;
	RTOS::channel<char*,16> setMachineStateChannel;
	
}