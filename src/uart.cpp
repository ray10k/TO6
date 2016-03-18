#include "uart.h"
#include <iostream>

uart::uart(void): ls()
{
	ls.open("/dev/ttyAMA0", 9600);
}

void uart::write(std::vector<std::uint8_t>* request)
{
    int nbBytes = sizeof(request);
	ls.write(request,nbBytes);
#ifdef DEBUG
	std::cout << "reading multiple bytes: " << std::hex;
	std::vector<std::uint8_t>::iterator bytes;
	for (bytes = request->begin(); bytes != request->end(); ++bytes)
	{
		std::cout << (int) *bytes << ' ';
	}
	std::cout << std::endl;
#endif
	ls.flush();
}

void uart::write(std::uint16_t request){
	char a,b;
	a = (char) (request &0xff);
	b = (char) ((request >> 8)&0xff);
	ls.writeChar(a);
	ls.writeChar(b);
#ifdef DEBUG
	std::cout << std::hex << "write x2: "<<(int)a << ' ' << (int)b << std::endl;
#endif
	ls.flush();
}

std::uint8_t uart::read(void)
{
	std::uint8_t response = 0;
	ls.read(&response, 1);
	ls.flush();
#ifdef DEBUG
	std::cout << std::hex << "read: "<<(int)response << std::endl;
#endif
	return response;
}

std::uint16_t uart::read_16(void)
{
	std::uint16_t retval = 0;
	std::uint8_t temp[2];
	ls.read(&temp,2);
	retval = temp[0] | (temp[1]<<8);
#ifdef DEBUG
	std::cout << std::hex << "read (16): "<<(int)retval << std::endl;
#endif
	return retval;
}
