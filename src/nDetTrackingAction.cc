//
// Created by David Pérez Loureiro on 4/4/17.
//

#include "nDetTrackingAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4TrackingManager.hh"
#include "nDetUserTrackingInformation.hh"

nDetTrackingAction::nDetTrackingAction(){

    G4cout<<"nDetTrackingAction::nDetTrackingAction()"<<G4endl;
    fAnlManager = static_cast<nDetAnalysisManager*> (nDetAnalysisManager::Instance());

}
nDetTrackingAction::~nDetTrackingAction(){}

void nDetTrackingAction::PreUserTrackingAction(const G4Track* aTrack){

    fpTrackingManager->SetUserTrackInformation(new nDetUserTrackingInformation);
}
void nDetTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

    //G4cout<<"nDetTrackingAction::PostUserTrackingAction() "<<aTrack->GetDefinition()->GetParticleName() <<G4endl;
    //if (aTrack->GetTrackLength() > 0) {
    //G4cout << "Track # " << aTrack->GetTrackID() << " length is " << aTrack->GetTrackLength() << G4endl;


    if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
;
        if (fAnlManager)
            fAnlManager->PostUserTrackingAction(aTrack);
        nDetUserTrackingInformation  *theInfo =(nDetUserTrackingInformation*)aTrack->GetUserInformation();
        G4int nReflections=theInfo->GetReflectionCount();
        G4int nDetections=theInfo->GetDetectionCount();
        G4int nAbsortions=theInfo->GetAbsortionCount();
        //}
        //G4cout << "Track # " << aTrack->GetTrackID() << " length is " << aTrack->GetTrackLength() << G4endl;

        if(nAbsortions<0 && fpTrackingManager->GetVerboseLevel()>1){
            G4cout << "Track # " << aTrack->GetTrackID() << " #OfReflections is " << nReflections << G4endl;
            G4cout << "Track # " << aTrack->GetTrackID() << " #OfAbsortions is " << nAbsortions << G4endl;
        }

        if(nDetections >0 && fpTrackingManager->GetVerboseLevel()>1) {
            G4cout << "Track # " << aTrack->GetTrackID() << " #OfReflections is " << nReflections << G4endl;
            G4cout << "Track # " << aTrack->GetTrackID() << " #OfDetections is " << nDetections << G4endl;
        }
    }
}
