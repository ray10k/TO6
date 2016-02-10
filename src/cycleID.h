#ifndef __CYCLEID_H
#define __CYCLEID_H

#include <string>

class cycleID
{
public:
	cycleID(const std::string& cycleName, const std::string& owner);
	cycleID(const cycleID& rhs);
	bool operator== (const cycleID& lhs) const;

	std::string name;
	std::string user;
}

#endif
