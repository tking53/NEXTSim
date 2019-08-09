#ifndef NDET_TRACKING_ACTION_HH
#define NDET_TRACKING_ACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4Types.hh"

class nDetRunAction;

/** @class nDetTrackingAction
  * @brief Performs actions which take place at the start and end of processing of a particle track
  * @date August 9, 2019
  */

class nDetTrackingAction: public G4UserTrackingAction {
	public:
	/** Constructor taking a pointer to a user run action
	  */
	nDetTrackingAction(nDetRunAction *run);

	/** Destructor
	  */    
	~nDetTrackingAction(){ }

	/** Action to perform before starting processing of a particle track
	  * @note Currently does nothing
	  */
	void PreUserTrackingAction(const G4Track*){ }

	/** Action to perform after a particle track has been processed
	  * @note Currently does nothing
	  */
	void PostUserTrackingAction(const G4Track*){ }

	/** Reset all class parameters
	  */
	void Reset(){ }

	private:
	nDetRunAction* runAction; ///< Pointer to the thread-local user run action
};

#endif
