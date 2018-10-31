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
#include "nDetAnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetStackingAction::nDetStackingAction(nDetRunAction* run) : runAct(run)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetStackingAction::~nDetStackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack nDetStackingAction::ClassifyNewTrack(const G4Track * aTrack) {
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) { // particle is optical photon
    numPhotonsProduced++;
    if (aTrack->GetParentID() > 0){  // primary particle, neutron, ID =0, then either proton, or others, their IDs are larger than 0, and then photons
      if (aTrack->GetVolume()->GetName().find("ej200")) { // particle is secondary and happens in the EJ200 scintillator
        //std::cout<<aTrack->GetVolume()->GetName()<<"....in ej200..."<<aTrack->GetGlobalTime()<<"..."<<aTrack->GetDynamicParticle()->GetDefinition()->GetParticleName()<<"...."<<aTrack->GetPosition()<<std::endl;
        runAct->vTimeOfPhotonInEJ200.push_back(aTrack->GetGlobalTime());
        runAct->vPrimaryPhotonPositionX.push_back(aTrack->GetPosition().x());
        runAct->vPrimaryPhotonPositionY.push_back(aTrack->GetPosition().y());
        runAct->vPrimaryPhotonPositionZ.push_back(aTrack->GetPosition().z());

//Kyle adding these lines:
        //runAct->particleName.push_back(aTrack->GetDefinition()->GetParticleName());
        //runAct->particleName.push_back(aTrack->GetParticleDefinition()->GetParticleName());
        // runAct->particleCharge.push_back(aTrack->GetDefinition()->GetPDGCharge());
        //runAct->particleCharge.push_back(aTrack->GetParticleDefinition()->GetPDGCharge());
//end

      }

      nDetAnalysisManager *theManager = (nDetAnalysisManager *) nDetAnalysisManager::Instance();
      if (theManager) {
        //G4cout<<"nDetStackingAction::ClassifyNewTrack()-->"<<G4endl;
        theManager->ClassifyNewTrack(aTrack);
      }
    }
  }
  /*else{
    G4cout<<"stacking: "<< aTrack->GetDefinition()->GetParticleName() << ", E=" << aTrack->GetKineticEnergy() << ", pnt=" << aTrack->GetParentID() << ", track=" << aTrack->GetTrackID() << G4endl;
  }*/
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetStackingAction::NewStage()
{
  //G4cout << "nDetStackingAction::NewStage()->No. optical photons produces in this event : "
         //<< photonNb << G4endl;



}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetStackingAction::PrepareNewEvent()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
