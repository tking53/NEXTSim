//
// Created by David Pérez Loureiro on 12/7/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETUSEREVENTINFORMATION_HH
#define NEXTMULTIPLESCATTERING_NDETUSEREVENTINFORMATION_HH

#include "G4VUserEventInformation.hh"
#include "globals.hh"

class nDetUserEventInformation:public G4VUserEventInformation{

public:
    nDetUserEventInformation();
    virtual ~nDetUserEventInformation();
    inline virtual void Print()const;

    void IncDetections(){fDetections++;}
    void IncAbsortions(){fAbsortions++;}
    G4int GetDetectionCount()const {return fDetections;}
    G4int GetAbsortionCount()const {return fAbsortions;}


private:
    G4int fDetections;
    G4int fAbsortions;

};

#endif //NEXTMULTIPLESCATTERING_NDETUSEREVENTINFORMATION_HH
