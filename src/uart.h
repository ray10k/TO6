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
	//! Writes the given two bytes to the UART.
	void write(std::uint16_t request);
	//! reads one byte from the UART.
	std::uint8_t read(void);
private:
	LibSerial ls;
};
