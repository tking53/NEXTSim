#ifndef NDET_MASTER_OUTPUT_FILE_MESSENGER_HH
#define NDET_MASTER_OUTPUT_FILE_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetMasterOutputFile;

class nDetMasterOutputFileMessenger : public messengerHandler {
  public:
	nDetMasterOutputFileMessenger();

	nDetMasterOutputFileMessenger(nDetMasterOutputFile *action);
	
	virtual void SetNewValue(G4UIcommand *command, G4String newValue);

  private:
	nDetMasterOutputFile *fOutputFile;
	
	void addAllCommands();
};

#endif
