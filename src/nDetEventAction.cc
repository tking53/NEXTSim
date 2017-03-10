//
// $Id: nDetEventAction.cc, v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang 
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
#include "nDetEventAction.hh"
#include "nDetHits.hh"
#include "nDetAnalysisManager.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"


#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "nDetSD.hh"
#include "SiPMSD.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::nDetEventAction(nDetRunAction* run)
:runAct(run)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::~nDetEventAction() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::BeginOfEventAction(const G4Event* evt)
{
  // initialisation per event.
  eventID = evt->GetEventID();
  depositedEnergy = 0; 

  if(eventID%1000 == 0) G4cout<<"Event ID: " << eventID << G4endl;
  // clear vector for next event
  runAct->vectorClear();

  fAnalysisManager=(nDetAnalysisManager*)nDetAnalysisManager::Instance();

  if(fAnalysisManager)
  fAnalysisManager->BeginOfEventAction(evt);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::EndOfEventAction(const G4Event* evt)
{

  // hit collection of event
  //G4int NbHits = (evt->GetHCofThisEvent())->GetNumberOfCollections();

//G4cout<< "Number of Collections "<<NbHits<<G4endl;

  //nDetHit* hit;
  //for(G4int i=0;i<NbHits;i++){
    // output the size of each hits collection
   
    //G4int photonNbSD += ((evt->GetHCofThisEvent())->GetHC(i))->GetSize();
    
     //if(photonNbSD>2){
     // G4cout<<"Event ID: " << eventID << G4endl;
     // G4cout<< "Hit number of Hits collection " << i << " is :"
	//<<((evt->GetHCofThisEvent())->GetHC(i))->GetSize() <<G4endl;
    //}
    /*for(G4int j=0; j<((evt->GetHCofThisEvent())->GetHC(i))->GetSize(); j++){
      // print all of hits in the hits collection
      //evt->GetHCofThisEvent()->GetHC(i)->GetHit(j)->Print();

      hit = (nDetHit*)evt->GetHCofThisEvent()->GetHC(i)->GetHit(j);
      runAct->vTimePhotonSDPushBack(hit->GetTime());
      runAct->vPhotonPositionXPushBack(hit->GetPos().x());
      runAct->vPhotonPositionYPushBack(hit->GetPos().y());
    }*/
  //}



  // set the depEnergy branch
  //G4cout<<depositedEnergy<<G4endl;
  runAct->setDepEnergy(depositedEnergy);
  runAct->setEventNb(evt->GetEventID());

  // fill branches
    runAct->fillBranch();
    if(fAnalysisManager)
        fAnalysisManager->EndOfEventAction(evt);
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
