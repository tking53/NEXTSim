#ifndef NDETRUNACTIONMESSENGER_HH
#define NDETRUNACTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetRunAction;
class nDetMasterOutputFile;

class nDetRunActionMessenger: public messengerHandler {
  public:
	nDetRunActionMessenger();

	nDetRunActionMessenger(nDetRunAction *action);
	
	virtual void SetNewValue(G4UIcommand *command, G4String newValue);

  private:
	nDetRunAction *fAction;
	nDetMasterOutputFile *fOutputFile;
	
	void addAllCommands();
};


#endif //NDETRUNACTIONMESSENGER_HH
