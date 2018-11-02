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

class nDetStackingAction;
class nDetTrackingAction;
class nDetSteppingAction;

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
    std::vector<short> Nphotons;
    std::vector<short> recoilMass;
    
    unsigned short photonArrivalTimes[100];
    
    unsigned int nPhotonsTot;
    unsigned int nPhotonsDet[2];
    
    double photonDetCenterOfMassX[2];
    double photonDetCenterOfMassY[2];
    double photonDetCenterOfMassZ[2];

    double photonDetEfficiency;
    double photonMinArrivalTime;
    double photonAvgArrivalTime;
    
    /*std::vector<double> vTimeOfPhotonInSD1;
    std::vector<double> vTimeOfPhotonInSD2;
    std::vector<double> vTimeOfPhotonInEJ200;

    // record the position of the initial photons inside the EJ200 scintillator
    std::vector<double> vPrimaryPhotonPositionX;
    std::vector<double> vPrimaryPhotonPositionY;
    std::vector<double> vPrimaryPhotonPositionZ;  

    std::vector<double> vSD1PhotonPositionX;
    std::vector<double> vSD1PhotonPositionY;
    std::vector<double> vSD1PhotonPositionZ;

    std::vector<double> vSD2PhotonPositionX;
    std::vector<double> vSD2PhotonPositionY;
    std::vector<double> vSD2PhotonPositionZ;

    std::vector<std::string> particleName;
    std::vector<double> particleCharge;*/

    nDetRunAction();
    
    virtual ~nDetRunAction();

    void BeginOfRunAction(const G4Run* aRun);
    
    void EndOfRunAction(const G4Run* aRun);

    bool fillBranch(); // deposited energy in YAP

    // clear all the vector for the next event
    void vectorClear();

	void setFilename(const std::string &fname){ filename = fname; }

	void setTreeName(const std::string &tname){ treename = tname; }

	void setActions(nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_);

    bool toggleVerboseMode(){ return (verbose = !verbose); }

    void scatterEvent(const G4Track *track);

    void initializeNeutron(const G4Step *step);

    void scatterNeutron(const G4Step *step);

    void finalizeNeutron(const G4Step *step);

  private:  // function member
    bool openRootFile(const G4Run* aRun); 
  
  private:  // data member
    G4Timer* timer;  // able to measure elasped user/system process time.

    std::string filename;
    std::string treename;
    TFile        *fFile;    // define root file
    TTree        *fTree;    // tree and its branches
    TBranch      *fBranch;
    
    bool defineRootBranch; 
    bool verbose;

    nDetAnalysisManager *fAnalysisManager;
    nDetStackingAction *stacking;
    nDetTrackingAction *tracking;
    nDetSteppingAction *stepping;
    
    std::vector<primaryTrackInfo> primaryTracks;
    
    photonCounter *counter;
    
    G4ThreeVector prevDirection;
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
