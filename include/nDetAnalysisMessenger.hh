//
// Created by David Pérez Loureiro on 3/31/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETANALYSISMESSENGER_HH
#define NEXTMULTIPLESCATTERING_NDETANALYSISMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class nDetAnalysisManager;
class G4UIdirectory;
class G4UIcmdWithAString;

class nDetAnalysisMessenger: public G4UImessenger {

public:
    nDetAnalysisMessenger(nDetAnalysisManager *);
    virtual ~nDetAnalysisMessenger();
    virtual void SetNewValue(G4UIcommand*, G4String);

private:
    nDetAnalysisManager *fManager;
    G4UIdirectory *fAnalysisDir;
    G4UIcmdWithAString *fOutputFileCmd;
};


#endif //NEXTMULTIPLESCATTERING_NDETANALYSISMESSENGER_HH
