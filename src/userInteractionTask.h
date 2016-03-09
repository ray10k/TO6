//**************************************
//! Task responsible for communication between the websocket and
//! saved data like washingCycles and users.
//! \authors
//!		- Wilco Louwerse
//!		- Daniel Klomp
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __USER_INTERACTION_TASK
#define __USER_INTERACTION_TASK

#include "prtos/pRTOS.h"

#include "cycleID.h"
#include "cycleState.h"
#include "cycleStateListener.h"
#include "machineStateListener.h"
#include "washingCycleTask.h"
#include "washingMachineWS.h"
#include "Packet.h"

#include <string>
#include <vector>

//! A struct that combines a userName with its password.
struct User
{
	std::string userName;
	std::string password;
};

//! A struct that is used for saving information about the current step of the
//! current washingCycle and sending this information in a easy way.
struct CycleStep
{
	int totalSteps;
	int currentStep;

	cycleState state ;
	std::string cycleName ;
	std::string stepName ;

	bool finished;		//Is true when the washingCycle finished correctly
								//and wasn't forced to stop.
};

class washingMachineWS;

class userInteractionTask : public RTOS::task, public machineStateListener,
						   public cycleStateListener
{
public:
	//! Constructor.
	userInteractionTask(washingCycleTask* WCT);
	//! An override function from machineStateListener.h.
	void stateChanged(MachineState currentState) override;
	//! An override function from cycleStateListener.h.
	void cycleStateChanged(unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName) override;
	//! An override function from cycleStateListener.h.
	void cyclePaused(const std::string& cycleName,
		const std::string& stepName) override;
	//! An override function from cycleStateListener.h.
	void cycleEnded(bool finished,
		const std::string& cycleName,
		const std::string& stepName) override;

	//The following functions are all used by the websocket to (set/)get information 
	//that this task will provide.
	//! A function used to set the state of the current washingCycle, see cycleState.h.
	void setCycleState(cycleState state);
	//! A function used to load/set a washingCycle that will be run.
	void loadCycle(std::string userName, std::string washingCycleName);
	//! A function that returns all washingCycle names of the washingCycles owned by
	//!	the current active user.
	std::vector<std::string> loadWashingCycleNames();
	//! A function that returns the total amount of steps of a given washingCycle.
	int getTotalCycleSteps(cycleID id);

	//! A function that adds a new user to the list of users.
	void addUser(User user);
	//! A function that returns true if the given userName exists.
	bool checkUserName(std::string userName);
	//! A function that returns true if the given userName has the same password 
	//! as the given password.
	bool checkPassword(std::string userName, std::string password);

	//! A function that sets the loggedIn boolean and sets the currentUser
	//! to the given user.
	void login(std::string userName);
	//! A function that sets the loggedIn boolean to false and resets the currentUser.
	void logout();
	//! A function that returns true if a user is logged in.
	bool getLoggedIn();
	//! A function that returns the password of the currently active user.
	std::string getCurrentUserPassword();
	//! A function that sets the password of the currently active user 
	//! to a new given password
	void changeCurrentUserPassword(std::string password);
	//! Gives the task access to the outside world.
	void setWebsocket(washingMachineWS* out);
	//! Receives a packet.
	void packet_received(Packet p);

protected:
		//because the Task interface demands it, and because this task needs to do
		//things.
		void main(void);

private:
	//! A function that searches if a user exists using a given userName 
	//! and if so it will return this user.
	User findUser(std::string userName);

	RTOS::pool<MachineState> machineStatePool;
	RTOS::flag stateUpdateFlag;
	RTOS::pool<CycleStep> cycleStatePool;

	MachineState currentState;
	CycleStep currentCycleStep;
	User currentUser = {"", ""};
	std::vector<User> users;
	washingCycleTask WCT;
	washingMachineWS* mySock;
	WebsocketController* webcon;
	
	bool loggedIn = false;
	
};



#endif
