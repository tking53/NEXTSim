//
// Created by David Pérez Loureiro on 3/31/17.
//

#ifndef NDETRUNACTIONMESSENGER_HH
#define NDETRUNACTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class nDetRunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class nDetRunActionMessenger: public G4UImessenger {

public:
    nDetRunActionMessenger(nDetRunAction *action);
    
    virtual ~nDetRunActionMessenger();
    
    virtual void SetNewValue(G4UIcommand *command, G4String newValue);

private:
    nDetRunAction *fAction;
    
    G4UIdirectory *fOutputDir;
    
    G4UIcmdWithAString *fOutputFileCmd[4];
    
    G4UIcmdWithAnInteger *fOutputFileIndex;
};


#endif //NDETRUNACTIONMESSENGER_HH
