#ifndef NDET_ACTION_INITIALIZATION_HH
#define NDET_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class G4MTRunManager;

class nDetConstruction;
class nDetRunAction;
class nDetEventAction;
class nDetStackingAction;
class nDetSteppingAction;
class nDetTrackingAction;

class nDetActionInitialization : public G4VUserActionInitialization {
  public:
	nDetActionInitialization();
  
	virtual void Build() const ;
	
	virtual void BuildForMaster() const ;

	nDetRunAction *getRunAction(){ return runAction; }
	
	nDetEventAction *getEventAction(){ return eventAction; }
	
	nDetSteppingAction *getSteppingAction(){ return steppingAction; }
	
	nDetStackingAction *getStackingAction(){ return stackingAction; }
	
	nDetTrackingAction *getTrackingAction(){ return trackingAction; }

  private:
	nDetRunAction *runAction;
	nDetEventAction *eventAction;
	nDetSteppingAction *steppingAction;
	nDetStackingAction *stackingAction;
	nDetTrackingAction *trackingAction;
	
	G4MTRunManager *masterRunManager;
};

#endif
