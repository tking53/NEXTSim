//
// Created by David Pérez Loureiro on 2/6/17.
//

#include "nDetAnalysisManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "nDetSD.hh"
#include "SiPMSD.hh"
#include "G4SystemOfUnits.hh"

#include "TSystem.h"
nDetAnalysisManager::nDetAnalysisManager(){

    G4cout << "nDetAnalysisManager::~nDetAnalysisManager()->"  << G4endl;

}


nDetAnalysisManager::~nDetAnalysisManager(){


    G4cout << "nDetAnalysisManager::~nDetAnalysisManager()->"  << G4endl;


    //delete fTree;
     //   delete fFile;


}

void nDetAnalysisManager::OpenROOTFile( const G4String fileName){

    fFileName=fileName;

    fFile = new TFile(fileName,"RECREATE");

    G4cout << "nDetAnalysisManager::OpenROOTFile()->" << fFileName << " has been opened." << G4endl;

    if(gSystem)
        gSystem->ProcessEvents();

    fTree=new TTree("theTree","NEXTSim Output Tree");

    fTree->SetAutoSave(1000000000); // autosave when 1 Gbyte written

    //if(defineRootBranch == false){
    G4int bufsize = 64000;
    fTree->Branch("runNb", &fRunNb, "runNb/L", bufsize);
    fTree->Branch("eventNb", &fEventNb, "enevtNb/L", bufsize);

//    fTree->Branch("neutronIncidentPositionX",&neutronIncidentPositionX,"neutronIncidentPositionX/D",bufsize);
//    fTree->Branch("neutronIncidentPositionY",&neutronIncidentPositionY,"neutronIncidentPositionY/D",bufsize);
//    fTree->Branch("neutronIncidentPositionZ",&neutronIncidentPositionZ,"neutronIncidentPositionZ/D",bufsize);

    fTree->Branch("depositedEnergy", &depEnergy, "depEnergy/D", bufsize);


      fTree->Branch("vPrimaryPhotonPositionX", &fvPrimaryPhotonPositionX);
      fTree->Branch("vPrimaryPhotonPositionY", &fvPrimaryPhotonPositionY);
      fTree->Branch("vPrimaryPhotonPositionZ", &fvPrimaryPhotonPositionZ);
//    fTree->Branch("vTimeOfPhotonInEJ200", &fvPrimaryPhotonTime);

//    fTree->Branch("vSDPhotonPositionX", &fvSDPhotonPositionX);
//    fTree->Branch("vSDPhotonPositionY", &fvSDPhotonPositionY);
//    fTree->Branch("vSDPhotonPositionZ", &fvSDPhotonPositionZ);
//    fTree->Branch("vSDPhotonTime", &fvSDPhotonTime);
//    fTree->Branch("vSDDetectorNumber", &vfSDNumber);

    //Following branches are added by Kyle.
//    fTree->Branch("particleCharge", &fparticleCharge);
//    fTree->Branch("particleName", &fparticleName);

    G4cout << "nDetAnalysisManager::OpenROOTFile()->" << fTree->GetName() << " has been created." << G4endl;

    OnceAWhileDoIt();

    return;

}

void nDetAnalysisManager::WriteFile() {

    fTree->Write();

    G4cout << "nDetAnalysisManager::WriteFile()->" << fFileName << " has been written." << G4endl;


}


void nDetAnalysisManager::CloseROOTFile() {

    fFile->Close();
}

void nDetAnalysisManager::FillTree(){

    G4cout<<"Filling Tree-> "<<fTree->GetName()<<G4endl;
    //fTree->Print();
    fTree->Fill();
}


void nDetAnalysisManager::ResetEvent() {

    G4cout<<"nDetAnalysisManager::ResetEvent()"<<G4endl;

    if(gSystem)
        gSystem->ProcessEvents();

    fEventNb=0;

    neutronIncidentPositionX=0;
    neutronIncidentPositionY=0;
    neutronIncidentPositionZ=0;

    fvPrimaryPhotonPositionX.resize(1);
    fvPrimaryPhotonPositionY.resize(1);
    fvPrimaryPhotonPositionZ.resize(1);

    depEnergy=0; // energy deposition inside of the EJ200 scintillator
//    G4cout<<"fvPrimaryPhotonPositionX "<<fvPrimaryPhotonPositionX.size()<<G4endl;
//    G4cout<<"fvPrimaryPhotonPositionY "<<fvPrimaryPhotonPositionY.size()<<G4endl;
//    G4cout<<"fvPrimaryPhotonPositionZ "<<fvPrimaryPhotonPositionZ.size()<<G4endl;


    G4cout<<"nDetAnalysisManager::ResetEvent()->Clearing Vectors..."<<G4endl;
    //fvPrimaryPhotonPositionX.clear();
    std::vector<double>().swap(fvPrimaryPhotonPositionX);
    G4cout<<"fvPrimaryPhotonPositionX "<<fvPrimaryPhotonPositionX.size()<<G4endl;
    //fvPrimaryPhotonPositionY.clear();
    std::vector<double>().swap(fvPrimaryPhotonPositionY);
    G4cout<<"fvPrimaryPhotonPositionY "<<fvPrimaryPhotonPositionY.size()<<G4endl;
    //fvPrimaryPhotonPositionZ.clear();
    std::vector<double>().swap(fvPrimaryPhotonPositionZ);
    G4cout<<"fvPrimaryPhotonPositionZ "<<fvPrimaryPhotonPositionZ.size()<<G4endl;
    //fvPrimaryPhotonTime.clear();


//    fvSDPhotonPositionX.clear();
//    fvSDPhotonPositionY.clear();
//    fvSDPhotonPositionZ.clear();
//    fvSDPhotonTime.clear();
//    fvSDNumber.clear();
//
//
//    fparticleName.clear();
//    fparticleCharge.clear();
    G4cout<<"nDetAnalysisManager::ResetEvent()->Vectors cleared"<<G4endl;

    G4cout<<"nDetAnalysisManager::ResetEvent()->HERE"<<G4endl;

    //OnceAWhileDoIt();
    return;
}


