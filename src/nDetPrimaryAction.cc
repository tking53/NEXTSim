//
// $Id: nDetPrimaryGeneratorAction.cc, v1.0 Sept., 2015  $
// Written by Dr. Xiaodong Zhang 
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "nDetPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Alpha.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Triton.hh"
#include "G4Deuteron.hh"
#include "Randomize.hh"
#include "G4ParticleGun.hh"
#include "nDetAnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetPrimaryGeneratorAction::nDetPrimaryGeneratorAction(nDetRunAction* run)
: runAct(run)
{
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  //default kinematic
  //
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  //G4ParticleDefinition* particle = G4Neutron::NeutronDefinition();
 //G4ParticleDefinition* particle = G4Alpha::AlphaDefinition();
  G4ParticleDefinition* particle = G4Gamma::GammaDefinition();
//  G4ParticleDefinition* particle = G4Electron::ElectronDefinition();

  particleGun->SetParticleDefinition(particle);

  /*
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0, -1., 0)); // along the y-axis direction
 // particleGun->SetParticleEnergy(10*MeV);
  //changed to 1 MeV KS 5/20/16
  particleGun->SetParticleEnergy(662.*keV);
  */
   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetPrimaryGeneratorAction::~nDetPrimaryGeneratorAction()
{
    delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

//  this code is used to shoot one quater of the small unit of the structure
//  G4double x = G4UniformRand()*0.00075; 
//  G4double y = G4UniformRand()*0.00075; //

// one unit of the small structure
  //G4double x = 0*mm; //(G4UniformRand()-5/6.0)*0.0015;
  // changing the neutron incident position to study the position performance
  //  G4double y = 100*mm;
  //  G4double z = 0*mm; // center of one pixel

//  x=-2*mm;
//  y=0;
//  z=0;


  //particleGun->SetParticlePosition(G4ThreeVector(x, y, z));

  runAct->initEnergy = particleGun->GetParticleEnergy();

  particleGun->GeneratePrimaryVertex(anEvent);

  G4ThreeVector VertexPosition= particleGun->GetParticlePosition();

  nDetAnalysisManager *theManager=(nDetAnalysisManager*)nDetAnalysisManager::Instance();

  if(theManager)
	theManager->GeneratePrimaries(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

