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
  if(neutronTrack){
    if(aStep->GetTrack()->GetTrackID() != 1) 
      neutronTrack = false;
    else if(aStep->GetPreStepPoint()->GetMaterial()->GetName() == "G4_AIR"){ // Escape
      runAction->finalizeNeutron(aStep);
      neutronTrack = false;
    }
    else // Normal scatter
      runAction->scatterNeutron(aStep);
  }
  else if(aStep->GetTrack()->GetTrackID() == 1){ // Enter the material.
    //if(!aStep->GetPostStepPoint()->GetMaterial()) // Unknown event (scatter in air)
      //std::cout << " STRT id=" << aStep->GetTrack()->GetTrackID() << " from " << aStep->GetPreStepPoint()->GetMaterial()->GetName() << " into NONE!!!\n";
    runAction->initializeNeutron(aStep);
    neutronTrack = true;
  }

  // Simplifying this process to try and alleviate problems (CRT)
  if(aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
    if (aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
      G4String vName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
      if(vName.find("psSiPM") != std::string::npos)
        detector->AddDetectedPhoton(aStep);
    }
  }
}

void nDetSteppingAction::Reset(){
  neutronTrack = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
