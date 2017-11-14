//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Apr  7 13:36:46 2017 by ROOT version 5.34/36
// from TTree theTree/NEXTSim Output Tree
// found on file: Out.root
//////////////////////////////////////////////////////////

#ifndef Anl_hex2_h
#define Anl_hex2_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

using namespace std;
// Header file for the classes stored in the TTree if any.
//#include "/Users/dploureiro/ROOT/root/cint/cint/lib/prec_stl/vector"
//#include "/Users/dploureiro/ROOT/root/cint/cint/lib/prec_stl/vector"

// Fixed size dimensions of array or collections stored in the TTree if any.

class Anl_hex2 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Long64_t        runNb;
   Long64_t        eventNb;
   Double_t        neutronIncidentPositionX;
   Double_t        neutronIncidentPositionY;
   Double_t        neutronIncidentPositionZ;
   Double_t        depositedEnergy;
   Double_t        firstEnergy;
   Int_t           NumberofPhotons;
   Int_t           NumberofDetectedPhotons;
   vector<double>  *vPrimaryPhotonPositionX;
   vector<double>  *vPrimaryPhotonPositionY;
   vector<double>  *vPrimaryPhotonPositionZ;
   vector<double>  *vTimeOfPhotonInEJ200;
   vector<int>     *vPhotonTrackIDEJ200;
   vector<double>  *vSDPhotonPositionX;
   vector<double>  *vSDPhotonPositionY;
   vector<double>  *vSDPhotonPositionZ;
   vector<double>  *vSDPhotonTime;
   vector<int>     *vSDDetectorNumber;
   vector<int>     *vSDTrackID;

   // List of branches
   TBranch        *b_runNb;   //!
   TBranch        *b_enevtNb;   //!
   TBranch        *b_neutronIncidentPositionX;   //!
   TBranch        *b_neutronIncidentPositionY;   //!
   TBranch        *b_neutronIncidentPositionZ;   //!
   TBranch        *b_depEnergy;   //!
   TBranch        *b_fEnergy;   //!
   TBranch        *b_Ngammas;   //!
   TBranch        *b_NgammasDet;   //!
   TBranch        *b_vPrimaryPhotonPositionX;   //!
   TBranch        *b_vPrimaryPhotonPositionY;   //!
   TBranch        *b_vPrimaryPhotonPositionZ;   //!
   TBranch        *b_vTimeOfPhotonInEJ200;   //!
   TBranch        *b_vPhotonTrackIDEJ200;   //!
   TBranch        *b_vSDPhotonPositionX;   //!
   TBranch        *b_vSDPhotonPositionY;   //!
   TBranch        *b_vSDPhotonPositionZ;   //!
   TBranch        *b_vSDPhotonTime;   //!
   TBranch        *b_vSDDetectorNumber;   //!
   TBranch        *b_vSDTrackID;   //!

   Anl_hex2(TTree *tree=0);
   virtual ~Anl_hex2();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Anl_hex2_cxx
Anl_hex2::Anl_hex2(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Out.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("Out.root");
      }
      f->GetObject("theTree",tree);

   }
   Init(tree);
}

Anl_hex2::~Anl_hex2()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Anl_hex2::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Anl_hex2::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Anl_hex2::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   vPrimaryPhotonPositionX = 0;
   vPrimaryPhotonPositionY = 0;
   vPrimaryPhotonPositionZ = 0;
   vTimeOfPhotonInEJ200 = 0;
   vPhotonTrackIDEJ200 = 0;
   vSDPhotonPositionX = 0;
   vSDPhotonPositionY = 0;
   vSDPhotonPositionZ = 0;
   vSDPhotonTime = 0;
   vSDDetectorNumber = 0;
   vSDTrackID = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNb", &runNb, &b_runNb);
   fChain->SetBranchAddress("eventNb", &eventNb, &b_enevtNb);
   fChain->SetBranchAddress("neutronIncidentPositionX", &neutronIncidentPositionX, &b_neutronIncidentPositionX);
   fChain->SetBranchAddress("neutronIncidentPositionY", &neutronIncidentPositionY, &b_neutronIncidentPositionY);
   fChain->SetBranchAddress("neutronIncidentPositionZ", &neutronIncidentPositionZ, &b_neutronIncidentPositionZ);
   fChain->SetBranchAddress("depositedEnergy", &depositedEnergy, &b_depEnergy);
   fChain->SetBranchAddress("firstEnergy", &firstEnergy, &b_fEnergy);
   fChain->SetBranchAddress("NumberofPhotons", &NumberofPhotons, &b_Ngammas);
   fChain->SetBranchAddress("NumberofDetectedPhotons", &NumberofDetectedPhotons, &b_NgammasDet);
   fChain->SetBranchAddress("vPrimaryPhotonPositionX", &vPrimaryPhotonPositionX, &b_vPrimaryPhotonPositionX);
   fChain->SetBranchAddress("vPrimaryPhotonPositionY", &vPrimaryPhotonPositionY, &b_vPrimaryPhotonPositionY);
   fChain->SetBranchAddress("vPrimaryPhotonPositionZ", &vPrimaryPhotonPositionZ, &b_vPrimaryPhotonPositionZ);
   fChain->SetBranchAddress("vTimeOfPhotonInEJ200", &vTimeOfPhotonInEJ200, &b_vTimeOfPhotonInEJ200);
   fChain->SetBranchAddress("vPhotonTrackIDEJ200", &vPhotonTrackIDEJ200, &b_vPhotonTrackIDEJ200);
   fChain->SetBranchAddress("vSDPhotonPositionX", &vSDPhotonPositionX, &b_vSDPhotonPositionX);
   fChain->SetBranchAddress("vSDPhotonPositionY", &vSDPhotonPositionY, &b_vSDPhotonPositionY);
   fChain->SetBranchAddress("vSDPhotonPositionZ", &vSDPhotonPositionZ, &b_vSDPhotonPositionZ);
   fChain->SetBranchAddress("vSDPhotonTime", &vSDPhotonTime, &b_vSDPhotonTime);
   fChain->SetBranchAddress("vSDDetectorNumber", &vSDDetectorNumber, &b_vSDDetectorNumber);
   fChain->SetBranchAddress("vSDTrackID", &vSDTrackID, &b_vSDTrackID);
   Notify();
}

Bool_t Anl_hex2::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Anl_hex2::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Anl_hex2::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Anl_hex2_cxx
