//
// $Id: RunAction.hh 12.10.2016 A Fijalkowska $
// 
/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Timer.hh"
#include "globals.hh"
#include "G4String.hh"
class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
  private:
    G4Timer* timer;
    G4String MakeOutputFileName();
    G4int totNrOfEvents;
};


#endif

