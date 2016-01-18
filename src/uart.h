#include "libserial.h"
#include <vector>
#include <string.h>

class uart
{
public:
	uart(void);
	void write(std::vector<std::uint8_t> request, int bytes);
	void read(void);
private:
	LibSerial ls;
}