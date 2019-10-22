#ifndef NDET_MATERIALS_MESSENGER_HH
#define NDET_MATERIALS_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetMaterials;

/** @class nDetMaterialsMessenger
  * @brief Class used to handle user commands for nDetMaterials class
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date August 30, 2019
  */

class nDetMaterialsMessenger : public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetMaterialsMessenger() : messengerHandler("nDetMaterialsMessenger"), fMaterials(NULL) { addAllCommands(); }

	/** Constructor taking a pointer to a nDetMaterials object
	  * @param materials Pointer to the nDetMaterials object for which this messenger is defined
	  */
	nDetMaterialsMessenger(nDetMaterials* materials) : messengerHandler("nDetMaterialsMessenger"), fMaterials(materials) { addAllCommands(); }
	
	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand* command, G4String newValue);

  private:
	nDetMaterials *fMaterials; ///< Pointer to the materials object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
