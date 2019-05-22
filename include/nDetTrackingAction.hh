#ifndef NDET_TRACKING_ACTION_HH
#define NDET_TRACKING_ACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4Types.hh"

class nDetRunAction;

class nDetTrackingAction: public G4UserTrackingAction {

public:
    nDetTrackingAction(nDetRunAction *run);
    
    ~nDetTrackingAction();
    
    void PreUserTrackingAction(const G4Track* aTrack);
    
    void PostUserTrackingAction(const G4Track*){ }

    void Reset(){ }

private:
    nDetRunAction* runAction; ///< Pointer to the thread-local user run action
};

#endif
