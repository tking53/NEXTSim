#ifndef NDET_STEPPING_ACTION_HH
#define NDET_STEPPING_ACTION_HH

#include "G4UserSteppingAction.hh"

class nDetConstruction;
class nDetRunAction;

class nDetSteppingAction : public G4UserSteppingAction
{
public:
  nDetSteppingAction(nDetRunAction*);
  
  virtual ~nDetSteppingAction();

  void UserSteppingAction(const G4Step*);

  void Reset();

private:
  nDetRunAction* runAction; ///< Pointer to the thread-local user run action
  
  bool neutronTrack; ///< Flag indicating that a primary particle is being tracked
};

#endif

