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

	//The following functions are all used by the websocket to get information 
	//that this task will provide.
	//! A function used to set the state of the current washingCycle, see 
	//! cycleState.h.
	void setCycleState(cycleState state);
	//! Attempts to load a given cycle. Can fail, and will not indicate when
	//! this happens.
	void loadCycle(std::string userName, std::string washingCycleName);
	//! Fetches a list of *all* washing cycle names.
	std::vector<std::string> loadWashingCycleNames();
	//! Returns the total number of steps for the given cycle.
	int getTotalCycleSteps(cycleID id);

	
	void addUser(User user);
	//! Returns true if the given userName is known within the system.
	bool checkUserName(std::string userName);
	//! Returns true if the given name/pass are a valid login combination.
	bool checkPassword(std::string userName, std::string password);
	
	//! ?
	void login(std::string userName);
	//! !
	void logout();
	//! Returns true if anyone is currently logged in.
	bool getLoggedIn();
	//! :P
	std::string getCurrentUserPassword();
	//! Changes the registered password of whoever is currently logged in.
	void changeCurrentUserPassword(std::string password);

protected:
		void main(void);

private:
	User findUser(std::string userName);

	RTOS::pool<MachineState> machineStatePool;
	RTOS::flag stateUpdateFlag;
	RTOS::pool<CycleStep> cycleStatePool;

	MachineState currentState;
	CycleStep currentCycleStep;
	User currentUser = {"", ""};
	std::vector<User> users;
	washingCycleTask WCT;
	bool loggedIn = false;
};



#endif
