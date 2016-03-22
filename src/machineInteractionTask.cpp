#include "machineInteractionTask.h"
#include <iostream>

const int MAX_DRUM_RPM = 1600;

machineInteractionTask::machineInteractionTask():
	RTOS::task(5,"Machine Interaction Task"),
	machineRequestFlag(this,"Machine Request Flag"),
	clock(this,500 MS,"Machine update clock"),
	currentState(),
	targetState(),
	previousState(),
	Uart(),
	listeners(),
	running(false)
{
	//during startup, send the washing machine the instruction to get into
	//its running state. Prevents trouble later down the line.
	MessageStruct startup;
	startup = requestEnum::MACHINE_REQ;
	startup = commandEnum::START_CMD;
	Uart.write(startup);
	MessageStruct reply; //living on a prayer, hoping the WM doesnt need long.
	reply = Uart.read_16();
	this->parseResponse(reply);
	
	this->currentState.runState = MachineRunState::IDLE;
}

void machineInteractionTask::addMachineStateListener
						(machineStateListener* listener)
{
	this->listeners.push_back(listener);
}

void machineInteractionTask::setTemperature(unsigned int temperature)
{
#ifdef DEBUG
	std::cout << "temperature set to " << std::dec << temperature << std::endl;
#endif
	if (temperature <= 100)
	{
		this->targetState.temperature = temperature;
	}
	else
	{
		this->targetState.temperature = 100;
	}
	this->machineRequestFlag.set();
}

void machineInteractionTask::setWaterLevel(unsigned int waterLevel)
{
#ifdef DEBUG
	std::cout << "water level set to " << std::dec << waterLevel << std::endl;
#endif
	if (waterLevel <= 100)
	{
		this->targetState.waterLevel = waterLevel;
	}
	else
	{
		this->targetState.waterLevel = waterLevel;
	}
	this->machineRequestFlag.set();
}

void machineInteractionTask::setRPM(bool clockwise, unsigned int rpm)
{
#ifdef DEBUG
	std::cout << "RPM set to " << std::dec << rpm << 
	' ' << (clockwise?"clockwise":"counterclockwise") << std::endl;
#endif
	if (rpm <= MAX_DRUM_RPM)
	{
		//the machine handles rpm in steps of 25.
		this->targetState.drumRPM = (rpm/25); 
	}
	else
	{
		//the machine RPM caps out at 0x40 * 0x19 = 0x640 = 1600 rpm.
		this->targetState.drumRPM = 0x40;
	}
	this->targetState.drumClockwise = clockwise;
	this->machineRequestFlag.set();
}

void machineInteractionTask::setDetergent(bool add)
{
#ifdef DEBUG
	std::cout << "Detergent turned " << 
	(add?"on":"off") << std::endl;
#endif
	this->targetState.soapDispenser = add;
	this->machineRequestFlag.set();
}

void machineInteractionTask::flush()
{
#ifdef DEBUG
	std::cout << "instructed to flush" << std::endl;
#endif
	this->targetState.waterLevel = 200; //Out-of-bounds to indicate the actual
	//level should be ignored during status update.
	this->machineRequestFlag.set();
}

void machineInteractionTask::setMachineState(bool run)
{
#ifdef DEBUG
	std::cout << "running state set to " <<
	(run?"running":"stopped") << std::endl;
#endif
	this->running = run;
	this->machineRequestFlag.set();
}

void machineInteractionTask::notifyListeners()
{
	trace;
	MachineState toSend(this->currentState);
	
	if (toSend != this->previousState)
	{
		std::vector<machineStateListener*>::iterator listen;
	
		for (listen = this->listeners.begin();
			listen != this->listeners.end();
			++listen)
		{
			(*listen)->stateChanged(toSend);
		}
	}
	
	this->previousState = this->currentState;
}

