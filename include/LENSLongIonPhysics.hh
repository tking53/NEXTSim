#ifndef LENSLongIonPhysics_h
#define LENSLongIonPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4HadronElasticProcess.hh"
#include "G4LElastic.hh"

#include "G4DeuteronInelasticProcess.hh"
#include "G4LEDeuteronInelastic.hh"

#include "G4TritonInelasticProcess.hh"
#include "G4LETritonInelastic.hh"

#include "G4AlphaInelasticProcess.hh"
#include "G4LEAlphaInelastic.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4MultipleScattering.hh"

class LENSLongIonPhysics : public G4VPhysicsConstructor
{
  public: 
    LENSLongIonPhysics(const G4String& name="ion");
    virtual ~LENSLongIonPhysics();

  public: 

    virtual void ConstructParticle();

    virtual void ConstructProcess();

  protected:

   G4HadronElasticProcess      theElasticProcess;
   G4LElastic*                 theElasticModel;


   G4MultipleScattering        fIonMultipleScattering;
   G4ionIonisation             fIonIonisation;


   G4MultipleScattering        fDeuteronMultipleScattering;
   G4hIonisation               fDeuteronIonisation;
   G4DeuteronInelasticProcess  fDeuteronProcess;
   G4LEDeuteronInelastic*      fDeuteronModel;

   G4MultipleScattering        fTritonMultipleScattering;
   G4hIonisation               fTritonIonisation;
   G4TritonInelasticProcess    fTritonProcess;
   G4LETritonInelastic*        fTritonModel;

   G4MultipleScattering        fAlphaMultipleScattering;
   G4hIonisation               fAlphaIonisation;
   G4AlphaInelasticProcess     fAlphaProcess;
   G4LEAlphaInelastic*         fAlphaModel;

   G4MultipleScattering        fHe3MultipleScattering;
   G4hIonisation               fHe3Ionisation;

};


#endif // LENSLongIonPhysics_h

