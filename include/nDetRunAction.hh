//
// $Id: nDetRunAction.hh,v1.0 Sept., 2015 $
//   Written by Dr. Xiaodong Zhang
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef nDetRunAction_h
#define nDetRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "TH1F.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
	// Run information.
    short runNb;

    // Normal neutron output.
    short nScatters;

    double depEnergy; // energy deposition inside of the EJ200 scintillator
    double initEnergy; // Initial energy of the neutron (CRT)
    
    bool nAbsorbed;
    bool goodEvent;

	// Neutron debug output variables.
    double neutronIncidentPositionX;
    double neutronIncidentPositionY;
    double neutronIncidentPositionZ;
	double neutronExitPositionX;
	double neutronExitPositionY;
	double neutronExitPositionZ;	

	double nTimeToFirstScatter;
	double nLengthToFirstScatter;
	double incidentTime;
	double timeInMaterial;
	
    std::vector<double> nScatterX;
    std::vector<double> nScatterY;
    std::vector<double> nScatterZ;
    std::vector<double> nScatterAngle;
    std::vector<double> nPathLength;
    std::vector<double> impartedE;
    std::vector<double> scatterTime;
    std::vector<short> segmentCol;
    std::vector<short> segmentRow;
    std::vector<short> Nphotons;
    std::vector<short> recoilMass;

    // Normal photon output.
    double photonDetEfficiency;
	double photonLightBalance;    
    double barCenterOfMassX;
    double barCenterOfMassY;

    float barTOF;
    float barQDC;    
    float barMaxADC;
	float pulsePhase[2];

	double photonDetCenterOfMassX[2];
	double photonDetCenterOfMassY[2];
	double reconstructedCenterX[2];
	double reconstructedCenterY[2];

    short centerOfMassColumn[2];
    short centerOfMassRow[2];
    
    // Photon debug output variables.
    unsigned int nPhotonsTot;
    unsigned int nPhotonsDetTot;
    unsigned int nPhotonsDet[2];

    double photonDetCenterOfMassZ[2];
    double photonMinArrivalTime[2];
    double photonAvgArrivalTime[2];

    float pulseMax[2];
    float pulseQDC[2];
    
    double anodeCurrent[2][4];
    
	double neutronCenterOfMass[3];
	double neutronWeight;
	double detSpeedLight;
    
    nDetRunAction(nDetConstruction *det);
    
    virtual ~nDetRunAction();

    void BeginOfRunAction(const G4Run* aRun);
    
    void EndOfRunAction(const G4Run* aRun);

	pmtResponse *getPmtResponseLeft();

	pmtResponse *getPmtResponseRight();
	
	ParticleSource *getSource(){ return source; }

    bool fillBranch();

    void vectorClear();

	void setOutputFilename(const std::string &fname);

	void setOutputTreeName(const std::string &tname){ treename = tname; }
	
	void setPersistentMode(const bool &enabled){ persistentMode = enabled; }
	
	void setOutputEnabled(const bool &enabled){ outputEnabled = enabled; }
	
	void setOutputFileTitle(const std::string &title){ runTitle = title; }	

	void setOutputFileIndex(const G4int &index){ runIndex = index; }
	
	void setOutputTraces(const bool &enabled){ outputTraces = enabled; }
	
	void setOutputDebug(const bool &enabled){ outputDebug = enabled; }
	
	void setOutputBadEvents(const bool &enabled){ outputBadEvents = enabled; }
	
	void setOverwriteOutputFile(const bool &overwrite){ overwriteExistingFile = overwrite; }

	void setActions(nDetEventAction *event_, nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_);

	void setBaselinePercentage(const double &percentage){ baselineFraction = percentage/100; }
	
	void setBaselineJitterPercentage(const double &percentage){ baselineJitterFraction = percentage/100; }

	void setPolyCfdFraction(const double &frac){ polyCfdFraction = frac; }
	
	void setPulseIntegralLow(const short &low){ pulseIntegralLow = low; }
	
	void setPulseIntegralHigh(const short &high){ pulseIntegralHigh = high; }

    bool toggleVerboseMode(){ return (verbose = !verbose); }

	void process();

    void initializeNeutron(const G4Step *step);

    void scatterNeutron(const G4Step *step);

    void finalizeNeutron(const G4Step *step);

  private:  // function member
    bool scatterEvent();

    bool openRootFile(const G4Run* aRun);
    
    bool closeRootFile();
  
  private:  // data member
    G4Timer* timer;  // able to measure elasped user/system process time.

    std::string filename;
    std::string treename;
    
    std::string filenamePrefix;
    std::string filenameSuffix;
    
    TFile *fFile; // define root file
    TTree *fTree; // tree and its branches
    TBranch *fBranch;
    
    bool defineRootBranch;
    bool persistentMode;
    bool verbose;
    
	bool outputEnabled;
    bool outputTraces;
    bool outputDebug;
    bool outputBadEvents;

	double baselineFraction;
	double baselineJitterFraction;
	double polyCfdFraction;
	
	short pulseIntegralLow;
	short pulseIntegralHigh;

	std::vector<unsigned short> lightPulseL;
	std::vector<unsigned short> lightPulseR;

    nDetRunActionMessenger *fActionMessenger;
    
    nDetEventAction *eventAction;
    nDetStackingAction *stacking;
    nDetTrackingAction *tracking;
    nDetSteppingAction *stepping;
    nDetConstruction *detector;  
    ParticleSource *source;  
    
    std::vector<primaryTrackInfo> primaryTracks;
    
    photonCounter *counter;
    
    G4ThreeVector prevDirection;
    G4ThreeVector prevPosition;
    
    std::string runTitle;
    G4int runIndex;
    
    bool overwriteExistingFile;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*nDetRunAction_h*/

// Class description:
//
// This is  the base class of a user's action class which defines the 
// user's action at the begining and the end of each run. The user can
// override the following two methods but the user should not change 
// any of the contents of G4Run object.

// Description of G4Run
// This class represents a run. An object of this class is constructed 
// and deleted by G4RunManager. Basically the user should use only 
// the get methods. All properties are set by G4RunManager.
//
