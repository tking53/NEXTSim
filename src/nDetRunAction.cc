//
// $Id: nDetRunAction.cc,v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Make this appear first!
#include "G4Timer.hh"
#include "time.h"

// Selfdefine classes
#include "nDetRunAction.hh"

// geant4 class
#include "G4Run.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetRunAction::nDetRunAction()
{
    timer = new G4Timer;
    fAnalysisManager= (nDetAnalysisManager*)nDetAnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetRunAction::~nDetRunAction()
{
    delete timer;
    //delete fAnalysisManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl; 
  timer->Start();

  // open a root file.
  openRootFile(aRun);
    if(file)
    G4cout << "### File " << file->GetName() << " opened." << G4endl;

    // get RunId
  setRunNb(aRun->GetRunID());



 if (fAnalysisManager)
     fAnalysisManager->BeginOfRunAction(aRun);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetRunAction::EndOfRunAction(const G4Run* aRun)
{   
  timer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent() 
         << " " << *timer << G4endl;


    if (fAnalysisManager)
        fAnalysisManager->EndOfRunAction(aRun);


    // close the root file.
  if(file){
    if (IsMaster()){
        file->Write();
        file->Close();
        G4cout << "*** " << fileName << " is created." << G4endl;
    }
  }
  else
    G4cout << "*** No output file created." << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool nDetRunAction::openRootFile(const G4Run* aRun)
{
  if (IsMaster()){

  //The file is saved in the ./output/ folder and
  //this file is now becoming the current directory.
  //and then create a ROOT tree and branches

  // get the system time, and use it to create the filename of root file.
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[180];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime (buffer,180,"_%H:%M:%S_%a_%b_%d_%Y",timeinfo);

  // create a root file for the current run
  sprintf(fileName, "./output/run_%03d%s.root",aRun->GetRunID(), buffer);

  std::cout<<fileName<<std::endl;

  //create a ROOT file
  file = new TFile(fileName,"RECREATE","ROOT file for high resolution neutron detector simulation");

  //create root tree
  tree = new TTree("neutronEvent","Photons produced by thermal neutrons");
  tree->SetAutoSave(1000000000); // autosave when 1 Gbyte written

  //if(defineRootBranch == false){
    int bufsize = 64000;
    branch = tree->Branch("runNb", &runNb, "runNb/L", bufsize);
    branch = tree->Branch("eventNb", &eventNb, "enevtNb/L", bufsize);

    branch = tree->Branch("neutronIncidentPositionX",&neutronIncidentPositionX,"neutronIncidentPositionX/D",bufsize);
    branch = tree->Branch("neutronIncidentPositionY",&neutronIncidentPositionY,"neutronIncidentPositionY/D",bufsize);
    branch = tree->Branch("neutronIncidentPositionZ",&neutronIncidentPositionZ,"neutronIncidentPositionZ/D",bufsize);

    branch = tree->Branch("depositedEnergy", &depEnergy, "depEnergy/D", bufsize);

    branch = tree->Branch("vTimeOfPhotonInSD1", &vTimeOfPhotonInSD1);
    branch = tree->Branch("vTimeOfPhotonInSD2", &vTimeOfPhotonInSD2);
    branch = tree->Branch("vTimeOfPhotonInEJ200", &vTimeOfPhotonInEJ200);

    branch = tree->Branch("vPrimaryPhotonPositionX", &vPrimaryPhotonPositionX);
    branch = tree->Branch("vPrimaryPhotonPositionY", &vPrimaryPhotonPositionY);
    branch = tree->Branch("vPrimaryPhotonPositionZ", &vPrimaryPhotonPositionZ);

    branch = tree->Branch("vSD1PhotonPositionX", &vSD1PhotonPositionX);
    branch = tree->Branch("vSD1PhotonPositionY", &vSD1PhotonPositionY);
    branch = tree->Branch("vSD1PhotonPositionZ", &vSD1PhotonPositionZ);

    branch = tree->Branch("vSD2PhotonPositionX", &vSD2PhotonPositionX);
    branch = tree->Branch("vSD2PhotonPositionY", &vSD2PhotonPositionY);
    branch = tree->Branch("vSD2PhotonPositionZ", &vSD2PhotonPositionZ);

//Following branches are added by Kyle.
    branch = tree->Branch("particleCharge", &particleCharge);
    branch = tree->Branch("particleName", &particleName);

    defineRootBranch = true;
  //}
  }
  return false; // in case of success.
}//end of open root file...

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool nDetRunAction::fillBranch()
{
  tree->Fill();// fill the tree
  //nEvent++;

  return false; // in case of success
}//end of fill events method

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
