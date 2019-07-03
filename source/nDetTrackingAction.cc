#include "nDetTrackingAction.hh"

nDetTrackingAction::nDetTrackingAction(nDetRunAction *run) : runAction(run) {
}

nDetTrackingAction::~nDetTrackingAction(){
}

void nDetTrackingAction::PreUserTrackingAction(const G4Track*){
}
