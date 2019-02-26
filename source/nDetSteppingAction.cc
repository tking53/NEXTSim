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
	if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){ // Check for detected optical photons.
		if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName().find("psSiPM") != std::string::npos)
			detector->AddDetectedPhoton(aStep);
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
	else if(track->GetTrackID() == 1 && aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName().find("Scint") != std::string::npos){ // Enter the material.
		runAction->initializeNeutron(aStep);
		neutronTrack = true;
	}
}

void nDetSteppingAction::Reset(){
	neutronTrack = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
