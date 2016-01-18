#include <vector>
#include <string.h>

class washingMachine
{
public:
	washingMachine(void);
	std::vector<std::uint8_t> requestTranslate(std::string request, std::string command);
	std::string responseTranslate(std::vector<std::uint8_t> response, std::string request);
}