#ifndef PARTICLE_SOURCE_HH
#define PARTICLE_SOURCE_HH

#include <vector>

#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImessenger.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class ParticleSource;
class nDetRunAction;
class Reaction;

class G4UIdirectory;
class G4UIcmdWithoutParameter;

///////////////////////////////////////////////////////////////////////////////
// class EnergyLevel
///////////////////////////////////////////////////////////////////////////////

class EnergyLevel{
  public:
	EnergyLevel() : energy(0), intensity(0), runningSum(0) { }
  
	EnergyLevel(const double &E_, const double &I_, const double &sum_) : energy(E_), intensity(I_), runningSum(sum_) { }
	
	double getEnergy() const { return energy; }
	
	bool check(const double &sum) const { return (sum >= runningSum && sum < runningSum+intensity); }
	
  private:
	double energy; ///< Gamma ray energy (in MeV)
	double intensity; ///< Fractional intensity
	double runningSum; ///< Sum of all energy level intensities preceeding this one
};

///////////////////////////////////////////////////////////////////////////////
// class Source
///////////////////////////////////////////////////////////////////////////////

class Source{
  public:
	Source() : iso(true), E(-1), totalIntegral(0), stepSize(0.1), size(0), energy(NULL), intensity(NULL), integral(NULL), ElowLimit(0), EhighLimit(-1), Ilow(0), Ihigh(-1) { }
	
	Source(const double &E_) : iso(false), E(E_), totalIntegral(0), stepSize(0.1), size(0), energy(NULL), intensity(NULL), integral(NULL), ElowLimit(0), EhighLimit(-1), Ilow(0), Ihigh(-1) { }
	
	virtual ~Source();
	
	bool getIsIsotropic() const { return iso; }
	
	double getEnergy() const { return E; }
	
	double getTotalIntegral() const { return totalIntegral; }

	double *getEnergyArray(){ return energy; }
	
	double *getIntensityArray(){ return intensity; }
	
	double *getIntegralArray(){ return integral; }

	bool setIsIsotropic(const bool &state_){ return (iso=state_); }

	double setEnergy(const double &E_){ return (E=E_); }
	
	void setEnergyLimits(const double &Elow_, const double &Ehigh_);
	
	void addLevel(const double &E_, const double &I_);
	
	double sample() const ;
	
	virtual double sample(const double &);
	
	virtual double func(const double &E_) const { return E_; }
	
  protected:
	bool iso; ///< Flag indicating that the source is isotropic
	double E; ///< Energy of the primary particle (in MeV)

	double totalIntegral; ///< Total integral of the energy distribution
	double stepSize; ///< Energy distribution step size (in MeV)
	
	size_t size; ///< Length of energy distribution arrays
	double *energy; ///< Array of source energy (in MeV)
	double *intensity; ///< Array for storing the source intensity as a function of the source energy
	double *integral; ///< Array for storing the running total integral of the intensity as a function of the source energy
	
	double ElowLimit; ///< Lower energy limit for restricting the source energy distribution (in MeV)
	double EhighLimit; ///< Upper energy limit for restricting the source energy distribution (in MeV)

	double Ilow; ///< Distribution integral corresponding to lower energy limit
	double Ihigh; ///< Distribution integral corresponding to upper energy limit

	std::vector<EnergyLevel> gammas; ///< Vector of all energy levels
  
	void initializeDistribution(const size_t &size_, const double &stepSize_);
  
	double sampleLevels() const ;
	
	double sampleDistribution() const ;
	
  private:
	double integrate(const double &E_) const ;
	
	void setEnergyLimits();
};

///////////////////////////////////////////////////////////////////////////////
// class Californium252
///////////////////////////////////////////////////////////////////////////////

class Californium252 : public Source {
  public:
	Californium252() : Source() { this->initializeDistribution(150, 0.1); }
	
	~Californium252(){ }

  private:
	double func(const double &E_) const ;
};

class ParticleSourceMessenger;
class nDetConstruction;

///////////////////////////////////////////////////////////////////////////////
// class ParticleSource
///////////////////////////////////////////////////////////////////////////////

class ParticleSource : public G4VUserPrimaryGeneratorAction {
  public:
    ParticleSource(nDetRunAction *run, nDetConstruction *det=NULL);
     
   ~ParticleSource();

	void GeneratePrimaries(G4Event*);

	Source *GetParticleSource(){ return psource; }

	ParticleSourceMessenger *GetMessenger(){ return fGunMessenger; }

    G4double RejectAccept() const { return 0; }

    G4double InverseCumul() const { return 0; }

	double GetTargetTimeOffset() const { return targTimeOffset; }

	void SetDirection(const G4ThreeVector &d);

	bool SetType(const G4String &str);

	bool SetBeamType(const G4String &str);
	
	void SetBeamspotRadius(const G4double &radius){ beamspot = radius*mm; }

	void SetBeamspotType(const G4String &str);

	void SetDetector(const nDetConstruction *det);

	void Set137Cs();
	
	void Set60Co();
	
	void Set133Ba();
	
	void Set241Am();
	
	void Set90Sr();
	
	void Set252Cf();
	
	void SetNeutronBeam(const double &energy_);
	
	void SetGammaRayBeam(const double &energy_);
	
	void SetLaserBeam(const double &energy_);
	
	void SetElectronBeam(const double &energy_);
	
	void SetIsotropicMode(bool state_=true);
	
	void SetEnergyLimits(const double &Elow_, const double &Ehigh_);
	
	bool LoadReactionFile(const G4String &fname);

	double Sample(bool verbose=false);

  private:    
	ParticleSourceMessenger *fGunMessenger;
 
	Source *psource; ///< Pointer to the 

	G4ThreeVector dir; ///< 3d momentum direction of the primary particle
	G4ThreeVector detPos; ///< 3d position of the detector used for isotropic sources
	G4ThreeVector detSize; ///< 3d size of the detector used for isotropic sources
	G4String type; ///< String indicating the type of source to use

	int beamspotType; ///< Integer indicating the type of beamspot to use for sampling

	double beamspot; ///< Radius of the beamspot (in mm)
	double targThickness; ///< Physical thickness of the target (in mm)
	double targEnergyLoss; ///< Slope equal to the energy loss of the primary particle as a function of distance in the target (in MeV/mm)
	double targTimeSlope; ///< Slope equal to the primary particle time straggling as a function of distance in the target (in ns/mm)
	double targTimeOffset; ///< Time offset due to primary particle straggling in the target (in ns)
	double beamE0; ///< Initial source energy (in MeV)

	G4ThreeVector unitX; ///< X-axis unit vector in the frame of the source
	G4ThreeVector unitY; ///< Y-axis unit vector in the frame of the source
	G4ThreeVector unitZ; ///< Z-axis unit vector in the frame of the source

	bool useReaction; ///< Flag indicating that energy and angle sampling should use a user specified reaction

	nDetRunAction *runAction; ///< Pointer to the thread-local user run action

	Reaction *particleRxn; ///< Pointer to a reaction object used for sampling reaction product particle energies and angles

	G4ParticleGun *particleGun; ///< Pointer to a generic Geant4 particle generator
	
	G4RotationMatrix rot; ///< Rotation of the source
	G4RotationMatrix detRot; ///< Rotation of the detector used for isotropic sources
	
    void InitFunction(){ }
    
    Source *GetNewSource(const double &E_=-1);
};

#endif
