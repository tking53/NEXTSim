//
// Created by David Pérez Loureiro on 1/31/17.
//

#ifndef NDETCONSTRUCTIONMESSENGER_HH
#define NDETCONSTRUCTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

#include "messengerHandler.hh"

class nDetConstruction;

class nDetConstructionMessenger : public messengerHandler {
  public:
	//Constructor
	nDetConstructionMessenger(nDetConstruction* detector);
	
	void SetNewValue(G4UIcommand* command, G4String newValue);

  private:
    nDetConstruction *fDetector;
};

#endif //NDETCONSTRUCTIONMESSENGER_HH
