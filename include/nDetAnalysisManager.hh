//
// Created by David Pérez Loureiro on 2/6/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETANALYSISMANAGER_HH
#define NEXTMULTIPLESCATTERING_NDETANALYSISMANAGER_HH

#include "G4RootAnalysisManager.hh"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include <vector>
#include "G4Run.hh"
#include "G4Event.hh"
#include "time.h"

class nDetAnalysisManager:public G4RootAnalysisManager {

    public:

    nDetAnalysisManager();
    ~nDetAnalysisManager();

    TTree* GetTree(){return fTree;}
    void SetTree(TTree* theTree){fTree=theTree;}
    void OpenROOTFile(const G4String fileName);
    void WriteFile();
    void CloseROOTFile();
    void FillTree();
    void ResetEvent();

    void BeginOfRunAction(const G4Run *aRun);
    void EndOfRunAction(const G4Run *aRun);

    void BeginOfEventAction(const G4Event *anEvent);
    void EndOfEventAction(const G4Event *anEvent);

    void OnceAWhileDoIt(const G4bool DoItNow=false);

private:

    G4String fFileName;

    TFile *fFile;

    TTree *fTree;

    G4long fRunNb;
    G4long fEventNb;

    time_t LastDoItTime;


    G4double     neutronIncidentPositionX;
    G4double     neutronIncidentPositionY;
    G4double     neutronIncidentPositionZ;

    G4double     depEnergy; // energy deposition inside of the EJ200 scintillator


    std::vector<double>     fvPrimaryPhotonPositionX;
    std::vector<double>     fvPrimaryPhotonPositionY;
    std::vector<double>     fvPrimaryPhotonPositionZ;
    std::vector<double>     fvPrimaryPhotonTime;


    std::vector<double>     fvSDPhotonPositionX;
    std::vector<double>     fvSDPhotonPositionY;
    std::vector<double>     fvSDPhotonPositionZ;
    std::vector<double>     fvSDPhotonTime;
    std::vector<int>        fvSDNumber;


    std::vector<std::string> fparticleName;
    std::vector<double>      fparticleCharge;



};



class NEXTData:public TObject{

public:

    NEXTData();
    ~NEXTData();
    void Reset();
    void Fill();

ClassDef(NEXTData,1);
};


#endif //NEXTMULTIPLESCATTERING_NDETANALYSISMANAGER_HH

