#ifndef PARTICLE_SOURCE_MESSENGER_HH
#define PARTICLE_SOURCE_MESSENGER_HH

#include "ParticleSource.hh"
#include "messengerHandler.hh"

class ParticleSourceMessenger: public messengerHandler {
  public:
    ParticleSourceMessenger(ParticleSource* Gun);
    
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
    ParticleSource* fAction;
};

#endif
