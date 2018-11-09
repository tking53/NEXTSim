#include "CfSource.hh"
#include "nDetConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "Randomize.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Source::~Source(){
	if(size > 0){
		delete[] energy;
		delete[] intensity;
		delete[] integral;
	}
}

void Source::addLevel(const double &E_, const double &I_){
	gammas.push_back(EnergyLevel(E_*keV, I_, totalIntegral));
	totalIntegral += I_;
}

double Source::sample() const {
	if(!gammas.empty())
		return this->sampleLevels();
	else if(size > 0)
		return this->sampleDistribution();
	return E;
}

void Source::initializeDistribution(const size_t &size_, const double &stepSize){
	size = size_;
	energy = new double[size_+1];
	intensity = new double[size_+1];
	integral = new double[size_+1];
	for(size_t i = 0; i < size+1; i++){
		energy[i] = i*stepSize;
		intensity[i] = func(energy[i]);
	}
	integral[0] = 0;
	for(size_t i = 1; i < size+1; i++){
		integral[i] = integral[i-1] + 0.5*(intensity[i-1]+intensity[i])*0.1;
	}
	totalIntegral = integral[size];
}

double Source::sampleLevels() const {
	double sampleIntegral = G4UniformRand()*totalIntegral;
	for(std::vector<EnergyLevel>::const_iterator iter = gammas.begin(); iter != gammas.end(); iter++){
		if(iter->check(sampleIntegral))
			return iter->getEnergy();
	}
	return -1;
}

