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

class nDetRunAction : public G4UserRunAction
{
  public:
    short       runNb;
    short       eventNb;

    double     neutronIncidentPositionX;
    double     neutronIncidentPositionY;
    double     neutronIncidentPositionZ;

    double     depEnergy; // energy deposition inside of the EJ200 scintillator
    double     initEnergy; // Initial energy of the neutron (CRT)

    std::vector<double>     vTimeOfPhotonInSD1;
    std::vector<double>     vTimeOfPhotonInSD2;
    std::vector<double>     vTimeOfPhotonInEJ200;

    // record the position of the initial photons inside the EJ200 scintillator
    std::vector<double>     vPrimaryPhotonPositionX;
    std::vector<double>     vPrimaryPhotonPositionY;
    std::vector<double>     vPrimaryPhotonPositionZ;  

    std::vector<double>     vSD1PhotonPositionX;
    std::vector<double>     vSD1PhotonPositionY;
    std::vector<double>     vSD1PhotonPositionZ;

    std::vector<double>     vSD2PhotonPositionX;
    std::vector<double>     vSD2PhotonPositionY;
    std::vector<double>     vSD2PhotonPositionZ;

    std::vector<std::string> particleName;
    std::vector<double>      particleCharge;
  
    nDetRunAction();
    
    virtual ~nDetRunAction();

    void BeginOfRunAction(const G4Run* aRun);
    
    void EndOfRunAction(const G4Run* aRun);

    bool fillBranch(); // deposited energy in YAP

    // clear all the vector for the next event
    void vectorClear(){
			vTimeOfPhotonInSD1.clear();
                        vTimeOfPhotonInSD2.clear();
                        vTimeOfPhotonInEJ200.clear();
			vPrimaryPhotonPositionX.clear();
			vPrimaryPhotonPositionY.clear();
                        vPrimaryPhotonPositionZ.clear(); 
                        vSD1PhotonPositionX.clear();
                        vSD1PhotonPositionY.clear();
                        vSD1PhotonPositionZ.clear();
                        vSD2PhotonPositionX.clear();
                        vSD2PhotonPositionY.clear();
                        vSD2PhotonPositionZ.clear();
                        particleName.clear();
                        particleCharge.clear();
			};

	void setFilename(const std::string &fname){ filename = fname; }

	void setTreeName(const std::string &tname){ treename = tname; }

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

    nDetAnalysisManager *fAnalysisManager;

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
