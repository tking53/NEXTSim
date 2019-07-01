#ifndef NDET_PARTICLE_SOURCE_HH
#define NDET_PARTICLE_SOURCE_HH

#include <mutex>
#include <vector>

#include "G4GeneralParticleSource.hh"
#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImessenger.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

class G4ParticleDefinition;
class G4Event;

class nDetParticleSource;
class Reaction;

class nDetParticleSourceMessenger;
class nDetConstruction;

/*! \class nDetParticleSource
 *  \brief Wrapper of G4GeneralParticleSource class for added convenience
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date June 5, 2019
 *
 *  The G4GeneralParticleSource class is unwieldy to use, so this class offers
 *  methods which make it easier for the user to define sources and beams of
 *  particles by defining methods in a more transparent way. 
 *  
 *  NOTE: The user  may still interact with the general particle source on a 
 *  low level via macro commands.
*/

class nDetParticleSource : public G4GeneralParticleSource {
  public:
	/** Destructor
	  */
	~nDetParticleSource();

	/** Copy constructor. Not implemented for singleton class
	  */
	nDetParticleSource(nDetParticleSource const &);

	/** Assignment operator. Not implemented for singleton class
	  */	
	nDetParticleSource &operator=(nDetParticleSource const &);

	/** Get an instance of the singleton
	  */
	static nDetParticleSource &getInstance();
	
	/** Get a pointer to the messenger used for this class
	  */
	nDetParticleSourceMessenger *GetMessenger(){ return fSourceMessenger; }

	/** Get the time offset due to primary particle straggling in the target (in ns)
	  */
	double GetTargetTimeOffset() const { return targTimeOffset; }

	/** Sets the source to mono-energetic and sets the particle's energy
	  * @param energy The energy of the mono-energetic particle (in MeV)
	  */	
	void SetBeamEnergy(const G4double &energy);	

	/** Sets the position of the center of the source
	  * @param position The position of the center of the source (in mm)
	  */	
	void SetSourcePosition(const G4ThreeVector &position);	

	/** Set the default direction of the source
	  * @param d The direction vector normal to the plane of the surface of the source/beam
	  */
	void SetSourceDirection(const G4ThreeVector &d);

	/** Set the type of pre-defined particle source or beam
	  * @param str Space-delimited string containing the name of the source [137Cs 60Co 133Ba 241Am 90Sr 252Cf] 
	  *            or the beam particle [neutron gamma laser electron] and optional beam energy (in MeV)
	  */
	bool SetSourceType(const G4String &str);

	/** Sets the standard direction of the particle. Overrides source parameters, use with caution
	  * @param direction The 3d direction vector of the particle
	  */
	void SetParticleMomentumDirection(const G4ParticleMomentum &direction);

	/** Set the primary beamspot radius
	  * @param r Primary radius (or width where applicable) (in mm)
	  */
	void SetBeamspotRadius(const G4double &radius){ beamspot = radius*mm; }

	/** Set the secondary beamspot radius
	  * @param r0 Secondary radius (or height where applicable) (in mm)
	  */
	void SetBeamspotRadius0(const G4double &radius){ beamspot0 = radius*mm; }

	/** Set the primary and secondary beamspot radii
	  * @param r Primary radius (or width where applicable) (in mm)
	  * @param r0 Secondary radius (or height where applicable) (in mm)
	  */
	void SetBeamspotRadii(const G4double &r, const G4double &r0){ beamspot = r; beamspot0 = r0; }

	/** Set the shape of the beamspot
	  * @param str String containing the name of the beamspot shape [point circle annulus ellipse square rectangle vertical horizontal gauss]
	  * 
	  * The following table gives the definition of each beamspot shape along with the usage of the beamspot primary and secondary radii
	  * | Shape      | Radius (primary)                                         | Radius0 (secondary)                 |
	  * |------------|----------------------------------------------------------|-------------------------------------|
	  * | point      | N/A                                                      | N/A                                 |
	  * | circle     | Outer radius                                             | N/A                                 |
	  * | annulus    | Outer radius                                             | Inner radius                        |
	  * | ellipse    | X-axis radius (width)                                    | Y-axis radius (height)              |
	  * | square     | Half X and Y axis dimension (half-width and half-height) | N/A                                 |
	  * | rectangle  | Half X-axis dimension (half-width)                       | Half Y-axis dimension (half-height) |
	  * | vertical   | Half Y-axis dimension (half-height)                      | N/A                                 |
	  * | horizontal | Half X-axis dimension (half-width)                       | N/A                                 |
	  * | gauss      | FWHM along the X-axis (width)                            | FWHM along the Y-axis (height)      |
	  */
	void SetBeamspotType(const G4String &str);

