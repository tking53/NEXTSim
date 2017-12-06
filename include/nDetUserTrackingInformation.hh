//
// Created by David Pérez Loureiro on 12/5/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETUSERTRACKINGINFORMATION_HH
#define NEXTMULTIPLESCATTERING_NDETUSERTRACKINGINFORMATION_HH



#include "G4VUserTrackInformation.hh"
#include "globals.hh"


enum nDetTrackStatus { active=1, hitPMT=2, absorbed=4, boundaryAbsorbed=8,inactive=14};

/*nDetTrackStatus:
  active: still being tracked
  hitPMT: stopped by being detected in a PMT
  absorbed: stopped by being absorbed with G4OpAbsorbtion
  boundaryAbsorbed: stopped by being aborbed with G4OpAbsorbtion
  inactive: track is stopped for some reason
*/


class nDetUserTrackingInformation: public G4VUserTrackInformation {

public:
    nDetUserTrackingInformation();
    virtual ~nDetUserTrackingInformation();

    void IncReflections(){fReflections++;}
    void IncDetections(){fDetections++;}
    void IncAbsortions(){fAbsortions++;}
    G4int GetReflectionCount()const {return fReflections;}
    G4int GetDetectionCount()const {return fDetections;}
    G4int GetAbsortionCount()const {return fAbsortions;}

private:
    G4int fReflections;
    G4int fDetections;
    G4int fAbsortions;

};

#endif //NEXTMULTIPLESCATTERING_NDETUSERTRACKINGINFORMATION_HH

