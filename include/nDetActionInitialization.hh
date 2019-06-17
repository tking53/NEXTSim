#ifndef NDET_ACTION_INITIALIZATION_HH
#define NDET_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class nDetRunAction;
class nDetEventAction;
class nDetSteppingAction;
class nDetStackingAction;
class nDetTrackingAction;

class nDetActionInitialization;

/*! \class userActionManager
 *  \brief Container for a thread's user actions
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date June 14, 2019
*/

class userActionManager{
  public:
	userActionManager(const nDetActionInitialization* init, bool verboseMode=false);
	
	int getThreadID() const { return threadID; }
	
	nDetRunAction* getRunAction(){ return runAction; }
	
	nDetEventAction* getEventAction(){ return eventAction; }
	
	nDetSteppingAction* getSteppingAction(){ return steppingAction; }
	
	nDetStackingAction* getStackingAction(){ return stackingAction; }
	
	nDetTrackingAction* getTrackingAction(){ return trackingAction; }	

	const nDetActionInitialization* getActionInit(){ return actionInit; }

  private:
	int threadID;
	
	nDetRunAction* runAction;
	nDetEventAction* eventAction;
	nDetSteppingAction* steppingAction;
	nDetStackingAction* stackingAction;
	nDetTrackingAction* trackingAction;
	
	const nDetActionInitialization* actionInit;
};

/*! \class nDetActionInitialization
 *  \brief Used to set the user actions for worker and master threads
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
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
