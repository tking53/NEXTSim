#ifndef nDetRunAction_h
#define nDetRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"

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
class ParticleSource;

class primaryTrackInfo{
  public:
	G4ThreeVector pos;
	G4ThreeVector dir;
	G4double kE, dkE;
	G4double gtime, plength;
	G4double angle;

	G4int copyNum;
	G4int trackID;
	G4int atomicMass;
	
	G4bool inScint;

	const G4ParticleDefinition *part;
	
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
	nDetRunAction(nDetConstruction *det);
	
	virtual ~nDetRunAction();

	void BeginOfRunAction(const G4Run* aRun);
	
	void EndOfRunAction(const G4Run* aRun);

	ParticleSource *getSource(){ return source; }

	void setActions(nDetEventAction *event_, nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_);

	void setBaselinePercentage(const double &percentage){ baselineFraction = percentage/100; }
	
	void setBaselineJitterPercentage(const double &percentage){ baselineJitterFraction = percentage/100; }

	void setPolyCfdFraction(const double &frac){ polyCfdFraction = frac; }
	
	void setPulseIntegralLow(const short &low){ pulseIntegralLow = low; }
	
	void setPulseIntegralHigh(const short &high){ pulseIntegralHigh = high; }
	
	centerOfMass *getCenterOfMassLeft(){ return &cmL; }
	  
	centerOfMass *getCenterOfMassRight(){ return &cmR; }

	pmtResponse *getPmtResponseLeft(){ return cmL.getPmtResponse();	}

	pmtResponse *getPmtResponseRight(){	return cmR.getPmtResponse(); }

	pmtResponse *getAnodeResponseLeft(){ return cmL.getAnodeResponse();	}

	pmtResponse *getAnodeResponseRight(){ return cmR.getAnodeResponse(); }

	void process();

	bool AddDetectedPhoton(const G4Step *step, const double &mass=1);

	void initializeNeutron(const G4Step *step);

	void scatterNeutron(const G4Step *step);

	void finalizeNeutron(const G4Step *step);

  private:
	G4Timer* timer;

	bool outputDebug;
	bool verbose;

	double baselineFraction;
	double baselineJitterFraction;
	double polyCfdFraction;
	
	short pulseIntegralLow;
	short pulseIntegralHigh;

	nDetRunActionMessenger *fActionMessenger;
	
	nDetEventAction *eventAction;
	nDetStackingAction *stacking;
	nDetTrackingAction *tracking;
	nDetSteppingAction *stepping;
	nDetConstruction *detector;  
	ParticleSource *source;  
	
	std::vector<primaryTrackInfo> primaryTracks;
	
	G4ThreeVector prevDirection;
	G4ThreeVector prevPosition;
	
	nDetDataPack data;

	photonCounter *counter;

	centerOfMass cmL;
	centerOfMass cmR;

	bool scatterEvent();	
};

#endif
