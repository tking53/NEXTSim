//
// $Id: nDetEventAction.cc, v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang 
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
#include "nDetEventAction.hh"

#include "G4Event.hh"
#include "G4Timer.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::nDetEventAction(nDetRunAction* run) : runAct(run), timer(new G4Timer), previousTime(0), totalTime(0), totalEvents(0), displayTimeInterval(10), numPhotons(0), numPhotonsDet(0) { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::~nDetEventAction(){ 
  delete timer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::BeginOfEventAction(const G4Event* evt){
  // initialisation per event.

  /*G4long eventID = evt->GetEventID();

  double avgTimePerEvent;
  double avgTimePerPhoton;
  double avgTimePerDetection;

  if(eventID != 0){
    timer->Stop();
    totalTime += timer->GetRealElapsed();
    if(displayTimeInterval > 0 && (totalTime - previousTime) >= displayTimeInterval){ // Display every 10 seconds.
      std::cout << "Event ID: " << eventID << ", TIME=" << totalTime << " s";
      avgTimePerEvent = totalTime/eventID;
      avgTimePerPhoton = totalTime/numPhotons;
      avgTimePerDetection = totalTime/numPhotonsDet;
      if(totalEvents > 0){
        std::cout << ", REMAINING=" << (totalEvents-eventID)*avgTimePerEvent << " s";
      }
      std::cout << ", RATE=" << 1/avgTimePerEvent << " evt/s (" << 1/avgTimePerPhoton << " phot/s & " << 1/avgTimePerDetection << " det/s)\n";
      previousTime = totalTime;
    }
  }*/
  
  // Start the timer.
  timer->Start();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::EndOfEventAction(const G4Event*){
  // Process primary scatters. 
  runAct->process();

  // Fill branches
  //runAct->fillBranch();

  // Update photon statistics.
  //numPhotons += runAct->nPhotonsTot;
  //numPhotonsDet += runAct->nPhotonsDet[0]+runAct->nPhotonsDet[1];
}

void nDetEventAction::StartTimer(){
  timer->Start();
}

G4double nDetEventAction::GetTimeElapsed(){
  return timer->GetRealElapsed();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
