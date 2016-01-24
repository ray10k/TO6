//**************************************
//! Task responsible for communication between the websocket and
//! saved data like washingCycles and users.
//! \authors
//!		- Wilco Louwerse
//!
//! \context
//!		- part of TO6 assignment 2015-2016
//**************************************

#ifndef __DISPLAY_TASK
#define __DISPLAY_TASK

#include "prtos/pRTOS.h"
#include "cycleState.h"
#include "loadCycleTask.h"
#include "washingCycleTask.h"
#include "cycleStateListener.h"
#include "machineStateListener.h"
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
	int totalSteps = -1;
	int currentStep = -1;
	int cycleState = -1;		//RUN=0, PAUSE=1, STOP=2
	std::string cycleName = "";
	std::string stepName = "";
	bool finished = false;		//Is true when the washingCycle finished correctly
								//and wasn't forced to stop.
};

class displayTask : public RTOS::task, public machineStateListener,
						   public cycleStateListener
{
	//! Constructor.
	displayTask(washingCycleTask* WCT, loadCycleTask* LCT);
	//! An override function from machineStateListener.h.
	void stateChanged(machineState currentState) override;
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

	//The following functions are all used by the websocket to get information that
	//this task will provide.
	//! A function used to set the state of the current washingCycle, see cycleState.h.
	void setCycleState(int state);
	//!
	void loadWashingCycle(std::string userName, std::string washingCycleName);
	std::vector<std::string> loadWashingCycleNames();
	int getTotalCycleSteps(std::string washingCycleName)

	void addUser(User user);
	bool checkUserName(std::string userName);
	bool checkPassword(std::string userName, std::string password);

	void login(std::string userName);
	void logout();
	bool getLoggedIn();
	std::string getCurrentUserPassword();
	void changeCurrentUserPassword(std::string password);

	protected:
		main(void);

	private:
	User findUser(std::string userName);

	RTOS::channel<machineState,1> machineStateChannel;
	RTOS::channel<CycleStep,8> washingCycleStateChannel;
	washingCycleTask WCT;
	loadCycleTask LCT;

	MachineState currentState;
	CycleStep currentCycleStep;
	User currentUser = {"", ""};
	std::vector<User&> users;
	bool loggedIn = false;
};



#endif
