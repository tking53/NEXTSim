//
// $Id: nDetEventAction.hh,v1.0 Sept., 2015 $
//   Written by Dr. Xiaodong Zhang
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
#ifndef nDetEventAction_h
#define nDetEventAction_h 1

#include "nDetRunAction.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

//class G4Event;
class nDetRunAction;
class G4Event;
class G4Timer;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class nDetEventAction : public G4UserEventAction
{
  public:
    nDetEventAction(nDetRunAction*);
    
    virtual ~nDetEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    
    void EndOfEventAction(const G4Event*);

    void StartTimer();
    
    void AddDepE(const G4double &depE){ depositedEnergy += depE; };

    void SetTotalEvents(const G4long &events){ totalEvents = events; }

    G4long GetEventID(){ return eventID; };

    G4double GetTimeElapsed();

  private:
    nDetRunAction *runAct;
    
    G4Timer *timer;
    
    G4double depositedEnergy;   // deposited energy in Scintillator.
    G4double avgTimePerEvent;
    G4double previousTime;
    G4double totalTime;
    
    G4long eventID;
    G4long previousEvents;
    G4long totalEvents;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

// Class description:
//
// This is the class of one of the user's optional action class.
// The two methods BeginOfEventAction() and EndOfEventAction() are invoked
// at the begining and the end of one event processing. These methods are
// invoked by G4EventManager.
//
// Be aware that BeginOfEventAction() is invoked when a G4Event object is 
// sent to G4EventManager. Thus the primary vertexes/particles have already
// been made by the primary generator. In case the user wants to do something
// before generating primaries(i.e., store randomnumber status), do it in the 
// G4VUserPrimaryGeneratorAction concrete class.
//    
