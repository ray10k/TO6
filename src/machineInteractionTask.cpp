#include "machineInteractionTask.h"
#include <iostream>

const int MAX_DRUM_RPM = 1600;

machineInteractionTask::machineInteractionTask():
	RTOS::task(5,"Machine Interaction Task"),
	machineRequestFlag(this,"Machine Request Flag"),
	clock(this,500 MS,"Machine update clock"),
	currentState(),
	targetState(),
	Uart(),
	listeners(),
	running(false)
{
	//during startup, send the washing machine the instruction to get into
	//its running state. Prevents trouble later down the line.
	MessageStruct startup;
	startup.message = (std::uint8_t)requestEnum::MACHINE_REQ;
	startup.operand = (std::uint8_t)commandEnum::START_CMD;
	Uart.write(startup);
	MessageStruct reply; //living on a prayer, hoping the WM doesnt need long.
	reply = Uart.read_16();
	this->parseResponse(reply);
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
	MachineState toSend(this->currentState);
	std::vector<machineStateListener*>::iterator listen;
	
	for (listen = this->listeners.begin();
			listen != this->listeners.end();
			++listen)
	{
		(*listen)->stateChanged(toSend);
	}
}

void machineInteractionTask::update()
{
	//don't send instructions when the machine isn't running, or the door isn't
	//locked. Receipe for disaster...
	if (!this->running || !this->currentState.doorLock)
	{
		if (this->currentState.waterLevel==0 && this->currentState.doorLock)
		{
			MessageStruct open;
			open = requestEnum::DOOR_LOCK_REQ;
			open = commandEnum::UNLOCK_CMD;
			this->Uart.write(open);
			this->sleep(10 MS);
			MessageStruct reply;
			reply = this->Uart.read_16();
			this->parseResponse(reply);
		}
		else if (this->currentState.waterLevel > 0)
		{
			MessageStruct drain;
			drain = requestEnum::PUMP_REQ;
			drain = commandEnum::ON_CMD;
			
			MessageStruct stop;
			stop = requestEnum::WATER_VALVE_REQ;
			stop = commandEnum::CLOSE_CMD;
			
			MessageStruct reply;
			
			this->Uart.write(drain);
			this->sleep(10 MS);
			reply = this->Uart.read_16();
			this->parseResponse(reply);
			
			this->Uart.write(stop);
			this->sleep(10 MS);
			reply = this->Uart.read_16();
			this->parseResponse(reply);
		}
		return;
	}
	
	std::vector<MessageStruct> toSend;

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
		toSend.push_back(door);
	}

	MessageStruct heater;
	heater = requestEnum::HEATING_UNIT_REQ;
	if (this->currentState.temperature < this->targetState.temperature &&
		! this->currentState.heatingUnit)
	{
		heater = commandEnum::ON_CMD;
		toSend.push_back(heater);
	}
	else if (this->currentState.temperature >= this->targetState.temperature &&
		this->currentState.heatingUnit)
	{
		heater = commandEnum::OFF_CMD;
		toSend.push_back(heater);
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
		toSend.push_back(pump);
		toSend.push_back(valve);
	}
	else
	{
		
		pump = commandEnum::CLOSE_CMD;
		valve = commandEnum::CLOSE_CMD;
		//close both by default, asses whether they should be opened with some
		//tolerances.
		if (this->currentState.waterLevel < this->targetState.waterLevel + 2)
		{
			if (!this->currentState.waterValve)
			{
				valve = commandEnum::OPEN_CMD;
				toSend.push_back(valve);
			}
			
			if (this->currentState.pump)
			{
				pump = commandEnum::OFF_CMD;
				toSend.push_back(pump);
			}
		}
		else if (this->currentState.waterLevel > this->targetState.waterLevel-2)
		{
			if (this->currentState.waterValve)
			{
				valve= commandEnum::CLOSE_CMD;
				toSend.push_back(valve);
			}
			
			if (!this->currentState.pump)
			{
				pump = commandEnum::ON_CMD;
				toSend.push_back(pump);
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
		toSend.push_back(soap);
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
		toSend.push_back(led);
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
		toSend.push_back(drum);
	}
	
	//all messages prepared, time to write them to the washing machine.
	std::vector<MessageStruct>::iterator msg;
	for (msg = toSend.begin(); msg != toSend.end();++msg)
	{
		this->Uart.write(*msg);
		sleep(10 MS);
		MessageStruct reply;
		reply = this->Uart.read_16();
		this->parseResponse(reply);
#ifdef DEBUG
		std::cout << 'I';
#endif
	}
#ifdef DEBUG	
	std::cout << std::endl;
#endif
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
			this->running = 
				(response.operand == ((uint8_t)stateEnum::RUNNING));
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

void machineInteractionTask::main()
{
	while (1==1)
	{
		RTOS::event e = wait(clock+machineRequestFlag);
		update();
		if (e == this->clock)
		{
			notifyListeners();
		}
	}
}
