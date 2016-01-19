//**************************************
//! Class responsible for communication between the device and the physical washing machine.
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
	//! Creates an empty uart object.
	uart(void);
	//!
	void write(std::vector<std::uint8_t> request);
	//!
	std::uint8_t read(void);
private:
	LibSerial ls;
}