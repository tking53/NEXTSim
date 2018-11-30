#ifndef PARTICLE_SOURCE_HH
#define PARTICLE_SOURCE_HH

#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImessenger.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"
#include <vector>

class G4ParticleGun;
class G4Event;

class ParticleSource;
class nDetRunAction;

class G4UIdirectory;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EnergyLevel{
  public:
	EnergyLevel() : energy(0), intensity(0), runningSum(0) { }
  
	EnergyLevel(const double &E_, const double &I_, const double &sum_) : energy(E_), intensity(I_), runningSum(sum_) { }
	
	double getEnergy() const { return energy; }
	
	bool check(const double &sum) const { return (sum >= runningSum && sum < runningSum+intensity); }
	
  private:
	double energy;
	double intensity;
	double runningSum;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Source{
  public:
	Source() : iso(true), E(-1), totalIntegral(0), size(0), energy(NULL), intensity(NULL), integral(NULL) { }
	
	Source(const double &E_) : iso(false), E(E_), totalIntegral(0), size(0), energy(NULL), intensity(NULL), integral(NULL) { }
	
	virtual ~Source();
	
	bool getIsIsotropic() const { return iso; }
	
	double getEnergy() const { return E; }
	
	double getTotalIntegral() const { return totalIntegral; }

	double *getEnergyArray(){ return energy; }
	
	double *getIntensityArray(){ return intensity; }
	
	double *getIntegralArray(){ return integral; }

	bool setIsIsotropic(const bool &state_){ return (iso=state_); }

	double setEnergy(const double &E_){ return (E=E_); }
	
	void addLevel(const double &E_, const double &I_);
	
	double sample() const ;
	
	virtual double func(const double &E_) const { return E_; }
	
  protected:
	bool iso;
	double E;

	double totalIntegral;
	
	size_t size;
	double *energy;
	double *intensity;
	double *integral;

	std::vector<EnergyLevel> gammas;
  
	void initializeDistribution(const size_t &size_, const double &stepSize);
  
	double sampleLevels() const ;
	
	double sampleDistribution() const ;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Californium252 : public Source {
  public:
	Californium252() : Source() { this->initializeDistribution(100, 0.1); }
	
	~Californium252(){ }
	
	double func(const double &E_) const ;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ParticleSourceMessenger;
class nDetConstruction;

class ParticleSource : public G4VUserPrimaryGeneratorAction {
  public:
    ParticleSource(nDetRunAction *run, const nDetConstruction *det=NULL);
     
   ~ParticleSource();

	void GeneratePrimaries(G4Event*);

	Source *GetParticleSource(){ return psource; }

    G4double RejectAccept() const { return 0; }

    G4double InverseCumul() const { return 0; }

	void SetPosition(const G4ThreeVector &p);
	
	void SetDirection(const G4ThreeVector &d);

	bool SetType(const G4String &str);

	bool SetBeamType(const G4String &str);
	
	void SetBeamspotRadius(const G4double &radius){ beamspot = radius*mm; }

	void SetDetector(const nDetConstruction *det);

	void Set137Cs();
	
	void Set60Co();
	
	void Set133Ba();
	
	void Set241Am();
	
	void Set90Sr();
	
	void Set252Cf();
	
	void SetNeutronBeam(const double &energy_);
	
	void SetGammaRayBeam(const double &energy_);
	
	void SetElectronBeam(const double &energy_);

  private:    
	ParticleSourceMessenger *fGunMessenger;
 
	Source *psource; // Generic particle source

	G4ThreeVector pos;
	G4ThreeVector dir;
	G4ThreeVector detPos;	
	G4ThreeVector detSize;
	G4ThreeVector vSourceDet;
	G4String type;

	double beamspot;

	G4ThreeVector unitX;
	G4ThreeVector unitY;
	G4ThreeVector unitZ;

	nDetRunAction *runAction;

	G4ParticleGun *particleGun;
	
	G4RotationMatrix rot;

    void InitFunction(){ }
    
    Source *GetNewSource(const double &E_=-1);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ParticleSourceMessenger: public G4UImessenger
{
  public:
    ParticleSourceMessenger(ParticleSource*);
    
   ~ParticleSourceMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    ParticleSource* fAction;
    
    G4UIdirectory* fDir;
    
    G4UIcommand* fActionCmd[6];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
