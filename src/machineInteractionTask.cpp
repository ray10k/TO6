#include "machineInteractionTask.h"

const int MAX_DRUM_RPM = 1600;

machineInteractionTask::machineInteractionTask():
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
}

void machineInteractionTask::addMachineStateListener
						(machineStateListener* listener)
{
	this->listeners.push_back(listener);
}

void machineInteractionTask::setTemperature(unsigned int temperature)
{
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
	this->targetState.soapDispenser = add;
	this->machineRequestFlag.set();
}

void machineInteractionTask::flush()
{
	this->targetState.waterLevel = 200; //Out-of-bounds to indicate the actual
	//level should be ignored during status update.
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
			open.message = (std::uint8_t)requestEnum::DOOR_LOCK_REQ;
			open.operand = (std::uint8_t)commandEnum::UNLOCK_CMD;
			this->Uart.write(open);
			this->sleep(10 MS);
			MessageStruct reply;
			reply = this->Uart.read_16();
			this->parseResponse(reply);
		}
		else if (this->currentState.waterLevel > 0)
		{
			MessageStruct drain;
			drain.message = (std::uint8_t)requestEnum::PUMP_REQ;
			drain.operand = (std::uint8_t)commandEnum::ON_CMD;
			
			MessageStruct stop;
			stop.message = (std::uint8_t)requestEnum::WATER_VALVE_REQ;
			stop.operand = (std::uint8_t)commandEnum::CLOSE_CMD;
			
			this->Uart.write(drain);
			this->Uart.write(stop);
			this->sleep(10 MS);
			MessageStruct reply;
			reply = this->Uart.read_16();
			this->parseResponse(reply);
			reply = this->Uart.read_16();
			this->parseResponse(reply);
		}
	}
	
	std::vector<MessageStruct> toSend;

	MessageStruct door;
	door.message = (std::uint8_t)requestEnum::DOOR_LOCK_REQ;
	if (this->targetState.doorLock)
	{
		door.operand = (std::uint8_t)commandEnum::LOCK_CMD;
	}
	else
	{
		door.operand = (std::uint8_t)commandEnum::UNLOCK_CMD;
	}
	toSend.push_back(door);
	
	MessageStruct heater;
	heater.message = (std::uint8_t)requestEnum::HEATING_UNIT_REQ;
	
	if (this->currentState.temperature > this->targetState.temperature)
	{
		heater.operand = (std::uint8_t)commandEnum::ON_CMD;
	}
	else
	{
		heater.operand = (std::uint8_t)commandEnum::OFF_CMD;
	}
	
	toSend.push_back(heater);
	
	MessageStruct pump,valve;
	pump.message = (std::uint8_t)requestEnum::PUMP_REQ;
	valve.message = (std::uint8_t)requestEnum::WATER_VALVE_REQ;
	
	if (this->targetState.waterLevel > 100)
	{
		//flush by running the water *and* opening the valve.
		pump.operand = (std::uint8_t)commandEnum::OPEN_CMD;
		valve.operand = (std::uint8_t)commandEnum::OPEN_CMD;
	}
	else
	{
		pump.operand = (std::uint8_t)commandEnum::CLOSE_CMD;
		valve.operand = (std::uint8_t)commandEnum::CLOSE_CMD;
		//close both by default, asses whether they should be opened with some
		//tolerances.
		if (this->targetState.waterLevel-3 < this->currentState.waterLevel)
		{
			//intended water level below actual, need more water.
			pump.operand = (std::uint8_t)commandEnum::OPEN_CMD;
		}
		else if 
		(this-> targetState.waterLevel+3 > this->currentState.waterLevel)
		{
			valve.operand = (std::uint8_t)commandEnum::OPEN_CMD;
		}
	}
	toSend.push_back(pump);
	toSend.push_back(valve);
	
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
	
	MessageStruct drum;
	drum.message = (std::uint8_t)requestEnum::SET_RPM_REQ;
	drum.operand = (std::uint8_t)this->targetState.drumRPM;
	if (! this->targetState.drumClockwise)
	{
		drum.operand |= (std::uint8_t)commandEnum::RPM_Clockwise;
	}
	toSend.push_back(drum);
	
	//all messages prepared, time to write them to the washing machine.
	std::vector<MessageStruct>::iterator msg;
	for (msg = toSend.begin(); msg != toSend.end();++msg)
	{
		this->Uart.write(*msg);
	}
	sleep(10 MS);
	for (msg = toSend.begin(); msg != toSend.end();++msg)
	{
		MessageStruct reply;
		reply = this->Uart.read_16();
		this->parseResponse(reply);
	}
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
			
		default:
		//nothing happens here, should only be reached when errors occur.
			trace;
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
		wait(clock+machineRequestFlag);
		update();
		notifyListeners();
		trace;
	}
}
