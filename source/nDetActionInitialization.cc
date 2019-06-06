
#include <iostream>

#include "G4MTRunManager.hh"
#include "G4Threading.hh"

#include "nDetActionInitialization.hh"
#include "nDetConstruction.hh"

#include "nDetRunAction.hh"
#include "nDetEventAction.hh"
#include "nDetStackingAction.hh"
#include "nDetSteppingAction.hh"
#include "nDetTrackingAction.hh"
#include "nDetThreadContainer.hh"

nDetActionInitialization::nDetActionInitialization(bool verboseMode/*=false*/){ 
	verbose = verboseMode;
}

void nDetActionInitialization::Build() const {
	// Define all user actions.
	nDetRunAction *runAction = new nDetRunAction();
	nDetEventAction *eventAction = new nDetEventAction(runAction);
	nDetSteppingAction *steppingAction = new nDetSteppingAction(runAction);
	nDetStackingAction *stackingAction = new nDetStackingAction(runAction);
	nDetTrackingAction *trackingAction = new nDetTrackingAction(runAction);

	if(verbose) runAction->toggleVerboseMode();

	this->SetUserAction(runAction);
	this->SetUserAction((G4VUserPrimaryGeneratorAction*)runAction->getParticleSource());	
	this->SetUserAction(eventAction);
	this->SetUserAction(steppingAction);
	this->SetUserAction(stackingAction);
	this->SetUserAction(trackingAction);

	// Link all actions back to run control thread.
	runAction->setActions(eventAction, stackingAction, trackingAction, steppingAction);

	// Add this thread to the 
	nDetThreadContainer::getInstance().addAction(runAction, G4Threading::G4GetThreadId());
}
	
void nDetActionInitialization::BuildForMaster() const {
	this->SetUserAction(new nDetRunAction());
}
