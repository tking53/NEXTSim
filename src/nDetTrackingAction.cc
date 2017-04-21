//
// Created by David Pérez Loureiro on 4/4/17.
//

#include "nDetTrackingAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"


nDetTrackingAction::nDetTrackingAction(){

    G4cout<<"nDetTrackingAction::nDetTrackingAction()"<<G4endl;
    fAnlManager = static_cast<nDetAnalysisManager*> (nDetAnalysisManager::Instance());

}
nDetTrackingAction::~nDetTrackingAction(){}
void nDetTrackingAction::PreUserTrackingAction(const G4Track* aTrack){
;
}
void nDetTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

    //G4cout<<"nDetTrackingAction::PostUserTrackingAction()"<<G4endl;
    //if (aTrack->GetTrackLength() > 0) {
    //    G4cout << "Track # " << aTrack->GetTrackID() << " length is " << aTrack->GetTrackLength() << G4endl;

    //}

    if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
;
        //if (fAnlManager)
            //fAnlManager->PostUserTrackingAction(aTrack);

    }
}
