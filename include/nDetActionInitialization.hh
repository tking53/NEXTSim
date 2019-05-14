#ifndef NDET_ACTION_INITIALIZATION_HH
#define NDET_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class nDetActionInitialization : public G4VUserActionInitialization {
  public:
	nDetActionInitialization(bool verboseMode=false);
  
	virtual void Build() const ;
	
	virtual void BuildForMaster() const ;

  private:
	bool verbose;
};

#endif
