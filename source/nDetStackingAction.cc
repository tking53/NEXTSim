#include "nDetStackingAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"

nDetStackingAction::nDetStackingAction(nDetRunAction* run) : runAct(run) {
	numPhotonsProduced = 0;
}

nDetStackingAction::~nDetStackingAction(){
}

G4ClassificationOfNewTrack nDetStackingAction::ClassifyNewTrack(const G4Track * aTrack) {
	if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) { // Particle is an optical photon
		numPhotonsProduced++;
		counter.addPhoton(aTrack->GetParentID());
	}
	return fUrgent;
}

void nDetStackingAction::NewStage(){
}

void nDetStackingAction::PrepareNewEvent(){
}

void nDetStackingAction::Reset(){
	numPhotonsProduced = 0;
	counter.clear();
}
