//**************************************
//! Task responsible for ...
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

struct User
{
	std::string userName;
	std::string password;
}

struct CycleStep
{
	int totalSteps = -1;
	int currentStep = -1;
	int cycleState = -1;
	std::string cycleName = "";
	std::string stepName = "";
	bool finished = false;
}

public class displayTask : public RTOS::Task, public machineStateListener, 
						   public cycleStateListener
{
	//! Constructor.
	displayTask(washingCycleTask* WCT, loadCycleTask* LCT);
	//! See machineStateListener.h
	void stateChanged(machineState currentState) override;
	//! see cycleStateListener.h
	void cycleStateChanged(unsigned int totalSteps,
		unsigned int currentStep,
		const std::string& cycleName,
		const std::string& stepName) override;
	//! see cycleStateListener.h
	void cyclePaused(const std::string& cycleName,
		const std::string& stepName) override;
	//! see cycleStateListener.h
	void cycleEnded(bool finished,
		const std::string& cycleName,
		const std::string& stepName) override;
	//! TODO: document this.
	void setCycleState(int state);
	//! TODO: document this.
	
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
