//
// Created by David Pérez Loureiro on 4/4/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH
#define NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4Types.hh"

class nDetRunAction;

class nDetTrackingAction: public G4UserTrackingAction {

public:
    nDetTrackingAction(nDetRunAction *run);
    
    ~nDetTrackingAction();
    
    void PreUserTrackingAction(const G4Track* aTrack);
    
    void PostUserTrackingAction(const G4Track*){ }

    void Reset(){ prevTrackID = 0; }

private:
    nDetRunAction* runAction;

    G4int prevTrackID;
};


#endif //NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH
