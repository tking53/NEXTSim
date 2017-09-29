//
// Created by David Pérez Loureiro on 1/10/17.
//

#ifndef NEXTMULTIPLESCATTERING_SIPMSD_HH
#define NEXTMULTIPLESCATTERING_SIPMSD_HH

#include "G4VSensitiveDetector.hh"
#include "SiPMHits.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class sipmMC;
class PhotonList;

class SiPMSD: public G4VSensitiveDetector
{
    public:
        SiPMSD(G4String name);
        ~SiPMSD();

        void Initialize(G4HCofThisEvent *HCE);
        G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
        G4bool ProcessHits_constStep(const G4Step *aStep, G4TouchableHistory *ROhist);
        void EndOfEvent(G4HCofThisEvent *HCE);

    //TODO to be added....
        void clear();
        void DrawAll();
        void PrintAll();

    private:
        SiPMHitsCollection *hitsCollection;
    sipmMC *fsipm;
    PhotonList *fphotons;

};


#endif //NEXTMULTIPLESCATTERING_SIPMSD_HH
