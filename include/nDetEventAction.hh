#ifndef nDetEventAction_h
#define nDetEventAction_h 1

#include "nDetRunAction.hh"

#include "G4UserEventAction.hh"

class nDetRunAction;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class nDetEventAction : public G4UserEventAction
{
  public:
    nDetEventAction(nDetRunAction*);
    
    virtual ~nDetEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    
    void EndOfEventAction(const G4Event*);

  private:
    nDetRunAction *runAct;
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
