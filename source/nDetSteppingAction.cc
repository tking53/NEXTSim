#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"

#include "nDetSteppingAction.hh"
#include "nDetConstruction.hh"
#include "nDetRunAction.hh"

nDetSteppingAction::nDetSteppingAction(nDetRunAction* runAct) : runAction(runAct) {
	neutronTrack = false;
}

nDetSteppingAction::~nDetSteppingAction(){ }

void nDetSteppingAction::UserSteppingAction(const G4Step* aStep){
	G4Track *track = aStep->GetTrack();
	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){ // Check for detected optical photons.
		if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName().find("psSiPM") != std::string::npos)
			runAction->AddDetectedPhoton(aStep);
	}
	else if(track->GetTrackStatus() != fAlive) return;
	else if(neutronTrack){ // Normal scattering event.
		if(track->GetTrackID() != 1)
			neutronTrack = false;
		else if(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "expHall_physV"){ // Exiting the detector.
			runAction->finalizeNeutron(aStep);
			neutronTrack = false;
			return;
		}
		else
			runAction->scatterNeutron(aStep);
	}
	else if(track->GetTrackID() == 1){ // This is a primary particle scatter event.
		runAction->initializeNeutron(aStep);
		neutronTrack = true;
	}
}

void nDetSteppingAction::Reset(){
	neutronTrack = false;
}
