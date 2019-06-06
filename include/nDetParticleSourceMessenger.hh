#ifndef NDET_PARTICLE_SOURCE_MESSENGER_HH
#define NDET_PARTICLE_SOURCE_MESSENGER_HH

#include "nDetParticleSource.hh"
#include "messengerHandler.hh"

/*! \class nDetParticleSourceMessenger
 *  \brief Class used to handle user commands for ParticleSource class
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class nDetParticleSourceMessenger: public messengerHandler {
  public:
	/** Default constructor
	  */
	nDetParticleSourceMessenger() : messengerHandler("particleSourceMessenger"), fAction(NULL) { addAllCommands(); }

	/** Constructor taking a pointer to a ParticleSource object
	  * @param detector Pointer to the ParticleSource object for which this messenger is defined
	  */
	nDetParticleSourceMessenger(nDetParticleSource* Gun) : messengerHandler("particleSourceMessenger"), fAction(Gun) { addAllCommands(); }

	/** Handle commands called by the user. Called from messengerHandler::SetNewValue()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewChildValue(G4UIcommand* command, G4String newValue);

  private:
	nDetParticleSource* fAction; ///< Pointer to the particle source object for which this messenger is defined

	/** Add all commands to the messenger
	  */
	void addAllCommands();
};

#endif
