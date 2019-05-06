//
// $Id: nDetSteppingAction.hh,v 1.0 Jan., 2015 $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
// by Dr. Xiaodong Zhang:q
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef nDetSteppingAction_h
#define nDetSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class nDetConstruction;
class nDetEventAction;
class nDetRunAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class nDetSteppingAction : public G4UserSteppingAction
{
public:
  nDetSteppingAction(nDetConstruction*, nDetRunAction*, nDetEventAction*);
  
  virtual ~nDetSteppingAction();

  void UserSteppingAction(const G4Step*);

  void Reset();

private:
  nDetConstruction* detector;
  
  nDetRunAction* runAction;
  
  nDetEventAction*  evtAction;
  
  G4long stepID;
  
  bool neutronTrack;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

