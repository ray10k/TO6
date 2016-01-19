#include "MachineInteractionTask.h"

MachineInteractionTask::MachineInteractionTask():
  setMachineStateChannel (this, "set_Machine_state_channel"),
  clock (this, 500 MS, "MIT_clock")
  {
	Uart = new uart;
  }
  
void MachineInteractionTask::setHeater(bool on)
{
	this-> SetMachineStateChannel.write("HEATING_UNIT_REQ");
	if(on){ this-> SetMachineStateChannel.write("ON_CMD"); }
	else { this-> SetMachineStateChannel.write("OFF_CMD"); }
}

void MachineInteractionTask::Main()
{	
	for(;;)
	{
		//if(Clock >= 500)
		RTOS::wait(this->clock);
	
		//read the request in words.
		std::string request = SetMachineStateChannel.read(); //HEATING_UNIT_REQ
		
		//read the command.
		std::string command = SetMachineStateChannel.read(); //ON_CMD
		
		//Translate the request to bytes.
		std::vector<std::uint8_t> TranslatedRequest = requestTranslate(request,command); 
		
		//Write the request in bytes to the uart/washing machine.
		uart.write(TranslatedRequest);
		RTOS::wait(10);
		
		//Read the response of the request.
		std::uint8_t response = uart.read();
		
		//Translate the response from bytes to words.
		ResponseStruct TranslatedResponse = responseTranslate(response, request); 
		
		//Write the translated response in the machine state channel.
		machineStateChannel.write(TranslatedResponse); //LET OP!! Strings in de channel
	}
	
}

std::vector<std::uint8_t> MachineInteractionTask::requestTranslate(std::string request, std::string command)
{
	std::vector<std::uint8_t> bytes;
	
	switch(request)
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
	
	switch(command)
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

ResponseStruct MachineInteractionTask::responseTranslate(std::uint8_t responseByte, std::string request)
{
	ResponseStruct rs;
	rs.request = request;
	rs.value = responseByte;
	
	switch(request)
	{
		case "MACHINE_REQ":
			switch(responseByte)
			{
				case 0x01: rs.response = "HALTED"; 	break;
				case 0x02: rs.response = "IDLE";	break;
				case 0x04: rs.response = "RUNNING";	break;
				case 0x08: rs.response = "STOPPED";	break;
			}
			break;
		
		case "DOOR_LOCK_REQ": case "WATER_VALVE_REQ": case "SOAP_DISPENSER_REQ":
			switch(responseByte)
			{
				case 0x01: rs.response = "OPENED";		break;
				case 0x02: rs.response = "CLOSED";		break;
				case 0x04: if(request == "DOOR_LOCK_REQ")
						  {rs.response = "LOCKED";}	break;
			}
			break;
			
		case "PUMP_REQ": case "HEATING_UNIT_REQ": case "SIGNAL_LED_REQ":
			switch(responseByte)
			{
				case 0x08: rs.response = "ON"; 	break;
				case 0x10: rs.response = "OFF";	break;
			}
			break;
			
		case "WATER_LEVEL_REQ":
			rs.response = "Niveau in %";
			break;
			
		case "TEMPERATURE_REQ":
			rs.response = "Temp in Graden Celcius";
			break;
			
		case "SET_RPM_REQ": case "GET_RPM_REQ":
			rs.response = "RPM";
			break;
			
		default:
			rs.response = "NO VALID REQUEST"
			break;
	}
	return rs;
}