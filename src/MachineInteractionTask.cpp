#include "MachineInteractionTask.h"

MachineInteractionTask::MachineInteractionTask():
  setMachineStateChannel (this, "set_Machine_state_channel"),
  clock (this, 500 MS, "MIT_clock"),
  Uart(),
  listeners(),
  currentState()
  {}

void MachineInteractionTask::addMachineStateListener(machineStateListener& listener)
{
	listeners.push_back(listener);
}
  
void MachineInteractionTask::notifyListeners(ResponseStruct response)
{
	std::vector<machineStateListener>::iterator listen = this->listeners.begin();
	
	for(;listen != this->listeners.end(); ++listen)
	{
		*listen.response(response);
	}
	if(stateUpdated)
	{
		for(;listen != this->listeners.end(); ++listen)
		{
			*listen.stateChanged(currentState);
		}
	}
}

void MachineInteractionTask::Main()
{	
	for(;;)
	{
		//if(Clock >= 500)
		RTOS::wait(this->clock);
	
		//read the request in words.
		RequestStruct request = SetMachineStateChannel.read(); //{"HEATING_UNIT_REQ", "ON_CMD"}
		
		//Translate the request to bytes.
		std::vector<std::uint8_t> TranslatedRequest = requestTranslate(request); 
		
		//Write the request in bytes to the uart/washing machine.
		uart.write(TranslatedRequest);
		RTOS::wait(10);
		
		//Read the response of the request.
		std::uint8_t responseByte = uart.read();
		
		//Translate the response from bytes to words.
		ResponseStruct TranslatedResponse = responseTranslate(responseByte, request); 
		
		//Update all the listeners.
		notifyListeners(TranslatedResponse);
	}
}

void MachineInteractionTask::setTemperature(unsigned int temperature)
{
	
}

void MachineInteractionTask::getState(std::string request)
{
	RequestStruct reqS;
	reqS.request = request;
	reqS.command = "STATUS_CMD";
	this-> SetMachineStateChannel.write(reqS);
}  
  
