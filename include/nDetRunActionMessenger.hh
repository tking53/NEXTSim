#ifndef NDET_RUN_ACTION_MESSENGER_HH
#define NDET_RUN_ACTION_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetRunAction;

/*! \class nDetRunActionMessenger
 *  \brief Class used to handle user commands for nDetRunAction class
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class nDetRunActionMessenger: public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetRunActionMessenger();

	/** Constructor taking a pointer to a nDetRunAction object
	  * @param detector Pointer to the nDetRunAction object for which this messenger is defined
	  */
	nDetRunActionMessenger(nDetRunAction *action);

	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand *command, G4String newValue);

  private:
	nDetRunAction *fAction; ///< Pointer to the user run action object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