void machineInteractionTask::update()
{
#ifdef DEBUG
	std::cout << "machine state prior to update:" << std::endl << std::dec
	<< "water: " << this->currentState.waterLevel << " temperature: "
	<< this->currentState.temperature << " drum speed: " 
	<< this->currentState.drumRPM*25 << std::endl
	<< "target state (summarized:)" << std::endl<<"water: " <<
	this->targetState.waterLevel << " temperature: " << targetState.temperature
	<< " drum speed: " <<targetState.drumRPM*25 << std::endl;
	
	std::cout << "current runState: ";
	switch (this->currentState.runState)
	{
		case MachineRunState::IDLE:
			std::cout << "IDLE";
			break;
		case MachineRunState::RUNNING:
			std::cout << "RUNNING";
			break;
		case MachineRunState::HALTED:
			std::cout << "HALTED";
			break;
		case MachineRunState::STOPPED:
			std::cout << "STOPPED";
			break;
		case MachineRunState::FAILED:
			std::cout << "FAILED";
			break;
	}
	std::cout << std::endl;
#endif
	if (this->currentState.runState != MachineRunState::RUNNING)
	{
		trace;
		//only in running state is it possible to update the state of the 
		//machine, in any other state most control statements are ignored.
		//no point in wasting time here.
		return;
	}
	
	if (!this->running)
	{
		//No program ongoing at this point in time.
		if (!this->inSafeState())
		{
			//move to safe state: no water, no movement, no heat, etc.
			if (!this->currentState.doorLock)
			{
				MessageStruct door;
				door = requestEnum::DOOR_LOCK_REQ;
				door = commandEnum::LOCK_CMD;
				this->send(door);
			}
			if (!this->currentState.pump)
			{
				MessageStruct pump;
				pump = requestEnum::PUMP_REQ;
				pump = commandEnum::ON_CMD;
				this->send(pump);
			}
			if (this->currentState.heatingUnit)
			{
				MessageStruct heat;
				heat = requestEnum::HEATING_UNIT_REQ;
				heat = commandEnum::OFF_CMD;
				this->send(heat);
			}
			if (this->currentState.waterValve)
			{
				MessageStruct valve;
				valve = requestEnum::WATER_VALVE_REQ;
				valve = commandEnum::CLOSE_CMD;
				this->send(valve);
			}
			if (this->currentState.drumRPM != 0)
			{
				MessageStruct drum;
				drum = requestEnum::SET_RPM_REQ;
				drum.operand = 0;
				this->send(drum);
			}
		}
		else
		{
			if (this->currentState.pump)
			{
				MessageStruct pump;
				pump = requestEnum::PUMP_REQ;
				pump = commandEnum::OFF_CMD;
				this->send(pump);
			}
			if (this->currentState.doorLock)
			{
				MessageStruct door;
				door = requestEnum::DOOR_LOCK_REQ;
				door = commandEnum::UNLOCK_CMD;
				this->send(door);
			}
		}
	}
	else if (!this->currentState.doorLock)
	{
		MessageStruct door;
		door = requestEnum::DOOR_LOCK_REQ;
		door = commandEnum::LOCK_CMD;
		this->send(door);
		return;
	}

	if (this->targetState.doorLock != this->currentState.doorLock)
	{
		MessageStruct door;
		door = requestEnum::DOOR_LOCK_REQ;
		if (this->targetState.doorLock)
		{
			door = commandEnum::LOCK_CMD;
		}
		else
		{
			door = commandEnum::UNLOCK_CMD;
		}
		this->send(door);
	}

	MessageStruct heater;
	heater = requestEnum::HEATING_UNIT_REQ;
	if (this->currentState.temperature < this->targetState.temperature &&
		! this->currentState.heatingUnit)
	{
		heater = commandEnum::ON_CMD;
		this->send(heater);
	}
	else if (this->currentState.temperature >= this->targetState.temperature &&
		this->currentState.heatingUnit)
	{
		heater = commandEnum::OFF_CMD;
		this->send(heater);
	}
	
	
	
	MessageStruct pump,valve;
	pump = requestEnum::PUMP_REQ;
	valve= requestEnum::WATER_VALVE_REQ;
	
	if (this->targetState.waterLevel > 100 && 
		! (this->currentState.pump && this->currentState.waterValve) )
	{
		//flush by running the water *and* opening the valve.
		pump = commandEnum::OPEN_CMD;
		valve= commandEnum::OPEN_CMD;
		this->send(pump);
		this->send(valve);
	}
	else
	{
		//there is a target water level. Assess diff. between current and target
		//, apply tolerances.
		
		int diff = this->currentState.waterLevel - this->targetState.waterLevel;
		
		
		if (diff-2 < 0) //water is too low, need more. (maybe.)
		{
			if (!this->currentState.waterValve)
			{
				valve = commandEnum::OPEN_CMD;
				this->send(valve);
			}
			
			if (this->currentState.pump)
			{
				pump = commandEnum::OFF_CMD;
				this->send(pump);
			}
		}
		else if (diff+2 > 0) //water is too high, need less (maybe)
		{
			if (this->currentState.waterValve)
			{
				valve= commandEnum::CLOSE_CMD;
				this->send(valve);
			}
			
			if (!this->currentState.pump)
			{
				pump = commandEnum::ON_CMD;
				this->send(pump);
			}
		}
	}
	
	if (this->currentState.soapDispenser != this->targetState.soapDispenser)
	{
		MessageStruct soap;
		soap.message = (std::uint8_t)requestEnum::SOAP_DISPENSER_REQ;
		if (this->targetState.soapDispenser)
		{
			soap.operand = (std::uint8_t)commandEnum::OPEN_CMD;
		}
		else
		{
			soap.operand = (std::uint8_t)commandEnum::CLOSE_CMD;
		}
		this->send(soap);
	}
	
	if (this->currentState.signalLed != this->targetState.signalLed)
	{
		MessageStruct led;
		led.message = (std::uint8_t)requestEnum::SIGNAL_LED_REQ;
		if(this->targetState.signalLed)
		{
			led.operand = (std::uint8_t)commandEnum::ON_CMD;
		}
		else
		{
			led.operand = (std::uint8_t)commandEnum::OFF_CMD;
		}
		this->send(led);
	}
	
	if (this->currentState.drumRPM != this->targetState.drumRPM ||
		this->currentState.drumClockwise != this->currentState.drumClockwise)
	{
		MessageStruct drum;
		drum = requestEnum::SET_RPM_REQ;
		drum.operand = (std::uint8_t)this->targetState.drumRPM;
		if (! this->targetState.drumClockwise)
		{
			drum.operand |= (std::uint8_t)commandEnum::RPM_Clockwise;
		}
		this->send(drum);
	}
	
	//all messages prepared, time to write them to the washing machine.

#ifdef DEBUG	
	std::cout << std::endl;
#endif
}