void MachineInteractionTask::setMachineState(bool start)
{
	RequestStruct reqS;
	reqS.request = "MACHINE_REQ";
	if(start){ reqS.command = "START_CMD"; }
	else { reqS.command = "STOP_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::setDoorLock(bool lock)
{
	RequestStruct reqS;
	reqS.request = "DOOR_LOCK_REQ";
	if(lock){ reqS.command = "LOCK_CMD"; }
	else { reqS.command = "UNLOCK_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::getWaterLevel()
{
	RequestStruct reqS;
	reqS.request = "WATER_LEVEL_REQ";
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::setWaterValve(bool open)
{
	RequestStruct reqS;
	reqS.request = "WATER_VALVE_REQ";
	if(open){ reqS.command = "OPEN_CMD"; }
	else { reqS.command = "CLOSE_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::setSoapDispenser(bool open)
{
	RequestStruct reqS;
	reqS.request = "SOAP_DISPENSER_REQ";
	if(open){ reqS.command = "OPEN_CMD"; }
	else { reqS.command = "CLOSE_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::setPump(bool on)
{
	RequestStruct reqS;
	reqS.request = "PUMP_REQ";
	if(on){ reqS.command = "ON_CMD"; }
	else { reqS.command = "OFF_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::getTemperature()
{
	RequestStruct reqS;
	reqS.request = "TEMPERATURE_REQ";
	this-> SetMachineStateChannel.write(reqS);
}
  
void MachineInteractionTask::setHeater(bool on)
{
	RequestStruct reqS;
	reqS.request = "HEATING_UNIT_REQ";
	if(on){ reqS.command = "ON_CMD"; }
	else { reqS.command = "OFF_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::getRPM()
{
	RequestStruct reqS;
	reqS.request = "GET_RPM_REQ";
	this-> SetMachineStateChannel.write(reqS);
}
  
void MachineInteractionTask::setRPM(...)
{
	RequestStruct reqS;
	reqS.request = "SET_RPM_REQ";
	this-> SetMachineStateChannel.write(reqS);
}

void MachineInteractionTask::setSignalLed(bool on)
{
	RequestStruct reqS;
	reqS.request = "SIGNAL_LED_REQ";
	if(on){ reqS.command = "ON_CMD"; }
	else { reqS.command = "OFF_CMD"; }
	this-> SetMachineStateChannel.write(reqS);
}

std::vector<std::uint8_t> MachineInteractionTask::requestTranslate(RequestStruct reqS)
{
	std::vector<std::uint8_t> bytes;
	
	switch(reqS.request)
	{
		case "MACHINE_REQ": 		bytes[0] = 0x01; break;
		case "DOOR_LOCK_REQ": 		bytes[0] = 0x02; break;
		case "WATER_VALVE_REQ":		bytes[0] = 0x03; break;
		case "SOAP_DISPENSER_REQ":	bytes[0] = 0x04; break;
		case "PUMP_REQ": 			bytes[0] = 0x05; break;
		case "WATER_LEVEL_REQ":		bytes[0] = 0x06; break;
		case "HEATING_UNIT_REQ":	bytes[0] = 0x07; break;	
		case "TEMPERATURE_REQ":		bytes[0] = 0x08; break;
		case "SET_RPM_REQ":			bytes[0] = 0x0A; break;	
		case "GET_RPM_REQ":			bytes[0] = 0x09; break;
		case "SIGNAL_LED_REQ":		bytes[0] = 0x0B; break;
		default:					return -1;		 break;
	}
	
	switch(reqS.command)
	{
		case "STATUS_CMD":	bytes[1] = 0x01; break;
		case "LOCK_CMD":	bytes[1] = 0x40; break;
		case "UNLOCK_CMD":	bytes[1] = 0x80; break;
		case "START_CMD":	case "OPEN_CMD":	case "ON_CMD":	bytes[1] = 0x10; break;
		case "STOP_CMD":	case "CLOSE_CMD":	case "OFF_CMD":	bytes[1] = 0x20; break;
		//case "RPM":
		//default:	break;
	}
	
	return bytes;
}

ResponseStruct MachineInteractionTask::responseTranslate(std::uint8_t responseByte, RequestStruct reqS)
{
	ResponseStruct resS;
	resS.request = reqS;
	resS.value = responseByte;
	
	switch(reqS.request)
	{
		case "MACHINE_REQ":
			switch(responseByte)
			{
				case 0x01: resS.response = "HALTED"; 	break;
				case 0x02: resS.response = "IDLE";	break;
				case 0x04: resS.response = "RUNNING";	break;
				case 0x08: resS.response = "STOPPED";	break;
			}
			break;
		
		case "DOOR_LOCK_REQ": case "WATER_VALVE_REQ": case "SOAP_DISPENSER_REQ":
			switch(responseByte)
			{
				case 0x01: resS.response = "OPENED";		break;
				case 0x02: resS.response = "CLOSED";		break;
				case 0x04: if(reqS.request == "DOOR_LOCK_REQ")
						  {resS.response = "LOCKED";}	break;
			}
			break;
			
		case "PUMP_REQ": case "HEATING_UNIT_REQ": case "SIGNAL_LED_REQ":
			switch(responseByte)
			{
				case 0x08: resS.response = "ON"; 	break;
				case 0x10: resS.response = "OFF";	break;
			}
			break;
			
		case "WATER_LEVEL_REQ":
			resS.response = "Niveau in %";
			break;
			
		case "TEMPERATURE_REQ":
			resS.response = "Temp in Graden Celcius";
			break;
			
		case "SET_RPM_REQ": case "GET_RPM_REQ":
			resS.response = "RPM";
			break;
			
		default:
			resS.response = "NO VALID REQUEST"
			break;
	}
	return resS;
}