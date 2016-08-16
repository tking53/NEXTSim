//
// $Id: nDetSteppingAction.hh,v 1.0 Sept., 2015 $
// by Dr. Xiaodong Zhang
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "nDetSteppingAction.hh"

//#include "nDetConstruction.hh"
//#include "nDetEventAction.hh"
#include "nDetSD.hh"
#include "G4Step.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpAbsorption.hh"
#include "G4ProcessManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

#include "G4ParticleDefinition.hh"
#include "G4Alpha.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Triton.hh"

#define DEBUG 0

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetSteppingAction::nDetSteppingAction( 
					nDetConstruction* det,
					nDetRunAction* runAct,
                                        nDetEventAction* evtAct)
//:runAction(runAct), evtAction(evtAct)
:detector(det), runAction(runAct), evtAction(evtAct)
{

  eventID = -1;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetSteppingAction::~nDetSteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetSteppingAction::UserSteppingAction(const G4Step* aStep)
{

  // collect energy step by step only if 
  // the energy deposited in the CsI are recorded.
  // used for debug...
  //G4cout<<aStep->GetTrack()->GetVolume()->GetName()<<G4endl;

  G4String name = aStep->GetTrack()->GetMaterial()->GetName();

  if( (name.find("EJ") != name.npos ) && aStep->GetTotalEnergyDeposit() > 0 ){
    G4double edep = aStep->GetTotalEnergyDeposit();

//Xiaodong says we can put some code here.  GetParticleName 
//step->track->particlename

  //  G4String pname = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();
  //  G4cout << pname << G4endl;

    /* 
    G4cout<<G4BestUnit(edep,"Energy")<<"**";
    G4cout<<aStep->GetTrack()->GetMaterial()->GetName()<<G4endl;
    */
    evtAction->AddDepE(edep);
  }

  // collect detected photons in the siPM
  G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  G4OpBoundaryProcess* boundary=NULL;
  if(!boundary){
      G4ProcessManager* pm
        = aStep->GetTrack()->GetDefinition()->GetProcessManager();       
      G4int nprocesses = pm->GetProcessListLength();
      G4ProcessVector* pv = pm->GetProcessList();
      G4int i;
        for( i=0;i<nprocesses;i++){
          if((*pv)[i]->GetProcessName()=="OpBoundary"){
            boundary = (G4OpBoundaryProcess*)(*pv)[i];
            //G4cout<<boundary->GetStatus()<<G4endl;
            break;
          }
         }

    if(i<nprocesses){
      boundaryStatus = boundary->GetStatus();
      if(aStep->GetPostStepPoint()->GetStepStatus()==fGeomBoundary ){
        if(boundaryStatus == Detection){
          G4String vName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
          G4double time = aStep->GetPostStepPoint()->GetGlobalTime();
          //G4cout<<"Detect one photon in SiPM"<< vName<<" Global time: "<<time<<" at the position of "<<aStep->GetPostStepPoint()->GetPosition().y()<<G4endl;

          if(vName.find("psSiPM")&&aStep->GetPostStepPoint()->GetPosition().y()>0) {
		runAction->vTimeOfPhotonInSD1PushBack(time);
                runAction->vSD1PhotonPositionXPushBack(aStep->GetPostStepPoint()->GetPosition().x());
                runAction->vSD1PhotonPositionYPushBack(aStep->GetPostStepPoint()->GetPosition().y());
		runAction->vSD1PhotonPositionZPushBack(aStep->GetPostStepPoint()->GetPosition().z());
	  }
          if(vName.find("psSiPM")&&aStep->GetPostStepPoint()->GetPosition().y()<0) {
		runAction->vTimeOfPhotonInSD2PushBack(time);
                runAction->vSD2PhotonPositionXPushBack(aStep->GetPostStepPoint()->GetPosition().x());
                runAction->vSD2PhotonPositionYPushBack(aStep->GetPostStepPoint()->GetPosition().y());
                runAction->vSD2PhotonPositionZPushBack(aStep->GetPostStepPoint()->GetPosition().z());
          }
       }
      }
    }
    else{
      return;
    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

