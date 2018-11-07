#include "CfSource.hh"
#include "nDetConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Neutron.hh"
#include "Randomize.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

double Californium::func(const double &E){
	const double a = 1.174; // MeV (Mannhart)
	const double b = 1.043; // 1/MeV (Mannhart)
	const double coeff = (2/std::sqrt(pi*b*a*a*a))*std::exp(-a*b/4);
        return coeff*std::exp(-E/a)*std::sinh(std::sqrt(b*E));
}

Californium::Californium() : totalIntegral(0.0) {
        for(size_t i = 0; i < 101; i++){
                energy[i] = i*0.1;
                neutrons[i] = func(energy[i]);
        }
        integral[0] = 0;
        for(size_t i = 1; i < 101; i++){
                integral[i] = integral[i-1] + 0.5*(neutrons[i-1]+neutrons[i])*0.1;
        }
        totalIntegral = integral[100];
}

double Californium::sample(){
        double sampleIntegral = G4UniformRand()*totalIntegral;
        for(size_t i = 1; i < 101; i++){
                if(integral[i-1] < sampleIntegral && integral[i] >= sampleIntegral){
                        return (energy[i-1] + 0.1*(sampleIntegral-integral[i-1])/(integral[i]-integral[i-1]));
                }
        }
        return neutrons[100];
}

CfSource::CfSource(nDetRunAction *run, const nDetConstruction *det/*=NULL*/) : nDetPrimaryGeneratorAction(), fGunMessenger(0), source(), pos(0, 0, 0), 
                                                                               dir(0, 0, 0), detPos(0, 0, 0), detSize(0, 0, 0), type("iso")
{
  runAct = run;

  particleGun = new G4ParticleGun(1);
  particleGun->SetParticleDefinition(G4Neutron::NeutronDefinition());
  particleGun->SetParticlePosition(this->pos);
  particleGun->SetParticleMomentumDirection(this->dir);

  if(det)
  	this->SetDetector(det);
  
  //create a messenger for this class
  fGunMessenger = new CfSourceMessenger(this); 
}

CfSource::~CfSource()
{ }

void CfSource::GeneratePrimaries(G4Event* anEvent)
{
	double energy = source.sample();
	particleGun->SetParticleEnergy(energy);
	runAct->initEnergy = energy;
	if(type == "iso"){ // Generate particles psuedo-isotropically
		// We don't really use a true isotropic source because that would be really slow.
		// Generate a random point inside the volume of the detector in the frame of the detector.
		G4double x = (detSize.getX()/2)*(2*G4UniformRand()-1);
		G4double y = (detSize.getY()/2)*(2*G4UniformRand()-1);
		G4double z = (detSize.getZ()/2)*(2*G4UniformRand()-1);
		G4ThreeVector dirPrime = vSourceDet + G4ThreeVector(x, y, z); // Now in the lab frame
		dirPrime *= (1/dirPrime.mag());
		particleGun->SetParticleMomentumDirection(dirPrime); // along the y-axis direction
	}
	particleGun->GeneratePrimaryVertex(anEvent);
}

void CfSource::SetPosition(const G4ThreeVector &p){ 
	pos = p; 
	vSourceDet = detPos - pos;
	particleGun->SetParticlePosition(this->pos);
}

void CfSource::SetDirection(const G4ThreeVector &d){ 
	dir = d; 
	particleGun->SetParticleMomentumDirection(this->dir);
}

void CfSource::SetType(const G4String &str){ 
	type = str; 
}

void CfSource::SetDetector(const nDetConstruction *det){
	detPos = det->GetDetectorPos();
	detSize = det->GetDetectorSize();
	vSourceDet = detPos - pos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CfSourceMessenger::CfSourceMessenger(CfSource* Gun) : G4UImessenger(), fAction(Gun), fDir(0)
{
  fDir = new G4UIdirectory("/cfsource/");
  fActionCmd[0] = new G4UIcmdWithoutParameter("/cfsource/sample",this); // test function
  fActionCmd[1] = new G4UIcmdWith3VectorAndUnit("/cfsource/position",this); // position of source
  fActionCmd[2] = new G4UIcmdWith3Vector("/cfsource/direction",this); // direction of source
  fActionCmd[3] = new G4UIcmdWithAString("/cfsource/type",this); // type of source (iso or directional)
}

CfSourceMessenger::~CfSourceMessenger()
{
  delete fDir;
}

void CfSourceMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if(command == fActionCmd[0])
  	std::cout << " energy=" << fAction->GetSource()->sample() << " MeV\n";
  else if(command == fActionCmd[1])
    fAction->SetPosition(G4UIcommand::ConvertToDimensioned3Vector(newValue));
  else if(command == fActionCmd[2])
    fAction->SetDirection(G4UIcommand::ConvertTo3Vector(newValue));
  else if(command == fActionCmd[3])
    fAction->SetType(newValue);
}
