#ifndef NDET_EVENT_ACTION_HH
#define NDET_EVENT_ACTION_HH

#include "nDetRunAction.hh"

#include "G4UserEventAction.hh"

class nDetRunAction;
class G4Event;

class nDetEventAction : public G4UserEventAction
{
  public:
    nDetEventAction(nDetRunAction*);
    
    virtual ~nDetEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    
    void EndOfEventAction(const G4Event*);

  private:
    nDetRunAction *runAct; ///< Pointer to the thread-local user run action
};

#endif
