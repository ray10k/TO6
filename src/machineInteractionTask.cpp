#include "machineInteractionTask.h"
#include <iostream>

machineInteractionTask::machineInteractionTask():
  RTOS::task(5,"machine interaction"),
  machineInstructionChannel (this,"machineInstructionChannel"),
  clock (this, 500 MS, "MIT_clock"),
  currentState(),
  setState(),
  Uart(),
  listeners()
{}

void machineInteractionTask::
	addMachineStateListener(machineStateListener* listener)
{
	this->listeners.push_back(listener);
}

void machineInteractionTask::notifyListeners()
{
	std::vector<machineStateListener*>::iterator listen =
	this->listeners.begin();

	for(;listen != this->listeners.end(); ++listen)
	{
		(*listen)->stateChanged(currentState);
	}
}

void machineInteractionTask::main()
{
	trace;
	for(;;)
	{
		//Wait for clock.
		trace;
		this->wait(this->machineInstructionChannel);
		update();

		//Read the pool and execute request through the uart,
		//returns the response in the ResponseStruct.
		ResponseStruct rs = doRequest(this->machineInstructionChannel.read());

		//Updates the currentState of the machine with the new read value
		//from the ResponseStruct.
		switch(rs.request.request)
		{
			case requestEnum::DOOR_LOCK_REQ:
				currentState.doorLock	 	= rs.value;
				break;
			case requestEnum::WATER_VALVE_REQ:
				currentState.waterValve  	= rs.value;
				break;
			case requestEnum::SOAP_DISPENSER_REQ:
				currentState.soapDispenser 	= rs.value;
				break;
			case requestEnum::PUMP_REQ:
				currentState.pump		 	= rs.value;
				break;
			case requestEnum::HEATING_UNIT_REQ:
				currentState.heatingUnit 	= rs.value;
				break;
			default:
				break;
		}
	}
}

void machineInteractionTask::update()
{
	//Close water valve and stop soap dispenser when
	//the wanted water level is reached.
	if(currentState.waterLevel >= setState.waterLevel)
	{
		if(currentState.waterValve == 1){setWaterValve(0);}
		if(currentState.soapDispenser == 1){setSoapDispenser(0);}
		setState.waterValve = 0; setState.soapDispenser = 0;
	}
	//Stop the pump and unlock the door when there is
	//no water in the washing machine.
	else if(currentState.waterLevel == 0)
	{
		if(currentState.pump == 1)	  {setPump(0);}
		if(currentState.doorLock == 1){setDoorLock(0);}
		setState.pump = 0; setState.doorLock = 0;
	}
	//Turn on the pump if the wanted water level is 0/empty.
	else if(setState.waterLevel == 0)
	{
		if(currentState.pump == 0){setPump(1);}
		setState.pump = 1;
	}

	//Turn heater on or off depending on current temperature
	//compared to wanted temperature
	if(currentState.temperature > setState.temperature)
	{if(currentState.heatingUnit == 1)	{setHeater(0);}}
	if(currentState.temperature < setState.temperature)
	{if(currentState.heatingUnit == 0)	{setHeater(1);}}

	//Update the current state of the washing machine
	//for temperature and waterLevel.
	getTemperature(); getWaterLevel();

	//Update all the listeners.
	notifyListeners();
}

ResponseStruct machineInteractionTask::doRequest(const RequestStruct& req)
{

	//Translate the request to bytes.
	MessageStruct TranslatedRequest = requestTranslate(req);

	//Write the request in bytes to the uart/washing machine.
	Uart.write(TranslatedRequest.message);
	Uart.write(TranslatedRequest.operand);
	this->sleep(10);

	//Read the response byte of the request.
	std::uint16_t responseByte = Uart.read_16();
	//Translate the response from byte to words and return it.
	return responseTranslate(responseByte, req);
}

void machineInteractionTask::setTemperature(unsigned int temperature)
{
	setState.temperature = temperature;
}

void machineInteractionTask::setWaterLevel(unsigned int waterLevel)
{
	setState.waterLevel = waterLevel;
	setState.doorLock = true;

	setDoorLock(1); //Locks the door
	if(currentState.waterLevel < waterLevel)
	{if(currentState.waterValve == 0)	{setWaterValve(1);}}
}

