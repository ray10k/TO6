//******************************************************************************
//! Simple class to combine a cycle name and username to create a semi-unique
//! identifier for a cycle.
//! 
//! \authors
//! 	- Wouter van den Heuvel
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//******************************************************************************

#ifndef __CYCLEID_H
#define __CYCLEID_H

#include <string>

class cycleID
{
public:
	//! Constructor; Just builds up the cycleID with the given details.
	cycleID(const std::string& cycleName, const std::string& owner);
	//! Copy constructor.
	cycleID(const cycleID& rhs);
	//! Returns true if both cycleID's have the same name and user, false if
	//! either differs.
	bool operator== (const cycleID& lhs) const;

	std::string name;
	std::string user;
};

#endif
