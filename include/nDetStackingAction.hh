#ifndef NDET_STACKING_ACTION_HH
#define NDET_STACKING_ACTION_HH

#include "globals.hh"
#include "G4UserStackingAction.hh"

#include "nDetEventAction.hh"
#include "nDetRunAction.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "photonCounter.hh"

/** @class nDetStackingAction
  * @brief Gives hooks to G4StackManager which Controls stacks of particle tracks
  * @date August 9, 2019
  */

class nDetStackingAction : public G4UserStackingAction {
  public:
	/** Constructor taking a pointer to a user run action
	  */
	nDetStackingAction(nDetRunAction*);

	/** Destructor
	  */
	~nDetStackingAction(){ }

	/** Classify a new particle track for the stack manager. If the track is
	  * an optical photon, add it to the photon counter
	  */
	G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);

	/** Get the total number of optical photon tracks produced
	  */
	G4int GetNumPhotonsProduced() const { return numPhotonsProduced; }

	/** Get a pointer to the optical photon track counter
	  */
	photonCounter *GetCounter(){ return &counter; }

	/** Called by G4StackManager when the urgent stack is empty and contents 
	  * of the waiting stack are transferred to the urgent stack
	  * @note Currently does nothing
	  */
	void NewStage(){ }

	/** Called by G4StackManager at the beginning of each primary event
	  * @note Currently does nothing
	  */
	void PrepareNewEvent(){ }

	/** Reset all class parameters
	  */
	void Reset();

  private:
	nDetRunAction* runAct; ///< Pointer to the thread-local user run action

	G4int numPhotonsProduced; ///< Total number of optical photon tracks generated

	photonCounter counter; ///< Object used to count the number of optical photon tracks for an event
};

#endif
