#ifndef NDET_CONSTRUCTION_MESSENGER_HH
#define NDET_CONSTRUCTION_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetConstruction;

class nDetConstructionMessenger : public messengerHandler {
  public:
	nDetConstructionMessenger() : messengerHandler("nDetConstructionMessenger"), fDetector(NULL) { addAllCommands(); }
	
	nDetConstructionMessenger(nDetConstruction* detector) : messengerHandler("nDetConstructionMessenger"), fDetector(detector) { addAllCommands(); }
	
	virtual void SetNewChildValue(G4UIcommand* command, G4String newValue);

  private:
	nDetConstruction *fDetector; ///< Pointer to the detector object for which this messenger is defined
	
	void addAllCommands();
};

#endif
