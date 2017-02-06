//
// Created by David Pérez Loureiro on 2/6/17.
//

#include "nDetAnalysisManager.hh"

nDetAnalysisManager::nDetAnalysisManager(){

}


nDetAnalysisManager::~nDetAnalysisManager(){

        delete fTree;
        delete fFile;


}

void nDetAnalysisManager::OpenROOTFile( const G4String fileName){

    fFileName=fileName;

    fFile = new TFile(fileName,"RECREATE");

    G4cout << "nDetAnalysisManager::OpenROOTFile()->" << fFileName << " has been opened." << G4endl;
    
    fTree=new TTree("theTree","NEXTSim Output Tree");

    fTree->SetAutoSave(1000000000); // autosave when 1 Gbyte written

    //if(defineRootBranch == false){
    G4int bufsize = 64000;
    fTree->Branch("runNb", &fRunNb, "runNb/L", bufsize);
    fTree->Branch("eventNb", &fEventNb, "enevtNb/L", bufsize);

    fTree->Branch("neutronIncidentPositionX",&neutronIncidentPositionX,"neutronIncidentPositionX/D",bufsize);
    fTree->Branch("neutronIncidentPositionY",&neutronIncidentPositionY,"neutronIncidentPositionY/D",bufsize);
    fTree->Branch("neutronIncidentPositionZ",&neutronIncidentPositionZ,"neutronIncidentPositionZ/D",bufsize);

    fTree->Branch("depositedEnergy", &depEnergy, "depEnergy/D", bufsize);


    fTree->Branch("vPrimaryPhotonPositionX", &vPrimaryPhotonPositionX);
    fTree->Branch("vPrimaryPhotonPositionY", &vPrimaryPhotonPositionY);
    fTree->Branch("vPrimaryPhotonPositionZ", &vPrimaryPhotonPositionZ);
    fTree->Branch("vTimeOfPhotonInEJ200", &vPrimaryPhotonTime);

    fTree->Branch("vSDPhotonPositionX", &vSDPhotonPositionX);
    fTree->Branch("vSDPhotonPositionY", &vSDPhotonPositionY);
    fTree->Branch("vSDPhotonPositionZ", &vSDPhotonPositionZ);
    fTree->Branch("vSDPhotonTime", &vSDPhotonTime);
    fTree->Branch("vSDDetectorNumber", &vSDNumber);

    //Following branches are added by Kyle.
    fTree->Branch("particleCharge", &particleCharge);
    fTree->Branch("particleName", &particleName);


}

void nDetAnalysisManager::WriteFile() {

    fTree->Write();

    G4cout << "nDetAnalysisManager::OpenROOTFile()->" << fFileName << " has been written." << G4endl;


}


void nDetAnalysisManager::CloseROOTFile() {

    fFile->Close();
}