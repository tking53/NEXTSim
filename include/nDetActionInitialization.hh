#ifndef NDET_ACTION_INITIALIZATION_HH
#define NDET_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class nDetRunAction;
class nDetEventAction;
class nDetSteppingAction;
class nDetStackingAction;
class nDetTrackingAction;

class nDetActionInitialization;

/** @class userActionManager
  * @brief Container for a thread's user actions
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date June 17, 2019
  */

class userActionManager{
  public:
	/** Constructor
	  * @param init Pointer to the action initialization
	  * @param verboseMode Verbosity flag
	  */
	userActionManager(const nDetActionInitialization* init, bool verboseMode=false);

	/** Get the Geant thread ID number
	  */	
	int getThreadID() const { return threadID; }

	/** Get a pointer to the run action for this thread
	  */	
	nDetRunAction* getRunAction(){ return runAction; }

	/** Get a pointer to the event action for this thread
	  */	
	nDetEventAction* getEventAction(){ return eventAction; }

	/** Get a pointer to the stepping action for this thread
	  */	
	nDetSteppingAction* getSteppingAction(){ return steppingAction; }

	/** Get a pointer to the stacking action for this thread
	  */	
	nDetStackingAction* getStackingAction(){ return stackingAction; }

	/** Get a pointer to the tracking action for this thread
	  */	
	nDetTrackingAction* getTrackingAction(){ return trackingAction; }	

	/** Get a pointer to the action initialization for this thread
	  */
	const nDetActionInitialization* getActionInit(){ return actionInit; }

  private:
	int threadID; ///< Geant thread ID number
	
	nDetRunAction* runAction; ///< Pointer to the run action for this thread
	nDetEventAction* eventAction; ///< Pointer to the event action for this thread
	nDetSteppingAction* steppingAction; ///< Pointer to the stepping action for this thread
	nDetStackingAction* stackingAction; ///< Pointer to the stacking action for this thread
	nDetTrackingAction* trackingAction; ///< Pointer to the tracking action for this thread
	
	const nDetActionInitialization* actionInit; ///< Pointer to the action initialization
};

/** @class nDetActionInitialization
  * @brief Used to set the user actions for worker and master threads
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date May 9, 2019
  */

class nDetActionInitialization : public G4VUserActionInitialization {
  public:
	/** Default constructor
	  * @param verboseMode Set the verbosity flag
	  */
	nDetActionInitialization(bool verboseMode=false);

	/** Set user actions for the worker threads. Called from G4RunManager::SetUserInitialization()
	  */
	virtual void Build() const ;
	
	/** Set user actions for the master thread. Called from G4MTRunManager::SetUserInitialization()
	  */
	virtual void BuildForMaster() const ;

  private:
	bool verbose; ///< Verbosity flag
};

#endif
