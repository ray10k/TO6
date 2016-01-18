#include "washingMachine.h"

washingMachine::washingMachine(void)
{}

std::vector<std::uint8_t> washingMachine::requestTranslate(std::string request, std::string command)
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
	}
	
	return bytes;
}

std::string washingMachine::responseTranslate(std::vector<std::uint8_t> responseByte, std::string request)
{
	std::string response;
	switch(request)
	{
		case "MACHINE_REQ":
			switch(responseByte[0])
			{
				case 0x01: response = "HALTED"; 	break;
				case 0x02: response = "IDLE";		break;
				case 0x04: response = "RUNNING";	break;
				case 0x08: response = "STOPPED";	break;
			}
			break;
		
		case "DOOR_LOCK_REQ": case "WATER_VALVE_REQ": case "SOAP_DISPENSER_REQ":
			switch(responseByte[0])
			{
				case 0x01: response = "OPENED";		break;
				case 0x02: response = "CLOSED";		break;
				case 0x04: if(request == "DOOR_LOCK_REQ")
						  {response = "LOCKED";}	break;
			}
			break;
			
		case "PUMP_REQ": case "HEATING_UNIT_REQ": case "SIGNAL_LED_REQ":
			switch(responseByte[0])
			{
				case 0x08: response = "ON";		break;
				case 0x10: response = "OFF";	break;
			}
			break;
			
		case "WATER_LEVEL_REQ":
			int niveauInProcent = responseByte[0];
			response = "" + niveauInProcent;
			break;
			
		case "TEMPERATURE_REQ":
			int tempInGradenCelcius = responseByte[0];
			response = "" + tempInGradenCelcius;
			break;
			
		case "SET_RPM_REQ": case "GET_RPM_REQ":
			response = "RPM";
			break;
	}
	return response;
}