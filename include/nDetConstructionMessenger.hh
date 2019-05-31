#ifndef NDET_CONSTRUCTION_MESSENGER_HH
#define NDET_CONSTRUCTION_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetConstruction;

/*! \class nDetConstructionMessenger
 *  \brief Class used to handle user commands for nDetConstruction class
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class nDetConstructionMessenger : public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetConstructionMessenger() : messengerHandler("nDetConstructionMessenger"), fDetector(NULL) { addAllCommands(); }

	/** Constructor taking a pointer to a nDetConstruction object
	  * @param detector Pointer to the nDetConstruction object for which this messenger is defined
	  */
	nDetConstructionMessenger(nDetConstruction* detector) : messengerHandler("nDetConstructionMessenger"), fDetector(detector) { addAllCommands(); }
	
	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand* command, G4String newValue);

  private:
	nDetConstruction *fDetector; ///< Pointer to the detector object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
