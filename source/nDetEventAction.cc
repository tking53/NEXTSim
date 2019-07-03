#include "nDetEventAction.hh"
#include "nDetRunAction.hh"

#include "G4Event.hh"
 
nDetEventAction::nDetEventAction(nDetRunAction* run) : runAct(run) { }
 
nDetEventAction::~nDetEventAction(){ 
}

void nDetEventAction::BeginOfEventAction(const G4Event* evt){
	runAct->setEventNumber(evt->GetEventID());
}

void nDetEventAction::EndOfEventAction(const G4Event*){
	// Process primary scatters. 
	runAct->process();
}
