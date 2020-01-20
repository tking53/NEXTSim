//
// $Id: SteppingAction.hh 12.07.2016 A. Fijalkowska $
//
/// \file SteppingAction.hh
/// \brief Definition of the SteppingAction class
//
#ifndef SteppingAction_H
#define SteppingACtion_H 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "G4OpBoundaryProcess.hh"


class SteppingAction : public G4UserSteppingAction
{
  public:

    SteppingAction();
    virtual ~SteppingAction();
    virtual void UserSteppingAction(const G4Step*);

 
  private:
     //G4ThreadLocal G4OpBoundaryProcess* FindBoundaryProcess(const G4Step*);
     void PrintStep(const G4Step*, G4OpBoundaryProcessStatus);
    G4OpBoundaryProcessStatus expectedNextStatus;
};

#endif
