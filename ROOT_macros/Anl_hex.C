#define Anl_hex_cxx
#include "Anl_hex.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>

void Anl_hex::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L Anl_hex.C
//      Root > Anl_hex t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

  TH1F *htime[4];
  
  htime[0]=new TH1F("h_time0","Time Distribution in SiPM 0 ",500,0,50);
  htime[1]=new TH1F("h_time1","Time Distribution in SiPM 1",500,0,50);
  htime[2]=new TH1F("h_time2","Time Distribution in SiPM 2",500,0,50);
  htime[3]=new TH1F("h_time3","Time Distribution in SiPM 3",500,0,50);

  TH1F *hmin=new TH1F("h_min","Time Distribution in closest SiPM",500,0,50);
  TH1F *hmax=new TH1F("h_max","Time Distribution in furthest SiPM",500,0,50);
  TH1F *hothers=new TH1F("h_others","Time Distribution in middle SiPM",500,0,50);

  
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntries();
  //Long64_t nentries = 40;
  std::cout<<"Number of Events "<<nentries<<std::endl;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

     
      Int_t nHits=vSDPhotonTime->size();
      if(nHits>0){
	//std::cout<<"Number of Hits "<<nHits<<std::endl;

      Double_t average[4]={0};
      Int_t hits[4]={0};
      Double_t minimum=200;
      Double_t maximum=-10;
      Int_t kminimum=0;
      Int_t kmaximum=0;
      Double_t sum=0;
      Int_t count=0;
      for(Int_t j=0;j<nHits; j++){
	Int_t DetectorNumber=vSDDetectorNumber->at(j);
	//std::cout<<"Detector Number "<<DetectorNumber<<std::endl;
	Int_t index=DetectorNumber-10;
	average[index]+=vSDPhotonTime->at(j);
	hits[index]++;
      }

      for(Int_t k=0;k<4;k++){
	if(hits[k]>0){
	  average[k]/=hits[k];
	  if(average[k]<minimum){
	    minimum=average[k];
	    kminimum=k;
	  }
	  if(average[k]>maximum){
	    maximum=average[k];
	    kmaximum=k;
	  }
	  //std::cout<<average[k]<<" "<<maximum<<" "<<minimum<<std::endl;
	  htime[k]->Fill(average[k]);
	}
       }
      for(Int_t m=0;m<4;m++){
	if(m==kminimum)
	  hmin->Fill(average[m]);
	else if(m==kmaximum)
	  hmax->Fill(average[m]);
	else{
	  sum+=average[m];
	  count++;
	}
      }
      hothers->Fill(sum/count);
      }
      
   }
}
