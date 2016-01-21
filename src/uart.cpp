#include "uart.h"

uart::uart(void): ls()
{
	ls.open("/dev/ttySO", 9600);
}

void uart::write(std::vector<std::uint8_t> request)
{
	ls.write(request, sizeof(request));
	ls.flush();
}

std::uint8_t uart::read(void)
{
	std::uint8_t response;
	ls.read(response, 1);
	ls.flush();
	return response;
}
