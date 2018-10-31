//
// Created by David Pérez Loureiro on 4/4/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH
#define NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "nDetAnalysisManager.hh"

class nDetRunAction;

class nDetTrackingAction: public G4UserTrackingAction {

public:
    nDetTrackingAction(nDetRunAction *run);
    ~nDetTrackingAction();
    void PreUserTrackingAction(const G4Track* aTrack);
    void PostUserTrackingAction(const G4Track* aTrack);

    void Reset(){ prevTrackID = 0; }

private:
    nDetRunAction* runAction;
    nDetAnalysisManager *fAnlManager;

    G4int prevTrackID;
};


#endif //NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH
