
//
// $Id: nDetStackingAction.hh,v1.0 Sept., 2015 $
//   Written by Xiaodong Zhang
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef nDetStackingAction_H
#define nDetStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"

#include "nDetEventAction.hh"
#include "nDetRunAction.hh"

// Class description, G4UserStackingAction, is the base class of one 
// of the user's optional classes. it gives the hooks for 
// G4StackingManager which controls the stacks of G4Track objects.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class nDetStackingAction : public G4UserStackingAction
{
  public:
    nDetStackingAction(nDetRunAction*);
   ~nDetStackingAction();

  public:
    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    void NewStage();
    void PrepareNewEvent();

  private:
    nDetRunAction* runAct;
    G4int photonNb;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

// Note that:
//
// Here some very detailed explanations about the data and function of
// this class are included.
//
// In the virtual function, G4Class..... ClassifyNewTrack(....),
// enum G4ClassificationOfNewTrack
// {
//   fUrgent, 	// put into the urgent stack
//   fWaiting, 	// put into the waiting stack
//   fPostphone,// postphone to the next event
//   fKill	// kill without stacking
// };
//
// The parent_ID of the track indicates the origin of it.
// 
// G4int parent_ID = aTrack->get_parentID();
//
// parent_ID = 0 : primary particle
//           > 0 : secondary particle
//           < 0 : postphoned from the previous event
//------------------------------------------------------------------------
// 
// Note for the method, NewStage(),
// This method is called by G4StackManager when the urgentStack becomnes
// empty and contens in the waitingStack are trantered to the urgentStack.
// Bote that this method is not called at the begining of each event,
// but "prepareNewEvent" is called.
//
// In case re-classification of the stacked tracks is needed,
// use the following method to request to G4StackManager.
//
// 	stackManager->ReClassify();
//
// All of the stacked tracks in the waitingStack will be re-classified
// by the "ClassifyNewTrack" method.
// To abort current event, use the following method.
//
//	stackManger->Clear();
//
// Note that this way is valid and safe only for the case it is called
// from this user class. The more global way of event abortion is
//
//	G4UImanager * UImanager = G4UImanager::GetUIpoiter();
//	UImanager->ApplyCommand("/event/abort");
//
//-------------------------------------------------------------------------
//
// Note for the method, PrepareNewEvent();
// This methos is claaed by G4StackManager at the begining of each event.
// Be careful that the urgentStack and the waitingStack of G4StackManager 
// are empty at this moment, because this method is called before accepting
// primary particles. Also, note that the postphoneStack of G4StackManager
// may have some postponed.
//
//------------------------------------------------------------------------- 
