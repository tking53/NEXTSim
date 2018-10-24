//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file eventgenerator/particleGun/include/CfSource.hh
/// \brief Definition of the CfSource class
//
//
// $Id: CfSource.hh 99713 2016-10-03 07:35:42Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef CfSource_h
#define CfSource_h 1

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

class CfSource : public nDetPrimaryGeneratorAction
{
  public:
    CfSource(nDetRunAction *run=NULL);   
     
   ~CfSource();

	void GeneratePrimaries(G4Event*);

	Californium *GetSource(){ return &source; }

    G4double RejectAccept(){ return 0; }

    G4double InverseCumul(){ return 0; }

	void SetPosition(const G4ThreeVector &p);
	
	void SetDirection(const G4ThreeVector &d);

	void SetType(const G4String &str);

  private:    
	CfSourceMessenger* fGunMessenger;
 
	Californium source; // 252Cf source

	G4ThreeVector pos;
	G4ThreeVector dir;
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