void machineInteractionTask::poll()
{
	MessageStruct mess,repl;
	mess = commandEnum::STATUS_CMD;
	//checking MACHINE_REQ (0x01) through GET_RPM_REQ (0x09)
	for (std::uint8_t i = 0x01; i <= 0x09; ++i)
	{
		mess.message = i;
		this->send(mess);
	}
	mess = requestEnum::SIGNAL_LED_REQ;
	this->send(mess);
}

void machineInteractionTask::parseResponse(MessageStruct response)
{	
	switch(response.message)
	{
		case (std::uint8_t)replyEnum::DOOR_LOCK_REP:
			this->currentState.doorLock = 
				(response.operand == ((uint8_t)stateEnum::LOCKED));
			break;
			
		case (std::uint8_t)replyEnum::WATER_VALVE_REP:
			this->currentState.waterValve =
				(response.operand == ((uint8_t)stateEnum::OPENED));
			break;
			
		case (std::uint8_t)replyEnum::SOAP_DISPENSER_REP:
			this->currentState.soapDispenser =
				(response.operand == ((uint8_t)stateEnum::OPENED));
			break;
			
		case (std::uint8_t)replyEnum::PUMP_REP:
			this->currentState.pump =
				(response.operand == ((uint8_t)stateEnum::ON));
			break;
			
		case (std::uint8_t)replyEnum::WATER_LEVEL_REP:
			this->currentState.waterLevel = response.operand;
			break;
			
		case (std::uint8_t)replyEnum::HEATING_UNIT_REP:
			this->currentState.heatingUnit =
				(response.operand == ((uint8_t)stateEnum::ON));
			break;
			
		case (std::uint8_t)replyEnum::TEMPERATURE_REP:
			this->currentState.temperature = response.operand;
			break;
			
		case (std::uint8_t)replyEnum::GET_RPM_REP:
		case (std::uint8_t)replyEnum::SET_RPM_REP:
			this->currentState.drumRPM = (response.operand & 0x7F);
			this->currentState.drumClockwise = 
				((response.operand & 0x80) == 0);
			break;
			
		case (std::uint8_t)replyEnum::SIGNAL_LED_REP:
			this->currentState.signalLed =
				(response.operand == ((uint8_t)stateEnum::ON));
			break;
			
		case (std::uint8_t)replyEnum::MACHINE_REP:
			//Sorry for the nested switch... :(
			switch (response.operand){
				case 0x01:
					this->currentState.runState = MachineRunState::HALTED;
					break;
				default:
				case 0x02:
					this->currentState.runState = MachineRunState::IDLE;
					break;
				case 0x04:
					this->currentState.runState = MachineRunState::RUNNING;
					break;
				case 0x08:
					this->currentState.runState = MachineRunState::STOPPED;
					break;
			}
			break;
		case (std::uint8_t)replyEnum::HALTED_REP:
			this->currentState.runState = MachineRunState::HALTED;
			break;
		default:
		//nothing happens here, should only be reached when errors occur.
			std::cout << "Something went wrong..." << std::endl;
			break;
	}
	return;
}

MessageStruct machineInteractionTask::getState(requestEnum request)
{
	if (request == requestEnum::SET_RPM_REQ)
	{
		MessageStruct reply;
		reply.message = 0xff;
		reply.operand = 0xff;
		return reply;
	}
	MessageStruct message;
	message.message = (std::uint8_t)request;
	message.operand = (std::uint8_t)commandEnum::STATUS_CMD; 
	
	return message;
}

bool machineInteractionTask::inSafeState()
{
	return (!currentState.heatingUnit
		&& currentState.drumRPM == 0
		&& currentState.waterLevel ==0);
}

MessageStruct machineInteractionTask::send(MessageStruct message)
{
	this->Uart.write(message);
	sleep(20 MS);
	MessageStruct retval;
	retval = this->Uart.read_16();
	this->parseResponse(retval);
	return retval;
}

void machineInteractionTask::main()
{
	while (1==1)
	{
		RTOS::event e = wait(clock+machineRequestFlag);

		if (e == this->clock)
		{
			poll();
			notifyListeners();
		}
		else if (e == this->machineRequestFlag)
		{
			update();
		}
	}
}
