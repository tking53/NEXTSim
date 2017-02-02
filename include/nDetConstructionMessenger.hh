//
// Created by David Pérez Loureiro on 1/31/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETCONSTRUCTIONMESSENGER_HH
#define NEXTMULTIPLESCATTERING_NDETCONSTRUCTIONMESSENGER_HH 1


#include "G4UImessenger.hh"
#include "globals.hh"




class nDetConstruction;
class G4UIcommand;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIdirectory;

class nDetConstructionMessenger:public G4UImessenger
{
public:

//Constructor
nDetConstructionMessenger(nDetConstruction*);
//Destructor
virtual ~nDetConstructionMessenger();

virtual void SetNewValue(G4UIcommand*,G4String);

private:

    nDetConstruction* fDetector;
    G4UIdirectory*    fDetectorDir;

    G4UIcmdWithAString*  fGeometryCmd;
    G4UIcmdWith3VectorAndUnit *fSiliconDimensionsCmd;
    G4UIcommand*         fUpdateCmd;

};


#endif //NEXTMULTIPLESCATTERING_NDETCONSTRUCTIONMESSENGER_HH
