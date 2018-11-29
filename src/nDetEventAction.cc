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
#include "G4Timer.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "nDetUserEventInformation.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "nDetSD.hh"
#include "SiPMSD.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::nDetEventAction(nDetRunAction* run) : runAct(run), timer(new G4Timer), fAnalysisManager(NULL), depositedEnergy(0), avgTimePerEvent(0), previousTime(0), totalTime(0), eventID(0), previousEvents(0), totalEvents(0) { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::~nDetEventAction(){ 
  delete timer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::BeginOfEventAction(const G4Event* evt){
  // initialisation per event.

  //New event, add the user information object
  G4EventManager::GetEventManager()->SetUserInformation(new nDetUserEventInformation);

  eventID = evt->GetEventID();
  depositedEnergy = 0; 

  if(eventID != 0){
    timer->Stop();
    G4double tprime = timer->GetRealElapsed();
    totalTime += tprime;
    if(totalTime - previousTime >= 10){ // Display every 10 seconds.
      std::cout << "Event ID: " << eventID << ", TIME=" << totalTime << " s";
      avgTimePerEvent = totalTime/eventID;
      if(totalEvents > 0){
        std::cout << ", REMAINING=" << (totalEvents-eventID)*avgTimePerEvent << " s";
      }
      std::cout << ", RATE=" << 1/avgTimePerEvent << " evt/s\n";
      previousTime = totalTime;
      previousEvents = eventID;
    }
    timer->Start();
  }
  else{ // Start the timer.
    timer->Start();
  }

  fAnalysisManager = (nDetAnalysisManager*)nDetAnalysisManager::Instance();

  if(fAnalysisManager)
    fAnalysisManager->BeginOfEventAction(evt);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::EndOfEventAction(const G4Event* evt){
  //G4cout<<"nDetEventAction::EndOfEventAction()"<<G4endl;
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
  //

  if(fpEventManager->GetVerboseLevel()==1){
    nDetUserEventInformation *theEventInformation= static_cast<nDetUserEventInformation*>(evt->GetUserInformation());
    G4int nDetections=theEventInformation->GetDetectionCount();    
    G4cout << "nDetEventAction::EndOfEventAction()->" << nDetections << " Photons detected in this event" << G4endl;
    //theEventInformation->Print();
  }

  // set the depEnergy branch
  //G4cout<<depositedEnergy<<G4endl;
  //runAct->depEnergy = depositedEnergy;
  //runAct->eventNb = evt->GetEventID();

  // fill branches
  runAct->fillBranch();
  if(fAnalysisManager)
    fAnalysisManager->EndOfEventAction(evt);
        
  // clear vector for next event
  runAct->vectorClear();
}

void nDetEventAction::StartTimer(){
  timer->Start();
}

G4double nDetEventAction::GetTimeElapsed(){
  return timer->GetRealElapsed();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
