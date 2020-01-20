
/***********************************************************************
 *
 *    Sensitive Detector for detector read-out of CloverSingle detector
 *
 ************************************************************************/

#ifndef CloverSingleSD_h
#define CloverSingleSD_h 1

#include "G4VSensitiveDetector.hh"
#include "CloverSingleHit.hh"

class G4Step;
class G4HCofThisEvent;
//class IDS_DetClass17HistoManager;
class G4VPhysicalVolume;
class G4AffineTransform;

#include <vector>

/*
class Detectorization{
  public:
      Detectorization(G4String);
      ~Detectorization();

      G4String DetectorName;
      G4int DetectorHitsCollectionID_LaBr3;
};
*/


class CloverSingleSD : public G4VSensitiveDetector
{
  public:
      CloverSingleSD(G4String name);
     ~CloverSingleSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

      inline G4int GetCloverSingleDetectorID() {return clov_ID;};
      inline G4int SetCloverSingleDetectorID(G4int det_id) {clov_ID=det_id;};
      inline G4int GetCloverSingleCrystalID() {return crys_ID;};
      inline G4int SetCloverSingleCrystalID(G4int det_id) {crys_ID=det_id;};
      //G4int AddLaBr3Detector(G4String name);
      //G4int AddLaBr3Detector();

      //G4int GetNbOfLaBr3Detectors();


  private:
      G4String HC_name;
      CloverSingleHitsCollection* hitsCollection; 

      G4int clov_ID;
      G4int crys_ID;
//      G4int tracecounter;
      G4int HCID;


//  protected:
//      vector<Detectorization*> detectors;
};

#endif