	/** Set the energy sampling region for energy distributions
	  * @param Elow_ Low limit of the sampling region (in MeV)
	  * @param Ehigh_ High limit of the sampling region (in MeV)
	  */
	void SetEnergyLimits(const double &Elow_, const double &Ehigh_);

	/** Set information about the size and position of the detector for isotropic sources
	  * @param det Pointer to the detector
	  */
	void SetDetector(const nDetConstruction *det);

	/** Enable or disable isotropic source mode
	  * @param state_ Flag indicating whether the source is isotropic (true) or not (false)
	  */
	void SetIsotropicMode(bool state_=true){ isotropic = state_; }

	/** Set the source to use the energy distribution of 252Cf
	  * @param size_ The number of points to add to the distribution (larger numbers give higher precision)
	  * @param stepSize_ The size of each energy step (in MeV, smaller numbers give higher precision)
	  */
	void Set252Cf(const size_t &size_=150, const double &stepSize_=0.1);

	/** Set a pre-defined gamma-ray source using a spectrum for 137Cs
	  */
	void Set137Cs();

	/** Set a pre-defined gamma-ray source using a spectrum for 60Co
	  */	
	void Set60Co();

	/** Set a pre-defined gamma-ray source using a spectrum for 133Ba
	  */	
	void Set133Ba();

	/** Set a pre-defined gamma-ray source using a spectrum for 241Am
	  */	
	void Set241Am();

	/** Set a pre-defined beta source using a spectrum for 90Sr
	  */	
	void Set90Sr();

	/** Set a beam of mono-energetic neutrons
	  * @param energy_ Energy of the beam (in MeV)
	  */	
	void SetNeutronBeam(const double &energy_);

	/** Set a beam of mono-energetic gamma-rays
	  * @param energy_ Energy of the beam (in MeV)
	  */	
	void SetGammaRayBeam(const double &energy_);

	/** Set a beam of mono-energetic optical photons
	  * @param energy_ Energy of the beam (in MeV)
	  */	
	void SetLaserBeam(const double &energy_);

	/** Set a beam of mono-energetic electrons
	  * @param energy_ Energy of the beam (in MeV)
	  */	
	void SetElectronBeam(const double &energy_);

	/** Set the default interpolation method for energy distributions
	  * @param method The G4SPSEneDistribution interpolation method to use
	  */
	void SetInterpolationMethod(const G4String &method){ interpolationMethod = method; }

	/** Read an energy distribution from an ascii file
	  * @param filename Path to an ascii file containing pairs of data [Energy(MeV), Intensity]
	  * @return True if the file was read successfully and return false otherwise
	  */
	bool ReadEnergyFile(const char *filename);

	/** Read a reaction setup file (see Reaction::Read())
	  * @param filename Path to an ascii file containing reaction parameters
	  * @return True if the file was read successfully and return false otherwise
	  */
	bool ReadReactionFile(const G4String &filename);

	/** Add a new energy level to the current source spectrum
	  * @param str Space-delimited string of the form: "<energy(keV)> [intensity] [particle]"
	  * @return True if the energy level was added successfully and return false otherwise
	  */
	bool AddDiscreteEnergy(const G4String &str);

	/** Add a new energy level to the current source spectrum
	  * @param energy Energy of the transition (in keV)
	  * @param intensity Intensity of the transition (a.u. but usually percent)
	  * @param isPair Flag indicating that this
	  * @param particle Geant particle definition for the transition (default=G4Gamma::GammaDefinition())
	  */
	void AddDiscreteEnergy(const G4double &energy, const G4double &intensity, const bool &isPair=false, G4ParticleDefinition *particle=NULL);

	/** Clear all defined sources
	  */
	void Reset();

	/** Update the position & rotation of the source and set the beam profile
	  */
	void UpdateAll();

	/** Simulate a specified number of events and write particle energies to an output file
	  * @param str Space-delimited string of the form: "<filename> <Nevents>"
	  * @return True if the file is written successfully and return false otherwise
	  */
	bool Test(const G4String &str);

	/** Simulate a specified number of events and write particle energies to an output file
	  * @param filename Desired filename of the ouptut ROOT file
	  * @param Nevents The number of events to simulate
	  * @return True if the file is written successfully and return false otherwise
	  */
	bool Test(const char *filename, const size_t &Nevents);

