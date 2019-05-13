#include "nDetEventAction.hh"

#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::nDetEventAction(nDetRunAction* run) : runAct(run) { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::~nDetEventAction(){ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetEventAction::BeginOfEventAction(const G4Event* evt){
	runAct->setEventNumber(evt->GetEventID());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::EndOfEventAction(const G4Event*){
	// Process primary scatters. 
	runAct->process();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
