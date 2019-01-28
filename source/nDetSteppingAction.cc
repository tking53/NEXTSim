//
// $Id: nDetSteppingAction.hh,v 1.0 Sept., 2015 $
// by Dr. Xiaodong Zhang
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <vector>

#include "G4Step.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpAbsorption.hh"
#include "G4ProcessManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4Alpha.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Triton.hh"
#include "G4EventManager.hh"

#include "nDetSteppingAction.hh"
#include "nDetConstruction.hh"
#include "nDetRunAction.hh"
#include "nDetEventAction.hh"

#define DEBUG 0

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetSteppingAction::nDetSteppingAction(nDetConstruction* det, nDetRunAction* runAct, nDetEventAction* evtAct) : detector(det), runAction(runAct), evtAction(evtAct)
{
	neutronTrack = false;
	eventID = -1;
	stepID = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetSteppingAction::~nDetSteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetSteppingAction::UserSteppingAction(const G4Step* aStep)
{
	G4Track *track = aStep->GetTrack();

	if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary){ // Check if the particle is at a geometry boundary.
		G4String vName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
		if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
			if(vName.find("psSiPM") != std::string::npos){
				detector->AddDetectedPhoton(aStep);
			}
		}
		else{
			if(neutronTrack){
				if(track->GetTrackID() != 1)
					neutronTrack = false;
				else if(vName == "expHall_physV"){ // Exiting the detector.
					runAction->finalizeNeutron(aStep);
					neutronTrack = false;
				}
				// Else do nothing (the particle is probably traversing the inner wrapping layers).
			}
			else if(track->GetTrackID() == 1 && aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName().find("Scint") != std::string::npos){ // Enter the material.
				runAction->initializeNeutron(aStep);
				neutronTrack = true;
			}
		}
	}
	else{ // Normal scattering event.
		if(neutronTrack){
			if(track->GetTrackID() != 1)
				neutronTrack = false;
			else
				runAction->scatterNeutron(aStep);
		}
	}
}

void nDetSteppingAction::Reset(){
	neutronTrack = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
