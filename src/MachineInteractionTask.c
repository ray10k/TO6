#include "MachineInteractionTask.h"


void MachineInteractionTask::Main(){
	MachineInteractionTask():
		setMachineStateChannel (this, "set_Machine_state_channel");
		clock (this, 250, "MIT_clock");
		{}
	
	for(;;){
		//if(Clock >= 250){ ???????????????????????????
		
		//read the request in words.
		std::string request = SetMachineStateChannel.read(); //TEMP_REQ
		
		//read the command.
		std::string command = SetMachineStateChannel.read(); //ON_CMD
		
		//Translate the request to bytes.
		std::vector<std::uint8_t> TranslatedRequest = WasMaschine.RequestTranslate(request,command); 
		
		//Write the request in bytes to the uart/washing machine.
		uart.write(TranslatedRequest);
		RTOS::wait(10);
		
		//Read the response of the request.
		std::vector<std::uint8_t> response = uart.read();
		
		//Translate the response from bytes to words.
		std::string TranslatedResponse = WasMaschine.ResponseTranslate(response); 
		
		//Write the translated response in the machine state channel.
		machineStateChannel.write(TranslatedResponse); //LET OP!! Strings in de channel
		RTOS::wait(500);
	}
	
}