double Source::sampleDistribution() const {
	double sampleIntegral = G4UniformRand()*totalIntegral;
	size_t indexUp = size/2;
	size_t indexDn = 0;
	size_t iter = 4;
	while(true){
		if(indexUp == indexDn){
			indexUp++;
		}
		else if(indexUp == indexDn+1){
			if(integral[indexDn] < sampleIntegral && integral[indexUp] >= sampleIntegral){
				return (energy[indexDn] + 0.1*(sampleIntegral-integral[indexDn])/(integral[indexUp]-integral[indexDn]));
			}
			indexDn++;
			indexUp++;
		}
		else if(size/iter > 0){
			if(sampleIntegral >= integral[indexUp]){
				indexDn = indexUp;
				indexUp += size/iter;
			}
			else{
				indexUp = indexUp - size/iter;
			}
		}
		else{
			indexUp--;
		}
		iter *= 2;
	}
	
	/*for(size_t i = 1; i < size+1; i++){
		if(integral[i-1] < sampleIntegral && integral[i] >= sampleIntegral){
			return (energy[i-1] + 0.1*(sampleIntegral-integral[i-1])/(integral[i]-integral[i-1]));
		}
	}*/
	
	return intensity[size];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

double Californium252::func(const double &E_) const {
	const double a = 1.174; // MeV (Mannhart)
	const double b = 1.043; // 1/MeV (Mannhart)
	const double coeff = (2/std::sqrt(pi*b*a*a*a))*std::exp(-a*b/4);
	return coeff*std::exp(-E_/a)*std::sinh(std::sqrt(b*E_));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParticleSource::ParticleSource(nDetRunAction *run, const nDetConstruction *det/*=NULL*/) : nDetPrimaryGeneratorAction(), fGunMessenger(0), psource(), pos(0, 0, 0), 
												                               dir(0, 0, 0), detPos(0, 0, 0), detSize(0, 0, 0), type("iso"), beamspot(0),
												                               unitX(1, 0, 0), unitY(0, 1, 0), unitZ(0, 0, 1)
{
	runAct = run;

	particleGun = new G4ParticleGun(1);
	particleGun->SetParticlePosition(this->pos);
	particleGun->SetParticleMomentumDirection(this->dir);
	this->SetNeutronBeam(1.0); // Set a 1 MeV neutron beam by default

	if(det)
		this->SetDetector(det);
	
	//create a messenger for this class
	fGunMessenger = new ParticleSourceMessenger(this); 
}

ParticleSource::~ParticleSource()
{ }

void ParticleSource::GeneratePrimaries(G4Event* anEvent)
{
	double energy = psource->sample();
	particleGun->SetParticleEnergy(energy);
	runAct->initEnergy = energy;
	if(psource->getIsIsotropic()){ // Generate particles psuedo-isotropically
		// We don't really use a true isotropic source because that would be really slow.
		// Generate a random point inside the volume of the detector in the frame of the detector.
		G4double x = (detSize.getX()/2)*(2*G4UniformRand()-1);
		G4double y = (detSize.getY()/2)*(2*G4UniformRand()-1);
		G4double z = (detSize.getZ()/2)*(2*G4UniformRand()-1);
		G4ThreeVector dirPrime = vSourceDet + G4ThreeVector(x, y, z); // Now in the lab frame
		dirPrime *= (1/dirPrime.mag());
		particleGun->SetParticleMomentumDirection(dirPrime); // along the y-axis direction
	}
	else if(beamspot > 0){ // Generate particles in a pencil-beam
		// Uniformly sample the circular profile
		double ranT = 2*pi*G4UniformRand();
		double ranU = G4UniformRand() + G4UniformRand();
		double ranR;
	
		if(ranU > 1){ ranR = 2 - ranU; }
		else{ ranR = ranU; }
		ranR *= beamspot;
		
		//particleGun->SetParticlePosition(G4ThreeVector(ranR*std::cos(ranT), ranR*std::sin(ranT), -offset_));
	}
	particleGun->GeneratePrimaryVertex(anEvent);
}

void ParticleSource::SetPosition(const G4ThreeVector &p){ 
	pos = p; 
	vSourceDet = detPos - pos;
	particleGun->SetParticlePosition(this->pos);
}

void ParticleSource::SetDirection(const G4ThreeVector &d){ 
	unitX = G4ThreeVector(1, 0, 0);
	unitY = G4ThreeVector(0, 1, 0);
	unitZ = G4ThreeVector(0, 0, 1);
	
	G4RotationMatrix rot;
	rot.rotateX(d.getX()*deg);
	rot.rotateY(d.getY()*deg);
	rot.rotateZ(d.getZ()*deg);
	unitX = rot*unitX;
	unitY = rot*unitY;
	unitZ = rot*unitZ;
	
	/*std::cout << " debug: x=(" << unitX.getX() << ", " << unitX.getY() << ", " << unitX.getZ() << ")\n";
	std::cout << " debug: y=(" << unitY.getX() << ", " << unitY.getY() << ", " << unitY.getZ() << ")\n";
	std::cout << " debug: z=(" << unitZ.getX() << ", " << unitZ.getY() << ", " << unitZ.getZ() << ")\n";*/
	
	// The "beam" is along the +X axis.
	dir = unitX;
	particleGun->SetParticleMomentumDirection(dir);
}

void ParticleSource::SetType(const G4String &str){ 
	if(str == "137Cs")
		this->Set137Cs();
	else if(str == "60Co")
		this->Set60Co();
	else if(str == "133Ba")
		this->Set133Ba();
	else if(str == "241Am")
		this->Set241Am();
	else if(str == "90Sr")
		this->Set90Sr();
	else if(str == "252Cf")
		this->Set252Cf();
	else{
		std::cout << " ParticleSource: Unknown source type \"" << type << "\"\n";
		return;
	}
	type = str;
	std::cout << " ParticleSource: Setting " << type << " source.\n";
}

void ParticleSource::SetBeamType(const G4String &str){ 
	size_t index = str.find_first_of(' ');
	std::string beamType = str;
	double beamEnergy = 1; // MeV
	if(index != std::string::npos){
		beamType = str.substr(0, index);
		beamEnergy = strtod(str.substr(index+1).c_str(), NULL);
	}
	if(beamType == "neutron")
		this->SetNeutronBeam(beamEnergy);
	else if(beamType == "gamma")
		this->SetGammaRayBeam(beamEnergy);
	else if(beamType == "electron")
		this->SetElectronBeam(beamEnergy);
	else{
		std::cout << " ParticleSource: Unknown beam type \"" << str << "\"\n";
		return;
	}
	type = "beam";
	std::cout << " ParticleSource: Setting " << beamType << " beam with energy of " << beamEnergy << " MeV.\n";
}

void ParticleSource::SetDetector(const nDetConstruction *det){
	detPos = det->GetDetectorPos();
	detSize = det->GetDetectorSize();
	vSourceDet = detPos - pos;
}

void ParticleSource::Set137Cs(){
	GetNewSource();
	particleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
	psource->addLevel(4.47, 0.91);
	psource->addLevel(31.817, 1.99);
	psource->addLevel(32.194, 3.64);
	psource->addLevel(36.304, 0.348);
	psource->addLevel(36.378, 0.672);
	psource->addLevel(37.255, 0.213);
	psource->addLevel(661.657, 85.10);
}

void ParticleSource::Set60Co(){
	GetNewSource();
	particleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
	psource->addLevel(7.461, 0.00322);
	psource->addLevel(7.478, 0.0063);
	psource->addLevel(347.14, 0.0075);
	psource->addLevel(826.10, 0.0076);
	psource->addLevel(1173.228, 99.85);
	psource->addLevel(1332.492, 99.9826);
	psource->addLevel(2158.57, 0.00120);
}

void ParticleSource::Set133Ba(){
	GetNewSource();
	particleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
	psource->addLevel(4.29, 15.7);
	psource->addLevel(30.625, 33.9);
	psource->addLevel(30.973, 62.2);
	psource->addLevel(34.92, 5.88);
	psource->addLevel(34.987, 11.4);
	psource->addLevel(35.818, 3.51);
	psource->addLevel(53.1622, 2.14);
	psource->addLevel(79.6142, 2.65);
	psource->addLevel(80.9979, 32.9);
	psource->addLevel(160.612, 0.638);
	psource->addLevel(223.2368, 0.453);
	psource->addLevel(276.3989, 7.16);
	psource->addLevel(302.8508, 18.34);
	psource->addLevel(356.0129, 62.05);
	psource->addLevel(383.8485, 8.94);
}

void ParticleSource::Set241Am(){
	GetNewSource();
	particleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
	psource->addLevel(13.9, 37);
	psource->addLevel(26.3446, 2.27);
	psource->addLevel(33.196, 0.126);
	psource->addLevel(42.704, 0.0055);
	psource->addLevel(43.42, 0.073);
	psource->addLevel(55.56, 0.0181);
	psource->addLevel(59.5409, 35.9);
	psource->addLevel(69.76, 0.0029);
	psource->addLevel(97.069, 0.00114);
	psource->addLevel(98.97, 0.0203);
	psource->addLevel(101.059, 0.00181);
	psource->addLevel(102.98, 0.0195);
}

void ParticleSource::Set90Sr(){
	GetNewSource();
	particleGun->SetParticleDefinition(G4Electron::ElectronDefinition());
	psource->addLevel(195.8, 100);
}

void ParticleSource::Set252Cf(){
	if(psource) delete psource;
	psource = new Californium252();
	particleGun->SetParticleDefinition(G4Neutron::NeutronDefinition());
}

void ParticleSource::SetNeutronBeam(const double &energy_, const double &beamspot_/*=0*/){
	beamspot = beamspot_;
	GetNewSource(energy_);
	particleGun->SetParticleDefinition(G4Neutron::NeutronDefinition());
}

void ParticleSource::SetGammaRayBeam(const double &energy_, const double &beamspot_/*=0*/){
	beamspot = beamspot_;
	GetNewSource(energy_);
	particleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
}

void ParticleSource::SetElectronBeam(const double &energy_, const double &beamspot_/*=0*/){
	beamspot = beamspot_;
	GetNewSource(energy_);
	particleGun->SetParticleDefinition(G4Electron::ElectronDefinition());
}

Source *ParticleSource::GetNewSource(const double &E_/*=-1*/){
	if(psource) delete psource;
	if(E_ > 0)
		return (psource = new Source(E_));
	return (psource = new Source());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParticleSourceMessenger::ParticleSourceMessenger(ParticleSource* Gun) : G4UImessenger(), fAction(Gun), fDir(0)
{
	fDir = new G4UIdirectory("/source/");
	fActionCmd[0] = new G4UIcmdWithoutParameter("/source/sample",this); // test function
	fActionCmd[1] = new G4UIcmdWith3VectorAndUnit("/source/position",this); // position of source
	fActionCmd[2] = new G4UIcmdWith3Vector("/source/direction",this); // direction of source
	fActionCmd[3] = new G4UIcmdWithAString("/source/type",this); // type of source (iso or directional)
	fActionCmd[4] = new G4UIcmdWithAString("/source/beam",this); // type of source (iso or directional)
}

ParticleSourceMessenger::~ParticleSourceMessenger()
{
	delete fDir;
}

void ParticleSourceMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
	if(command == fActionCmd[0])
		std::cout << " energy=" << fAction->GetParticleSource()->sample() << " MeV\n";
	else if(command == fActionCmd[1])
		fAction->SetPosition(G4UIcommand::ConvertToDimensioned3Vector(newValue));
	else if(command == fActionCmd[2])
		fAction->SetDirection(G4UIcommand::ConvertTo3Vector(newValue));
	else if(command == fActionCmd[3])
		fAction->SetType(newValue);
	else if(command == fActionCmd[4])
		fAction->SetBeamType(newValue);
}
