#ifndef PARTICLE_SOURCE_MESSENGER_HH
#define PARTICLE_SOURCE_MESSENGER_HH

#include "ParticleSource.hh"
#include "messengerHandler.hh"

class ParticleSourceMessenger: public messengerHandler {
  public:
	ParticleSourceMessenger() : messengerHandler("particleSourceMessenger"), fAction(NULL) { addAllCommands(); }

	ParticleSourceMessenger(ParticleSource* Gun) : messengerHandler("particleSourceMessenger"), fAction(Gun) { addAllCommands(); }
	
	virtual void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
	ParticleSource* fAction;
	
	void addAllCommands();
};

#endif
