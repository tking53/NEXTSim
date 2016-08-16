#ifndef LENSLongIndiumTinNeutrinoPhysics_h
#define LENSLongIndiumTinNeutrinoPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"


class LENSLongIndiumTinNeutrinoPhysics : public G4VPhysicsConstructor
{
  public: 
    LENSLongIndiumTinNeutrinoPhysics( const G4String& name="indiumTinNeutrino" );
    virtual ~LENSLongIndiumTinNeutrinoPhysics();

  public: 

    virtual void ConstructParticle();

    virtual void ConstructProcess();

  protected:

};


#endif // LENSLongIndiumTinNeutrinoPhysics_h

