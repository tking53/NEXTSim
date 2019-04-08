#ifndef NDETCONSTRUCTIONMESSENGER_HH
#define NDETCONSTRUCTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetConstruction;

class nDetConstructionMessenger : public messengerHandler {
  public:
	nDetConstructionMessenger() : messengerHandler("nDetConstructionMessenger"), fDetector(NULL) { addAllCommands(); }
	
	nDetConstructionMessenger(nDetConstruction* detector) : messengerHandler("nDetConstructionMessenger"), fDetector(detector) { addAllCommands(); }
	
	virtual void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
	nDetConstruction *fDetector;
	
	void addAllCommands();
};

#endif //NDETCONSTRUCTIONMESSENGER_HH
