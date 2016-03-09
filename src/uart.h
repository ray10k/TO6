//**************************************
//! Class responsible for communication between the device and the physical
//! washing machine.
//! \authors
//! 	- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#include "libserial/libserial.h"
#include <vector>
#include <string>

class uart
{
public:
	//! Creates an empty uart object and opens the connection.
	uart(void);
	//! Writes the contents of the given vector to the UART.
	void write(std::vector<std::uint8_t>* request);
	//! Writes the given two bytes to the UART, low 8 bits first.
	void write(std::uint16_t request);
	//! reads one byte from the UART.
	std::uint8_t read(void);
	//! reads two bytes from the UART, with the first received 
	//! byte in the low 8 bits.
	std::uint16_t read_16(void);
private:
	LibSerial ls;
};
