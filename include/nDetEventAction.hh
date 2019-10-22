#ifndef NDET_EVENT_ACTION_HH
#define NDET_EVENT_ACTION_HH

#include "G4UserEventAction.hh"

class nDetRunAction;
class G4Event;

/** @class nDetEventAction
  * @brief Performs actions which take place at the start and end of a primary particle event
  * @date August 9, 2019
  */

class nDetEventAction : public G4UserEventAction {
  public:
	/** Constructor taking a pointer to a user run action
	  */
	nDetEventAction(nDetRunAction*);
	
	/** Destructor
	  */
	virtual ~nDetEventAction();

  public:
	/** Action to perform at the beginning of each primary event
	  */
	void BeginOfEventAction(const G4Event*);

	/** Action to perform at the end of each primary event
	  */	
	void EndOfEventAction(const G4Event*);

  private:
	nDetRunAction *runAct; ///< Pointer to the thread-local user run action
};

#endif
