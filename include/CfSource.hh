#ifndef CF_SOURCE_HH
#define CF_SOURCE_HH

#include "G4VUserActionInitialization.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImessenger.hh"
#include "globals.hh"
#include <vector>

#include "nDetPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;

class CfSource;
class G4UIdirectory;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Californium{
  public:
        double totalIntegral;
        double energy[101];
        double neutrons[101];
        double integral[101];

        double func(const double &E);

        Californium();

        double sample();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CfSourceMessenger;
class nDetConstruction;

class CfSource : public nDetPrimaryGeneratorAction
{
  public:
    CfSource(nDetRunAction *run, const nDetConstruction *det=NULL);   
     
   ~CfSource();

	void GeneratePrimaries(G4Event*);

	Californium *GetSource(){ return &source; }

    G4double RejectAccept(){ return 0; }

    G4double InverseCumul(){ return 0; }

	void SetPosition(const G4ThreeVector &p);
	
	void SetDirection(const G4ThreeVector &d);

	void SetType(const G4String &str);

	void SetDetector(const nDetConstruction *det);

  private:    
	CfSourceMessenger* fGunMessenger;
 
	Californium source; // 252Cf source

	G4ThreeVector pos;
	G4ThreeVector dir;
	G4ThreeVector detSize;
	G4ThreeVector detPos;
	G4ThreeVector vSourceDet;
	G4String type;

    void InitFunction(){ }    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CfSourceMessenger: public G4UImessenger
{
  public:
    CfSourceMessenger(CfSource*);
    
   ~CfSourceMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    CfSource* fAction;
    
    G4UIdirectory*            fDir;
    
    G4UIcommand*              fActionCmd[4];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SteppingVerbose : public G4SteppingVerbose {

public:   

  SteppingVerbose(){ }
  
 ~SteppingVerbose(){ }
 
  virtual void TrackingStarted(){ }

  virtual void StepInfo(){ }
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
