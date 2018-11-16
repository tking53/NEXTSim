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

#include "nDetAnalysisManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "TH1F.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Timer;
class G4Run;

class nDetRunActionMessenger;
class nDetStackingAction;
class nDetTrackingAction;
class nDetSteppingAction;
class nDetConstruction;

class photonCounter;

class primaryTrackInfo{
  public:
    G4ThreeVector pos;
    G4ThreeVector dir;
    G4double kE, dkE;
    G4double gtime, plength;
    G4double angle;
    
    const G4ParticleDefinition *part;
    
    primaryTrackInfo(const G4Step *step);
    
    primaryTrackInfo(const G4Track *track);

    bool compare(const G4Track *rhs);

    double getAngle(const G4Track *rhs);

    double getAngle(const G4ThreeVector &rhs);

	double getPathLength(const G4ThreeVector &rhs);

    void print();
};

class nDetRunAction : public G4UserRunAction
{
  public:
    short runNb;
    short eventNb;
    short nScatters;

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
    double depEnergy; // energy deposition inside of the EJ200 scintillator
    double initEnergy; // Initial energy of the neutron (CRT)
    
    bool nAbsorbed;

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
    
    unsigned short photonArrivalTimes[100];
    
    unsigned int nPhotonsTot;
    unsigned int nPhotonsDet[2];
    
    double photonDetCenterOfMassX[2];
    double photonDetCenterOfMassY[2];
    double photonDetCenterOfMassZ[2];
    double photonMinArrivalTime[2];
    double photonAvgArrivalTime[2];

    double photonDetEfficiency;
    
    nDetRunAction(nDetConstruction *det);
    
    virtual ~nDetRunAction();

    void BeginOfRunAction(const G4Run* aRun);
    
    void EndOfRunAction(const G4Run* aRun);

    bool fillBranch(); // deposited energy in YAP

    // clear all the vector for the next event
    void vectorClear();

	void setOutputFilename(const std::string &fname);

	void setOutputTreeName(const std::string &tname){ treename = tname; }
	
	void setPersistentMode(const bool &enabled){ persistentMode = enabled; }
	
	void setOutputEnabled(const bool &enabled){ outputEnabled = enabled; }
	
	void setOutputFileTitle(const std::string &title){ runTitle = title; }	

	void setOutputFileIndex(const G4int &index){ runIndex = index; }
	
	void setOverwriteOutputFile(const bool &overwrite){ overwriteExistingFile = overwrite; }

	void setActions(nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_);

    bool toggleVerboseMode(){ return (verbose = !verbose); }

    void scatterEvent(const G4Track *track);

    void initializeNeutron(const G4Step *step);

    void scatterNeutron(const G4Step *step);

    void finalizeNeutron(const G4Step *step);

  private:  // function member
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
    bool outputEnabled;
    bool verbose;

    nDetAnalysisManager *fAnalysisManager;
    nDetRunActionMessenger *fActionMessenger;
    
    nDetStackingAction *stacking;
    nDetTrackingAction *tracking;
    nDetSteppingAction *stepping;
    nDetConstruction *detector;    
    
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
