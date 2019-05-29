#ifndef NDET_RUN_ACTION_MESSENGER_HH
#define NDET_RUN_ACTION_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetRunAction;

class nDetRunActionMessenger: public messengerHandler {
  public:
	nDetRunActionMessenger();

	nDetRunActionMessenger(nDetRunAction *action);
	
	virtual void SetNewChildValue(G4UIcommand *command, G4String newValue);

  private:
	nDetRunAction *fAction; ///< Pointer to the user run action object for which this messenger is defined
	
	void addAllCommands();
};

#endif
