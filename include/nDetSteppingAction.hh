#ifndef NDET_STEPPING_ACTION_HH
#define NDET_STEPPING_ACTION_HH

#include "G4UserSteppingAction.hh"

class nDetConstruction;
class nDetRunAction;

/** @class nDetSteppingAction
  * @brief Performs actions which take place at the start and end of processing of a particle step
  * @date August 9, 2019
  */

class nDetSteppingAction : public G4UserSteppingAction {
  public:
	/** Constructor taking a pointer to a user run action
	  */
	nDetSteppingAction(nDetRunAction*);

	/** Destructor
	  */
	virtual ~nDetSteppingAction();

	/** Perform processing on a particle step
	  */
	void UserSteppingAction(const G4Step*);

	/** Reset all class parameters
	  */
	void Reset();

  private:
	nDetRunAction* runAction; ///< Pointer to the thread-local user run action

	bool neutronTrack; ///< Flag indicating that a primary particle is being tracked
};

#endif

