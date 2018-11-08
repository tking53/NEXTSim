//
// $Id: nDetSteppingAction.hh,v 1.0 Jan., 2015 $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
// by Dr. Xiaodong Zhang:q
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef nDetSteppingAction_h
#define nDetSteppingAction_h 1

#include <vector>

#include "G4UserSteppingAction.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

//class nDetConstruction;
//class nDetEventAction;
#include "nDetConstruction.hh"
#include "nDetRunAction.hh"
#include "nDetEventAction.hh"
#include "photonCounter.hh"
#include "centerOfMass.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class nDetSteppingAction : public G4UserSteppingAction
{
public:
  nDetSteppingAction(nDetConstruction*, nDetRunAction*, nDetEventAction*);
  
  virtual ~nDetSteppingAction();

  void SetGenerator(G4VUserPrimaryGeneratorAction *ptr){ generator = ptr; }

  void UserSteppingAction(const G4Step*);

  photonCounter *GetCounter(){ return &counter; }
  
  centerOfMass *GetCenterOfMassPositiveSide(){ return &center[0]; }
  
  centerOfMass *GetCenterOfMassNegativeSide(){ return &center[1]; }

  bool setPmtSpectralResponse(const char *fname);
  
  void Reset();

private:
  nDetConstruction* detector;
  
  nDetRunAction* runAction;
  
  nDetEventAction*  evtAction;
  
  G4VUserPrimaryGeneratorAction *generator;
  
  G4long eventID;
  
  G4long stepID;
  
  photonCounter counter;
  
  centerOfMass center[2];
  
  bool neutronTrack;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

