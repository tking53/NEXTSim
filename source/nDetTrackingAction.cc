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

nDetTrackingAction::nDetTrackingAction(nDetRunAction *run) : runAction(run) {
  prevTrackID = 0;
}

nDetTrackingAction::~nDetTrackingAction(){}

void nDetTrackingAction::PreUserTrackingAction(const G4Track* aTrack){
}
