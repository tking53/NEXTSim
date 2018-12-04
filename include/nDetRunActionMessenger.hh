//
// Created by David Pérez Loureiro on 3/31/17.
//

#ifndef NDETRUNACTIONMESSENGER_HH
#define NDETRUNACTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetRunAction;

class nDetRunActionMessenger: public messengerHandler {

public:
    nDetRunActionMessenger(nDetRunAction *action);
    
    virtual void SetNewValue(G4UIcommand *command, G4String newValue);

private:
    nDetRunAction *fAction;
};


#endif //NDETRUNACTIONMESSENGER_HH
