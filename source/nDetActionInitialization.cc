
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4Threading.hh"

#include "nDetActionInitialization.hh"
#include "nDetConstruction.hh"

#include "nDetParticleSource.hh"
#include "nDetRunAction.hh"
#include "nDetEventAction.hh"
#include "nDetStackingAction.hh"
#include "nDetSteppingAction.hh"
#include "nDetTrackingAction.hh"
#include "nDetThreadContainer.hh"

userActionManager::userActionManager(const nDetActionInitialization* init, bool verboseMode/*=false*/) : threadID(0), runAction(NULL), eventAction(NULL), steppingAction(NULL), stackingAction(NULL), trackingAction(NULL), actionInit(init) {
	threadID = G4Threading::G4GetThreadId();

	// Define all user actions.
	runAction = new nDetRunAction();
	eventAction = new nDetEventAction(runAction);
	steppingAction = new nDetSteppingAction(runAction);
	stackingAction = new nDetStackingAction(runAction);
	trackingAction = new nDetTrackingAction(runAction);
	
	// Set verbose mode
	if(verboseMode) runAction->toggleVerboseMode();
	
	// Link all actions back to run control thread
	runAction->setActions(this);
}

nDetActionInitialization::nDetActionInitialization(bool verboseMode/*=false*/) : verbose(verboseMode) { 
}

void nDetActionInitialization::Build() const {
	userActionManager manager(this, verbose);

	// Set pointers to all user actions
	SetUserAction(manager.getRunAction());
	SetUserAction(manager.getEventAction());
	SetUserAction(manager.getSteppingAction());
	SetUserAction(manager.getStackingAction());
	SetUserAction(manager.getTrackingAction());
	SetUserAction(new nDetPrimaryGeneratorAction());

	// Add this thread to the list of all threads
	nDetThreadContainer::getInstance().addAction(manager);
}
	
void nDetActionInitialization::BuildForMaster() const {
	nDetRunAction *runAction = new nDetRunAction();
	this->SetUserAction(runAction);
	
	// Add the master thread to the list of all threads
	nDetThreadContainer::getInstance().setMaster(runAction);
}