	/** Sample the current source spectrum
	  * @param evt Pointer to a G4Event. If NULL, generate a new event
	  * @return Particle kinetic energy from the spectrum (in MeV)
	  */
	double Sample(G4Event *evt=NULL);

	/** Sample the current source and print energies (and angles if applicable) to stdout
	  * @param Nsamples The number of times to sample the source
	  * @return The final sample of the energy (in MeV)
	  */
	double Print(const size_t &Nsamples=1);

	/** Generate primary particles
	  * @param anEvent Pointer to the current event
	  */
	virtual void GeneratePrimaries(G4Event* anEvent);

  private:
	nDetParticleSourceMessenger *fSourceMessenger; ///< Pointer to the messenger for this class
	
	G4ThreeVector unitX; ///< X-axis unit vector in the frame of the source
	G4ThreeVector unitY; ///< Y-axis unit vector in the frame of the source
	G4ThreeVector unitZ; ///< Z-axis unit vector in the frame of the source

	G4ThreeVector sourceOrigin; ///< Position of the center of the source
	
	int beamspotType; ///< Integer indicating the type of beamspot to use for sampling

	double beamspot; ///< Radius of the beamspot (in mm)
	double beamspot0; ///< Secondary radius of the beamspot (in mm)	
	
	G4RotationMatrix rot; ///< Rotation of the source

	double targThickness; ///< Physical thickness of the target (in mm)
	double targEnergyLoss; ///< Slope equal to the energy loss of the primary particle as a function of distance in the target (in MeV/mm)
	double targTimeSlope; ///< Slope equal to the primary particle time straggling as a function of distance in the target (in ns/mm)
	double targTimeOffset; ///< Time offset due to primary particle straggling in the target (in ns)
	double beamE0; ///< Initial source energy (in MeV)

	bool useReaction; ///< Flag indicating that energy and angle sampling should use a user specified reaction
	bool isotropic; ///< Flag indicating that the source is isotropic
	bool back2back; ///< Flag indicating that two back-to-back particles will be generated for every event

	Reaction *particleRxn; ///< Pointer to a reaction object used for sampling reaction product particle energies and angles

	G4ThreeVector detPos; ///< 3d position of the detector used for isotropic sources
	G4ThreeVector detSize; ///< 3d size of the detector used for isotropic sources
	G4RotationMatrix detRot; ///< Rotation of the detector used for isotropic sources

	size_t sourceIndex; ///< Current index in the vector of G4SingleParticleSources
	size_t numSources; ///< Number of currently defined sources

	G4String interpolationMethod; ///< Interpolation method to use for G4SPSEneDistribution class

	std::vector<G4SingleParticleSource*> allSources; ///< Vector of all single particle sources (needed because G4GeneralParticleSource::sourceVector is private)

	double b2bEnergy[2]; ///< Array containing particle energies for back-to-back particle emitter state

	std::mutex generatorLock; ///< Mutex lock for thread-safe TTree filling

	/** Default constructor (private for singleton class)
	  */
	nDetParticleSource(nDetConstruction *det=NULL);

	/** Get the next G4SingleParticleSource in the vector of all sources
	  * @return A pointer to the next source (starting from the zeroth) or return NULL if the end of the vector has been reached
	  */
	G4SingleParticleSource *nextSource();

	/** Add a new single particle source
	  * @param intensity The intensity of the new source
	  * @return A pointer to the newly added G4SingleParticleSource
	  */
	G4SingleParticleSource *addNewSource(const G4double &intensity=1);

	/** Compute the intensity of a 252Cf source for a given neutron energy
	  * @param E_ Energy of the neutron (in MeV)
	  * @return The number density of neutrons at the specified energy
	  */
	double cf252(const double &E_) const ;
	
	/** Set the profile of the beam based on user settings
	  * @param src Pointer to the G4SingleParticleSource which will have its profile set
	  */
	void setBeamProfile(G4SingleParticleSource *src);
	
	/** Modify a generated event by making it isotropic
	  * @param vertex A pointer to a G4PrimaryVertex which was generated by GeneratePrimaries()
	  */
	void generateIsotropic(G4PrimaryVertex *vertex);
};

class nDetPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
  public:
	nDetPrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() { }
	
	~nDetPrimaryGeneratorAction(){ }
	
	/** Generate primary particles
	  * @param anEvent Pointer to the current event
	  */
	virtual void GeneratePrimaries(G4Event* anEvent);
};

#endif
