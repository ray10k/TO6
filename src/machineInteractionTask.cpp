#include "machineInteractionTask.h"

machineInteractionTask::machineInteractionTask():
  RTOS::task(5,"machine interaction"),
  machineInstructionPool ("machineInstructionPool"),
  clock (this, 500 MS, "MIT_clock"),
  Uart(),
  listeners(),
  currentState(),
  setState()
{}

void machineInteractionTask::addMachineStateListener(machineStateListener& listener)
{
	this->listeners.push_back(listener);
}

void machineInteractionTask::notifyListeners()
{
	std::vector<machineStateListener>::iterator listen =
	this->listeners.begin();

	for(;listen != this->listeners.end(); ++listen)
	{
		(*listen).stateChanged(currentState);
	}
}

void machineInteractionTask::main()
{
	for(;;)
	{
		//Wait for clock.
		this->wait(this->clock);
		update();

		//Read the pool and execute request through the uart,
		//returns the response in the ResponseStruct.
		ResponseStruct rs = readPool();
		//Updates the currentState of the machine with the new read value
		//from the ResponseStruct.
		switch(rs.request.request)
		{
			case DOOR_LOCK_REQ:
				currentState.doorLock	 	= rs.value;
				break;
			case WATER_VALVE_REQ:
				currentState.waterValve  	= rs.value;
				break;
			case SOAP_DISPENSER_REQ:
				currentState.soapDispenser 	= rs.value;
				break;
			case PUMP_REQ:
				currentState.pump		 	= rs.value;
				break;
			case HEATING_UNIT_REQ:
				currentState.heatingUnit 	= rs.value;
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

ResponseStruct machineInteractionTask::readPool()
{
	//Read the request in words.
	RequestStruct request = machineInstructionPool.read();
	//return example: {"HEATING_UNIT_REQ", "ON_CMD"}

	//Translate the request to bytes.
	std::vector<std::uint8_t>* TranslatedRequest = requestTranslate(request);

	//Write the request in bytes to the uart/washing machine.
	Uart.write(TranslatedRequest);
	this->sleep(10);

	//Read the response byte of the request.
	std::uint8_t responseByte = (*Uart.read());

	//Translate the response from byte to words and return it.
	return responseTranslate(responseByte, request);
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
	reqS.request = SET_RPM_REQ;
	if(clockwise){reqS.command = RPM_Clockwise;}
	else{reqS.command = RPM_counterClockwise;}

	this-> machineInstructionPool.write(reqS);
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
	if(currentState.waterLevel > 0)
	{if(currentState.pump = 0){setPump(1);}}
}

void machineInteractionTask::setMachineState(bool start)
{
	RequestStruct reqS;
	reqS.request = MACHINE_REQ;
	if(start){ reqS.command = START_CMD; }
	else { reqS.command = STOP_CMD; }
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::setDoorLock(bool lock)
{
	RequestStruct reqS;
	reqS.request = DOOR_LOCK_REQ;
	if(lock){ reqS.command = LOCK_CMD; }
	else { reqS.command = UNLOCK_CMD; }
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::getState(int request)
{
	RequestStruct reqS;
	reqS.request = request;
	reqS.command = STATUS_CMD;
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::getWaterLevel()
{
	RequestStruct reqS;
	reqS.request = WATER_LEVEL_REQ;
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::setWaterValve(bool open)
{
	RequestStruct reqS;
	reqS.request = WATER_VALVE_REQ;
	if(open){ reqS.command = OPEN_CMD; }
	else { reqS.command = CLOSE_CMD; }
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::setSoapDispenser(bool open)
{
	RequestStruct reqS;
	reqS.request = SOAP_DISPENSER_REQ;
	if(open){ reqS.command = OPEN_CMD; }
	else { reqS.command = CLOSE_CMD; }
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::setPump(bool on)
{
	RequestStruct reqS;
	reqS.request = PUMP_REQ;
	if(on){ reqS.command = ON_CMD; }
	else { reqS.command = OFF_CMD; }
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::getTemperature()
{
	RequestStruct reqS;
	reqS.request = TEMPERATURE_REQ;
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::setHeater(bool on)
{
	RequestStruct reqS;
	reqS.request = HEATING_UNIT_REQ;
	if(on){ reqS.command = ON_CMD; }
	else { reqS.command = OFF_CMD; }
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::getRPM()
{
	RequestStruct reqS;
	reqS.request = GET_RPM_REQ;
	this-> machineInstructionPool.write(reqS);
}

void machineInteractionTask::setSignalLed(bool on)
{
	RequestStruct reqS;
	reqS.request = SIGNAL_LED_REQ;
	if(on){ reqS.command = ON_CMD; }
	else { reqS.command = OFF_CMD; }
	this-> machineInstructionPool.write(reqS);
}


std::vector<std::uint8_t>* machineInteractionTask::requestTranslate(
	RequestStruct reqS)
{
	//Vector that will contain the bytes that will be returned.
	std::vector<std::uint8_t>* bytes;

	//Check which request byte should be send
	switch(reqS.request)
	{
		case MACHINE_REQ: 			(*bytes)[0] = 0x01; break;
		case DOOR_LOCK_REQ: 		(*bytes)[0] = 0x02; break;
		case WATER_VALVE_REQ:		(*bytes)[0] = 0x03; break;
		case SOAP_DISPENSER_REQ:	(*bytes)[0] = 0x04; break;
		case PUMP_REQ: 				(*bytes)[0] = 0x05; break;
		case WATER_LEVEL_REQ:		(*bytes)[0] = 0x06; break;
		case HEATING_UNIT_REQ:		(*bytes)[0] = 0x07; break;
		case TEMPERATURE_REQ:		(*bytes)[0] = 0x08; break;
		case SET_RPM_REQ:			(*bytes)[0] = 0x0A; break;
		case GET_RPM_REQ:			(*bytes)[0] = 0x09; break;
		case SIGNAL_LED_REQ:		(*bytes)[0] = 0x0B; break;
		default:					(*bytes)[0] = 0x00; break;
	}
	//Check which command byte should be send.
	switch(reqS.command)
	{
		case STATUS_CMD:	(*bytes)[1] = 0x01; break;
		case LOCK_CMD:		(*bytes)[1] = 0x40; break;
		case UNLOCK_CMD:	(*bytes)[1] = 0x80; break;
		case START_CMD:	case OPEN_CMD:	case ON_CMD:	(*bytes)[1] = 0x10; break;
		case STOP_CMD:	case CLOSE_CMD:	case OFF_CMD:	(*bytes)[1] = 0x20; break;
		case RPM_Clockwise: 		(*bytes)[1] = setState.drumRPM | 0x80;  break;
		case RPM_counterClockwise: 	(*bytes)[1] = setState.drumRPM; 		break;
		//default:	break;
	}

	return bytes;
}

ResponseStruct machineInteractionTask::responseTranslate(
	std::uint8_t responseByte, RequestStruct reqS)
{
	//ResponseStruct that will contain the response that will be returned.
	ResponseStruct resS;
	resS.request = reqS;		//The request that was send and caused the response.
	resS.value = responseByte;	//Saves the returned value as an int.

	//Check to which request this response came from and what the returned byte means.
	switch(reqS.request)
	{
		case MACHINE_REQ:
			switch(responseByte)
			{
				case 0x01: resS.response = "HALTED"; 	break;
				case 0x02: resS.response = "IDLE";		break;
				case 0x04: resS.response = "RUNNING";	break;
				case 0x08: resS.response = "STOPPED";	break;
			}
			break;

		case DOOR_LOCK_REQ: case WATER_VALVE_REQ: case SOAP_DISPENSER_REQ:
			switch(responseByte)
			{
				case 0x01: resS.response = "OPENED"; resS.value = 1; break;
				case 0x02: resS.response = "CLOSED"; resS.value = 0; break;
				case 0x04: if(reqS.request == DOOR_LOCK_REQ)
						  {resS.response = "LOCKED";}	break;
			}
			break;

		case PUMP_REQ: case HEATING_UNIT_REQ: case SIGNAL_LED_REQ:
			switch(responseByte)
			{
				case 0x08: resS.response = "ON";  resS.value = 1;	break;
				case 0x10: resS.response = "OFF"; resS.value = 0;	break;
			}
			break;

		case WATER_LEVEL_REQ:
			currentState.waterLevel = responseByte; //Save read waterLevel value
			resS.response = "Niveau in %";
			break;

		case TEMPERATURE_REQ:
			currentState.temperature = responseByte; //Save read temperature value
			resS.response = "Temp in Graden Celcius";
			break;

		case SET_RPM_REQ: case GET_RPM_REQ:
			if(responseByte >= 0x00|0x80 && responseByte <= 0x40|0x80)
			{
				currentState.drumRPM = responseByte|0x80; 	//Save read RPM value
				currentState.drumClockwise = true;			//Save RPM is going clockwise
				resS.response = "RPM_Clockwise";
			}
			else if(responseByte >= 0x00 && responseByte <= 0x40)
			{
				currentState.drumRPM = responseByte;	//Save read RPM value
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
