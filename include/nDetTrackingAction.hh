//
// Created by David Pérez Loureiro on 4/4/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH
#define NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "nDetAnalysisManager.hh"

class nDetTrackingAction: public G4UserTrackingAction {

public:

    nDetTrackingAction();
    ~nDetTrackingAction();
    void PreUserTrackingAction(const G4Track* aTrack);
    void PostUserTrackingAction(const G4Track* aTrack);

private:

    nDetAnalysisManager *fAnlManager;

};


#endif //NEXTMULTIPLESCATTERING_NDETTRACKINGACTION_HH
