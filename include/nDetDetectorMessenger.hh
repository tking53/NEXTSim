#ifndef NDET_DETECTOR_MESSENGER_HH
#define NDET_DETECTOR_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetDetectorParams;

/** @class nDetDetectorMessenger
  * @brief Handles user commands for nDetDetector class
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 16, 2019
*/

class nDetDetectorMessenger : public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetDetectorMessenger() : messengerHandler("nDetDetectorMessenger"), fDetector(NULL) { addAllCommands(); }

	/** Constructor taking a pointer to a nDetDetector object
	  * @param detector Pointer to the nDetDetector object for which this messenger is defined
	  */
	nDetDetectorMessenger(nDetDetectorParams* detector) : messengerHandler("nDetDetectorMessenger"), fDetector(detector) { addAllCommands(); }
	
	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand* command, G4String newValue);

  private:
	nDetDetectorParams *fDetector; ///< Pointer to the detector object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
