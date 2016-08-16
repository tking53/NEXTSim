#ifndef LENSLongGeneralPhysics_h
#define LENSLongGeneralPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4Decay.hh"

class LENSLongGeneralPhysics : public G4VPhysicsConstructor
{
  public: 
    LENSLongGeneralPhysics(const G4String& name = "general");
    virtual ~LENSLongGeneralPhysics();

  public: 

    virtual void ConstructParticle();
 

    virtual void ConstructProcess();

  protected:
    G4Decay fDecayProcess;
};


#endif // LENSLongGeneralPhysics_h








