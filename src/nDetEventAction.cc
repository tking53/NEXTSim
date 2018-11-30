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
 
nDetEventAction::nDetEventAction(nDetRunAction* run) : runAct(run), timer(new G4Timer), depositedEnergy(0), avgTimePerEvent(0), previousTime(0), totalTime(0), eventID(0), previousEvents(0), totalEvents(0) { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
nDetEventAction::~nDetEventAction(){ 
  delete timer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::BeginOfEventAction(const G4Event* evt){
  // initialisation per event.

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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void nDetEventAction::EndOfEventAction(const G4Event* evt){
  // fill branches
  runAct->fillBranch();
        
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
