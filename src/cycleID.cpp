#include "cycleID.h"

cycleID::cycleID(const std::string& cycleName, const std::string& owner):
	name(cycleName),
	user(owner)
{}

cycleID::cycleID(const cycleID& rhs):
	name(rhs.name),
	user(rhs.user)
{}

bool cycleID::operator== (const cycleID& lhs) const{
	int nameComp, userComp;
	nameComp = this->name.compare(lhs.name);
	userComp = this->user.compare(lhs.user);
	return (nameComp == 0 && userComp == 0);
}
