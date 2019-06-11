#ifndef NDET_RUN_ACTION_HH
#define NDET_RUN_ACTION_HH

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"

#include "nDetConstruction.hh"
#include "nDetDataPack.hh"
#include "centerOfMass.hh"

class G4Timer;
class G4Run;

class nDetRunActionMessenger;
class nDetEventAction;
class nDetStackingAction;
class nDetTrackingAction;
class nDetSteppingAction;
class nDetConstruction;
class pmtResponse;

class photonCounter;
class nDetParticleSource;

class primaryTrackInfo{
  public:
	G4ThreeVector pos; ///< 3d position of scatter (in mm)
	G4ThreeVector dir; ///< 3d momentum direction of primary particle prior to scattering
	G4double kE; ///< Kinetic energy of primary particle prior to scattering (in MeV)
	G4double dkE; ///< Change in kinetic energy of primary particle during scatter event (in MeV)
	G4double gtime; ///< Global time of scatter event (in ns)
	G4double plength; ///< Path length of primary particle between successive scatter events (in mm)
	G4double angle; ///< Angle between incoming and outgoing primary particle (in degrees)

	G4int copyNum; ///< Copy number of object inside which the scatter event occured
	G4int trackID; ///< Geant track ID of the primary particle track
	G4int atomicMass; ///< Atomic mass of the recoiling particle after the scatter event (NOT WORKING)
	
	G4bool inScint; ///< Flag indicating that the scatter event occured in a scintillator material

	const G4ParticleDefinition *part; ///< Pointer to the Geant particle definition of the primary particle
	
	primaryTrackInfo(const G4Step *step);
	
	primaryTrackInfo(const G4Track *track);

	bool compare(const G4Track *rhs);

	double getAngle(const G4Track *rhs);

	double getAngle(const G4ThreeVector &rhs);

	double getPathLength(const G4ThreeVector &rhs);

	void print();
	
  private:
	void setValues(const G4Track *track);
};

class nDetRunAction : public G4UserRunAction
{
  public:
	nDetRunAction();
	
	virtual ~nDetRunAction();

	void BeginOfRunAction(const G4Run* aRun);
	
	void EndOfRunAction(const G4Run* aRun);

	nDetParticleSource *getParticleSource(){ return source; }

	void setActions(nDetEventAction *event_, nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_);

	void setBaselinePercentage(const double &percentage){ baselineFraction = percentage/100; }
	
	void setBaselineJitterPercentage(const double &percentage){ baselineJitterFraction = percentage/100; }

	void setPolyCfdFraction(const double &frac){ polyCfdFraction = frac; }
	
	void setPulseIntegralLow(const short &low){ pulseIntegralLow = low; }
	
	void setPulseIntegralHigh(const short &high){ pulseIntegralHigh = high; }
	
	void setEventNumber(const int &eventID){ data.eventID = eventID; }
	
	void setPrintTrace(const bool &enabled){ printTrace = enabled; }
	
	void setOutputDebug(const bool &enabled){ outputDebug = enabled; }
	
	bool toggleVerboseMode(){ return (verbose = !verbose); }
	
    void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);

    void setPmtSpectralResponse(centerOfMass *left, centerOfMass *right);

	void setPmtGainMatrix(centerOfMass *left, centerOfMass *right);
	
	std::vector<centerOfMass> *getCenterOfMassLeft(){ return &cmL; }
	  
	std::vector<centerOfMass> *getCenterOfMassRight(){ return &cmR; }

	pmtResponse *getPmtResponseLeft(const size_t &index=0){ return cmL.at(index).getPmtResponse();	}

	pmtResponse *getPmtResponseRight(const size_t &index=0){	return cmR.at(index).getPmtResponse(); }

	pmtResponse *getAnodeResponseLeft(const size_t &index=0){ return cmL.at(index).getAnodeResponse();	}

	pmtResponse *getAnodeResponseRight(const size_t &index=0){ return cmR.at(index).getAnodeResponse(); }
	
	nDetRunActionMessenger *getMessenger(){ return fActionMessenger; }

    unsigned long long getNumPhotons() const { return numPhotonsTotal; }
    
    unsigned long long getNumPhotonsDet() const { return numPhotonsDetTotal; }

	void updateDetector(nDetConstruction *det);

	G4int checkCopyNumber(const G4int &num) const ;

	bool getSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const ;

	void process();

	bool AddDetectedPhoton(const G4Step *step, const double &mass=1);

	void initializeNeutron(const G4Step *step);

	void scatterNeutron(const G4Step *step);

	void finalizeNeutron(const G4Step *step);

  private:
	G4Timer* timer; ///< Timer used to measure the total time for a run (in seconds)

	bool outputDebug; ///< Flag indicating that the user has requested low-level debug to be written to the output file
	bool verbose; ///< Verbosity flag
	bool printTrace; ///< Flag indicating that the left and right digitized PMT traces will be printed to the screen

	double baselineFraction; ///< Digitized light pulse baseline as a fraction of the total ADC dynamic range
	double baselineJitterFraction; ///< Digitized light pulse baseline jitter as a fraction of the total ADC dynamic range
	double polyCfdFraction; ///< PolyCFD F parameter as a fraction of max pulse height minus the baseline
	
	short pulseIntegralLow; ///< Number of ADC clock ticks for start of digitized pulse TQDC integral wrt pulse maximum
	short pulseIntegralHigh; ///< Number of ADC clock ticks for stop of digitized pulse TQDC integral wrt pulse maximum

	nDetRunActionMessenger *fActionMessenger; ///< Pointer to the messenger object used for this class
	
	nDetEventAction *eventAction; ///< Pointer to the thread-local user event action
	nDetStackingAction *stacking; ///< Pointer to the thread-local user stacking action
	nDetTrackingAction *tracking; ///< Pointer to the thread-local user tracking action
	nDetSteppingAction *stepping; ///< Pointer to the thread-local user stepping action
	
	nDetConstruction *detector; ///< Pointer to the global detector singleton
	nDetParticleSource *source; ///< Pointer to the thread-local particle source
	
	std::vector<primaryTrackInfo> primaryTracks; ///< Vector of primary particle scatter events
	
	G4ThreeVector prevDirection; ///< Momentum direction of the previous primary particle scatter
	G4ThreeVector prevPosition; ///< Position of the previous primary particle scatter
	
	nDetDataPack data; ///< Data structure used for storing event information

	photonCounter *counter; ///< Counter used to record the total number of optical photons produced by scattering

	std::vector<centerOfMass> cmL; ///< Object used to compute the optical photon detection center-of-mass for the left PMT
	std::vector<centerOfMass> cmR; ///< Object used to compute the optical photon detection center-of-mass for the right PMT
	std::vector<int> cmCounts; ///< Vector of optical photon counts for all defined detectors

    unsigned long long numPhotonsTotal; ///< Total number of simulated optical photons (thread-local)
    unsigned long long numPhotonsDetTotal; ///< Total number of detected optical photons (thread-local)

	std::vector<userAddDetector> userDetectors; ///< Vector of detectors added by the user

	bool scatterEvent();	
};

#endif
