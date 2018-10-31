//
// Created by David Pérez Loureiro on 4/4/17.
//

#include <cmath>

#include "nDetTrackingAction.hh"
#include "nDetRunAction.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4TrackingManager.hh"
#include "nDetUserTrackingInformation.hh"

nDetTrackingAction::nDetTrackingAction(nDetRunAction *run) : runAction(run) {
  G4cout<<"nDetTrackingAction::nDetTrackingAction()"<<G4endl;
  fAnlManager = static_cast<nDetAnalysisManager*> (nDetAnalysisManager::Instance());
  prevTrackID = 0;
}
nDetTrackingAction::~nDetTrackingAction(){}

void nDetTrackingAction::PreUserTrackingAction(const G4Track* aTrack){
  fpTrackingManager->SetUserTrackInformation(new nDetUserTrackingInformation);
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) { // Photon track
  }
  else if(aTrack->GetParentID() == 1){ // Scatter event
    if(aTrack->GetTrackID() != prevTrackID){ // Skip cascades. We're only concerned with primary scattering
      runAction->scatterEvent(aTrack);
      prevTrackID = aTrack->GetTrackID();
    }
  }
  //else{ // Primary particle (neutron)
    //G4cout<<"pri tracking: "<< aTrack->GetDefinition()->GetParticleName() << ", E=" << aTrack->GetKineticEnergy() << ", pnt=" << aTrack->GetParentID() << ", track=" << aTrack->GetTrackID() << G4endl;
  //}
}

void nDetTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

	//if(aTrack->GetDefinition()->GetParticleName() != "opticalphoton")
	    //G4cout<<"nDetTrackingAction::PostUserTrackingAction() "<<aTrack->GetDefinition()->GetParticleName() <<G4endl;
    //if (aTrack->GetTrackLength() > 0) {
    //G4cout << "Track # " << aTrack->GetTrackID() << " length is " << aTrack->GetTrackLength() << G4endl;

    if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
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
