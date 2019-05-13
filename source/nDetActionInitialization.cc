
#include "G4MTRunManager.hh"

#include "nDetActionInitialization.hh"
#include "nDetConstruction.hh"

#include "nDetRunAction.hh"
#include "nDetEventAction.hh"
#include "nDetStackingAction.hh"
#include "nDetSteppingAction.hh"
#include "nDetTrackingAction.hh"

nDetActionInitialization::nDetActionInitialization(bool verboseMode/*=false*/) : runAction(NULL), eventAction(NULL), steppingAction(NULL), stackingAction(NULL), trackingAction(NULL), masterRunManager(NULL) {
	// Define all actions.
	runAction = new nDetRunAction();
	eventAction = new nDetEventAction(runAction);
	steppingAction = new nDetSteppingAction(runAction, eventAction);
	stackingAction = new nDetStackingAction(runAction);
	trackingAction = new nDetTrackingAction(runAction);	

	if(verboseMode) runAction->toggleVerboseMode();
	
	// Link all actions back to run control thread.
	runAction->setActions(eventAction, stackingAction, trackingAction, steppingAction);
	
	masterRunManager = G4MTRunManager::GetMasterRunManager();
}

void nDetActionInitialization::Build() const {
	this->SetUserAction(runAction);
	this->SetUserAction((G4VUserPrimaryGeneratorAction*)runAction->getSource());	
	this->SetUserAction(eventAction);
	this->SetUserAction(steppingAction);
	this->SetUserAction(stackingAction);
	this->SetUserAction(trackingAction);
}
	
void nDetActionInitialization::BuildForMaster() const {
	this->SetUserAction(runAction);
}