void nDetAnalysisManager::BeginOfRunAction(const G4Run *aRun) {

    //SetVerboseLevel(2);

    G4cout<<"nDetAnalysisManager::BeginOfRunAction()"<<G4endl;


    fRunNb=aRun->GetRunID();


    OpenROOTFile("DPL_test.root");

    ResetEvent();

    if(gSystem) {
       gSystem->ProcessEvents();
    }
    //G4cout<<"HERE!"<<G4endl;

    OnceAWhileDoIt(true);

    //G4cout<<"HERE2!"<<G4endl;
    return;

    }


void nDetAnalysisManager::EndOfRunAction(const G4Run *aRun) {

    G4cout<<"nDetAnalysisManager::EndOfRunAction()"<<G4endl;

    if(gSystem)
        gSystem->ProcessEvents();

    WriteFile();
    CloseROOTFile();

    OnceAWhileDoIt(true);
    return;


}


void nDetAnalysisManager::BeginOfEventAction(const G4Event *anEvent) {

    G4cout<<"nDetAnalysisManager::BeginOfEventAction()"<<G4endl;

    if(gSystem)
        gSystem->ProcessEvents();


    ResetEvent();




    fEventNb=anEvent->GetEventID();


    OnceAWhileDoIt();

    return;
 }



void nDetAnalysisManager::EndOfEventAction(const G4Event *anEvent){

    G4cout<<"nDetAnalysisManager::EndOfEventAction()"<<G4endl;


    G4SDManager *man=G4SDManager::GetSDMpointer();

    G4int ScintCollectionID=man->GetCollectionID("SciCollection");

    G4int SiPMCollectionID=man->GetCollectionID("SiPMCollection");

    //G4cout<<"ScintCollectionID->"<<ScintCollectionID<<G4endl;
    //G4cout<<"SiPMCollectionID->"<<SiPMCollectionID<<G4endl;

    G4HCofThisEvent *HCE=anEvent->GetHCofThisEvent();

    nDetHitsCollection *DHC_Sci=0;
    SiPMHitsCollection *DHC_SiPM=0;

    if(HCE) {

        DHC_Sci=(nDetHitsCollection*)(HCE->GetHC(ScintCollectionID));
        DHC_SiPM=(SiPMHitsCollection*)(HCE->GetHC(SiPMCollectionID));
    }

    if(DHC_Sci){

        G4int NbHits=DHC_Sci->entries();

        G4cout<<"nDetAnalysisManager::EndOfEventAction()->Nb of Hits in Scint "<<NbHits<<G4endl;

        for(Int_t i=0;i<NbHits;i++){

            G4ThreeVector pos = (*DHC_Sci)[i]->GetPos();
            G4double time = (*DHC_Sci)[i]->GetTime()/ns;
            G4double energy=(*DHC_Sci)[i]->GetEdep()/keV;
            depEnergy+=energy;
            fvPrimaryPhotonPositionX.push_back(pos.x()/mm);
            fvPrimaryPhotonPositionY.push_back(pos.y()/mm);
            fvPrimaryPhotonPositionZ.push_back(pos.z()/mm);
            //(*DHC_Sci)[i]->Print();
        }




    }

    if(DHC_SiPM) {

        G4int NbHits = DHC_SiPM->entries();

        G4cout << "nDetAnalysisManager::EndOfEventAction()->Nb of Hits in SiPM " << NbHits << G4endl;

       for (Int_t i = 0; i < NbHits; i++) {

            G4ThreeVector pos = (*DHC_SiPM)[i]->GetPos();
            G4double time = (*DHC_SiPM)[i]->GetTime() / ns;
            G4int detector=(*DHC_SiPM)[i]->GetSiPMNumber();
            //(*DHC_SiPM)[i]->Print();

//           fvSDPhotonPositionX.push_back(pos.x()/mm);
//           fvSDPhotonPositionY.push_back(pos.y()/mm);
//           fvSDPhotonPositionZ.push_back(pos.z()/mm);
//           fvSDPhotonTime.push_back(time);
//           fvSDNumber.push_back(detector);

       }

    }
    else{
        G4cout << "nDetAnalysisManager::EndOfEventAction()->No Hits in SiPM !"<< G4endl;

    }
    FillTree();
    //ResetEvent();

    OnceAWhileDoIt();

    return;

}


void nDetAnalysisManager::OnceAWhileDoIt(const G4bool DoItNow) {
    time_t Now = time(0); // get the current time (measured in seconds)
    if ( (!DoItNow) && (LastDoItTime > (Now - 10)) ) return; // every 10 seconds
    LastDoItTime = Now;

    G4cout<<"Doing it!"<<G4endl;

    if (gSystem) gSystem->ProcessEvents();

}