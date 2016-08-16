#ifndef LENSLongParticles_h
#define LENSLongParticles_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4ParticleDefinition.hh"
#include "G4Ions.hh"

class LENSLongTin497 : public G4Ions
{
 private:
   static LENSLongTin497 theTin497;
   
 private:
   LENSLongTin497(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable
   );

 public:
   virtual ~LENSLongTin497();

   static LENSLongTin497* Tin497Definition();
   static LENSLongTin497* Tin497();
};


#endif // LENSLongParticles_h

