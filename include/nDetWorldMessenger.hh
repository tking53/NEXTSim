#ifndef NDET_WORLD_MESSENGER_HH
#define NDET_WORLD_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetWorld;

/** @class nDetWorldMessenger
  * @brief Handles user commands for nDetWorld class
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date August 14, 2019
  */

class nDetWorldMessenger : public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetWorldMessenger() : messengerHandler("nDetWorldMessenger"), fWorld(NULL) { addAllCommands(); }

	/** Constructor taking a pointer to a nDetWorld object
	  * @param world Pointer to the nDetWorld object for which this messenger is defined
	  */
	nDetWorldMessenger(nDetWorld* world) : messengerHandler("nDetWorldMessenger"), fWorld(world) { addAllCommands(); }
	
	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand* command, G4String newValue);

  private:
	nDetWorld *fWorld; ///< Pointer to the world object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
