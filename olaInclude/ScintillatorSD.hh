//
// $Id: ScintillatorSD.hh 12.08.2016 A. Fijalkowska $
//
/// \file ScintillatorSD.hh
/// \brief Definition of the Scintillator sensitive detector class
//
//
#ifndef ScintillatorSD_h
#define ScintillatorSD_h 1

#include "G4DataVector.hh"
#include "G4VSensitiveDetector.hh"
#include "G4LogicalVolume.hh"
#include "ScintillatorHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

class ScintillatorSD : public G4VSensitiveDetector
{

  public:

    ScintillatorSD(G4String name);
    virtual ~ScintillatorSD();
 
    virtual void Initialize(G4HCofThisEvent* );
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );
    virtual void EndOfEvent(G4HCofThisEvent* );
    virtual void clear();
    void SetModuleDeph(G4int moduleDephVal) {moduleDeph = moduleDephVal;}
    
  private:

    G4int GetIndex(const G4Step* aStep, int deph);
    G4double GetEnergyDeposit(const G4Step* aStep);
    G4double GetHitTime(const G4Step* aStep);
    G4ThreeVector GetPosition(const G4Step* aStep);
    ScintillatorHitsCollection* scintillatorHitCollection;
    G4int moduleDeph;

};

#endif
