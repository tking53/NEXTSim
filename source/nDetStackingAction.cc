//
// $Id: nDetStackingAction.cc, v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang 
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "nDetStackingAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetStackingAction::nDetStackingAction(nDetRunAction* run) : runAct(run)
{
  numPhotonsProduced = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetStackingAction::~nDetStackingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack nDetStackingAction::ClassifyNewTrack(const G4Track * aTrack) {
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) { // particle is optical photon
    numPhotonsProduced++;
    counter.addPhoton(aTrack->GetParentID());
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetStackingAction::NewStage()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetStackingAction::PrepareNewEvent()
{
}

void nDetStackingAction::Reset(){
  numPhotonsProduced = 0;
  counter.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
