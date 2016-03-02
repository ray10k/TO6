#include "uart.h"

uart::uart(void): ls()
{
	ls.open("/dev/ttySO", 9600);
}

void uart::write(std::vector<std::uint8_t>* request)
{
    int nbBytes = sizeof(request);
	ls.write(request,nbBytes);
	ls.flush();
}

void uart::write(std::uint16_t request){
	char a,b;
	a = (char) (request &0xff);
	b = (char) ((request >> 8)&0xff);
	ls.writeChar(a);
	ls.writeChar(b);
	ls.flush();
}

std::uint8_t uart::read(void)
{
	std::uint8_t response = 0;
	ls.read(&response, 1);
	ls.flush();
	return response;
}
