#ifndef NDET_STACKING_ACTION_HH
#define NDET_STACKING_ACTION_HH

#include "globals.hh"
#include "G4UserStackingAction.hh"

#include "nDetEventAction.hh"
#include "nDetRunAction.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "photonCounter.hh"

class nDetStackingAction : public G4UserStackingAction
{
  public:
    nDetStackingAction(nDetRunAction*);

   ~nDetStackingAction();

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    
    G4int GetNumPhotonsProduced() const { return numPhotonsProduced; }

	photonCounter *GetCounter(){ return &counter; }

    void NewStage();

    void PrepareNewEvent();

	void Reset();

  private:
    nDetRunAction* runAct; ///< Pointer to the thread-local user run action
    
    G4int numPhotonsProduced; ///< Total number of optical photon tracks generated

    photonCounter counter; ///< Object used to count the number of optical photon tracks for an event
};

#endif