void machineInteractionTask::setRPM(bool clockwise, unsigned int rpm)
{
	setState.drumRPM = rpm;
	setState.drumClockwise = clockwise;

	RequestStruct reqS;
	reqS.request = requestEnum::SET_RPM_REQ;
	if(clockwise){reqS.command = commandEnum::RPM_Clockwise;}
	else{reqS.command = commandEnum::RPM_counterClockwise;}

	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setDetergent(bool add)
{
	setState.soapDispenser = add;
	if(currentState.soapDispenser != add){setSoapDispenser(add);}
}

void machineInteractionTask::flush()
{
	setState.temperature = 20; //Reset to standby temperature
	setState.waterLevel = 0;
	if(currentState.waterLevel > 0){
		if(!currentState.pump){
			setPump(1);
		}
	}
}

void machineInteractionTask::setMachineState(bool start)
{
	RequestStruct reqS;
	reqS.request = requestEnum::MACHINE_REQ;
	if(start){ reqS.command = commandEnum::START_CMD; }
	else { reqS.command = commandEnum::STOP_CMD; }
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setDoorLock(bool lock)
{
	RequestStruct reqS;
	reqS.request = requestEnum::DOOR_LOCK_REQ;
	if(lock){ reqS.command = commandEnum::LOCK_CMD; }
	else { reqS.command = commandEnum::UNLOCK_CMD; }
	this-> machineInstructionChannel.write(reqS);
}

//Function marked private, and doesn't get called at all. Still needed?
void machineInteractionTask::getState(requestEnum request)
{
	RequestStruct reqS;
	reqS.request = request;
	reqS.command = commandEnum::STATUS_CMD;
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::getWaterLevel()
{
	RequestStruct reqS;
	reqS.request = requestEnum::WATER_LEVEL_REQ;
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setWaterValve(bool open)
{
	RequestStruct reqS;
	reqS.request = requestEnum::WATER_VALVE_REQ;
	if(open){ reqS.command = commandEnum::OPEN_CMD; }
	else { reqS.command = commandEnum::CLOSE_CMD; }
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setSoapDispenser(bool open)
{
	RequestStruct reqS;
	reqS.request = requestEnum::SOAP_DISPENSER_REQ;
	if(open){ reqS.command = commandEnum::OPEN_CMD; }
	else { reqS.command = commandEnum::CLOSE_CMD; }
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setPump(bool on)
{
	RequestStruct reqS;
	reqS.request = requestEnum::PUMP_REQ;
	if(on){ reqS.command = commandEnum::ON_CMD; }
	else { reqS.command = commandEnum::OFF_CMD; }
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::getTemperature()
{
	RequestStruct reqS;
	reqS.request = requestEnum::TEMPERATURE_REQ;
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setHeater(bool on)
{
	RequestStruct reqS;
	reqS.request = requestEnum::HEATING_UNIT_REQ;
	if(on){ reqS.command = commandEnum::ON_CMD; }
	else { reqS.command = commandEnum::OFF_CMD; }
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::getRPM()
{
	RequestStruct reqS;
	reqS.request = requestEnum::GET_RPM_REQ;
	this-> machineInstructionChannel.write(reqS);
}

void machineInteractionTask::setSignalLed(bool on)
{
	RequestStruct reqS;
	reqS.request = requestEnum::SIGNAL_LED_REQ;
	if(on){ reqS.command = commandEnum::ON_CMD; }
	else { reqS.command = commandEnum::OFF_CMD; }
	this-> machineInstructionChannel.write(reqS);
}


MessageStruct machineInteractionTask::requestTranslate(RequestStruct reqS){
	MessageStruct retval;
	//Check which request byte should be send
	switch(reqS.request){
		case requestEnum::MACHINE_REQ: 			retval.message = 0x01; break;
		case requestEnum::DOOR_LOCK_REQ: 		retval.message = 0x02; break;
		case requestEnum::WATER_VALVE_REQ:		retval.message = 0x03; break;
		case requestEnum::SOAP_DISPENSER_REQ:	retval.message = 0x04; break;
		case requestEnum::PUMP_REQ:				retval.message = 0x05; break;
		case requestEnum::WATER_LEVEL_REQ:		retval.message = 0x06; break;
		case requestEnum::HEATING_UNIT_REQ:		retval.message = 0x07; break;
		case requestEnum::TEMPERATURE_REQ:		retval.message = 0x08; break;
		case requestEnum::SET_RPM_REQ:			retval.message = 0x0A; break;
		case requestEnum::GET_RPM_REQ:			retval.message = 0x09; break;
		case requestEnum::SIGNAL_LED_REQ:		retval.message = 0x0B; break;
		default:								retval.message = 0x00; break;
	}
	//Check which command byte should be send.
	switch(reqS.command){
		case commandEnum::STATUS_CMD:	retval.operand = 0x01; break;
		case commandEnum::LOCK_CMD:		retval.operand = 0x40; break;
		case commandEnum::UNLOCK_CMD:	retval.operand = 0x80; break;
		
		case commandEnum::START_CMD:	
		case commandEnum::OPEN_CMD:	
		case commandEnum::ON_CMD:		retval.operand = 0x10; break;
		
		case commandEnum::STOP_CMD:	
		case commandEnum::CLOSE_CMD:	
		case commandEnum::OFF_CMD:		retval.operand = 0x20; break;
		case commandEnum::RPM_Clockwise: 		
			retval.operand = (setState.drumRPM | 0x80);  break;
		case commandEnum::RPM_counterClockwise: 	
			retval.operand = setState.drumRPM; 		break;
		default:
			break;
	}

	return retval;
}

ResponseStruct machineInteractionTask::responseTranslate(
	std::uint16_t response, const RequestStruct& reqS)
{
	//ResponseStruct that will contain the response that will be returned.
	ResponseStruct resS;
	resS.request = reqS;		//The request that was send and caused the response.
	resS.value = (std::uint8_t) ((response>>8)&0xff);	
	
	//Check to which request this response came from and what the returned byte means.
	switch(reqS.request)
	{
		case requestEnum::MACHINE_REQ:
			switch(response)
			{
				case 0x01: resS.response = "HALTED"; 	break;
				case 0x02: resS.response = "IDLE";		break;
				case 0x04: resS.response = "RUNNING";	break;
				case 0x08: resS.response = "STOPPED";	break;
			}
			break;

		case requestEnum::DOOR_LOCK_REQ: 
		case requestEnum::WATER_VALVE_REQ: 
		case requestEnum::SOAP_DISPENSER_REQ:
			switch(response)
			{
				case 0x01: resS.response = "OPENED"; resS.value = 1; break;
				case 0x02: resS.response = "CLOSED"; resS.value = 0; break;
				case 0x04: if(reqS.request == requestEnum::DOOR_LOCK_REQ)
						  {resS.response = "LOCKED";}	break;
			}
			break;

		case requestEnum::PUMP_REQ: 
		case requestEnum::HEATING_UNIT_REQ: 
		case requestEnum::SIGNAL_LED_REQ:
			switch(response)
			{
				case 0x08: resS.response = "ON";  resS.value = 1;	break;
				case 0x10: resS.response = "OFF"; resS.value = 0;	break;
			}
			break;

		case requestEnum::WATER_LEVEL_REQ:
			currentState.waterLevel = response; //Save read waterLevel value
			resS.response = "Niveau in %";
			break;

		case requestEnum::TEMPERATURE_REQ:
			currentState.temperature = response; //Save read temperature value
			resS.response = "Temp in Graden Celcius";
			break;

		case requestEnum::SET_RPM_REQ: 
		case requestEnum::GET_RPM_REQ:
		//What do these values mean? (COMMENT_ME)
			if(response >= (0x00|0x80) && response <= (0x40|0x80))
			{
				currentState.drumRPM = response|0x80; 	//Save read RPM value
				currentState.drumClockwise = true;			//Save RPM is going clockwise
				resS.response = "RPM_Clockwise";
			}
			else if(response >= 0x00 && response <= 0x40)
			{
				currentState.drumRPM = response;	//Save read RPM value
				currentState.drumClockwise = false;		//Save RPM is going counterclockwise
				resS.response = "RPM_counterClockwise";
			}
			break;

		default:
			resS.response = "NO VALID REQUEST";
			break;
	}
	return resS;
}
