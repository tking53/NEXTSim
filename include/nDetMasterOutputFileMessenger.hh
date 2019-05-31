#ifndef NDET_MASTER_OUTPUT_FILE_MESSENGER_HH
#define NDET_MASTER_OUTPUT_FILE_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetMasterOutputFile;

/*! \class nDetMasterOutputFileMessenger
 *  \brief Class used to handle user commands for nDetMasterOutputFile class
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class nDetMasterOutputFileMessenger : public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetMasterOutputFileMessenger();

	/** Constructor taking a pointer to a nDetMasterOutputFile object
	  * @param detector Pointer to the nDetMasterOutputFile object for which this messenger is defined
	  */
	nDetMasterOutputFileMessenger(nDetMasterOutputFile *action);

	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand *command, G4String newValue);

  private:
	nDetMasterOutputFile *fOutputFile; ///< Pointer to the output file object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
