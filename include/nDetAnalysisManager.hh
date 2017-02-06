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

class nDetAnalysisManager:public G4RootAnalysisManager {

    public:

    nDetAnalysisManager();
    ~nDetAnalysisManager();

    TTree* GetTree(){return fTree;}
    void SetTree(TTree* theTree){fTree=theTree;}
    void OpenROOTFile(const G4String fileName);
    void WriteFile();
    void CloseROOTFile();



    private:

    G4String fFileName;

    TFile *fFile;

    TTree *fTree;

    G4long fRunNb;
    G4long fEventNb;


    G4double     neutronIncidentPositionX;
    G4double     neutronIncidentPositionY;
    G4double     neutronIncidentPositionZ;

    G4double     depEnergy; // energy deposition inside of the EJ200 scintillator


    std::vector<double>     vPrimaryPhotonPositionX;
    std::vector<double>     vPrimaryPhotonPositionY;
    std::vector<double>     vPrimaryPhotonPositionZ;
    std::vector<double>     vPrimaryPhotonTime;


    std::vector<double>     vSDPhotonPositionX;
    std::vector<double>     vSDPhotonPositionY;
    std::vector<double>     vSDPhotonPositionZ;
    std::vector<double>     vSDPhotonTime;
    std::vector<int>        vSDNumber;


    std::vector<std::string> particleName;
    std::vector<double>      particleCharge;



};

#endif //NEXTMULTIPLESCATTERING_NDETANALYSISMANAGER_HH

