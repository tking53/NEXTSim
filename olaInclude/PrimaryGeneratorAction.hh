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
//
// $Id: PrimaryGeneratorAction.hh 12.10.2016 A Fijalkowska $
//
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4ParticleGun;
class G4Event;
class Decay;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

  public:
    virtual void GeneratePrimaries(G4Event*);

  private:
	void SetUpDefault();
	void LoadDecay(std::string filename);
	void GenerateIsotropicDirectionDistribution (G4ThreeVector* direction, 
	                                             G4double thetaMin, 
	                                             G4double thetaMax, 
	                                             G4double phiMin, 
	                                             G4double phiMax);
	void GenerateIsotropicDirectionDistribution(G4ThreeVector* direction, 
	                                            G4double theta0);
	void RadiateBarByParallelBeam(G4Event* anEvent, 
	                            G4double barWidth, 
	                            G4double barLength);
	void GenerateDecay(G4Event* anEvent);
	void GenerateSingleParticle(G4Event* anEvent);
	void RadiateIsotropicallyMediumBar(G4Event* anEvent);
	
	G4double GetRandomFromLorentzDistribution(G4double en0, G4double gamma);
    void Generate17N(G4Event* anEvent);
    
                                
    G4ParticleGun* particleGun;
    Decay* decay;
    
    bool runDecay;
};


#endif 

