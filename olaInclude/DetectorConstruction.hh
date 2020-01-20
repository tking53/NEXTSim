// $Id: DetectorConstruction.hh 18.10.2016 A. Fijalkowska $
//
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//
#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;

#include "VANDLEBar.hh"
#include "CERNVandleArray.hh"
#include "G4VUserDetectorConstruction.hh"
#include "MaterialsManager.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

  private:
    void ConstructCERNElements();
    G4VPhysicalVolume* ConstructDetector();
    G4LogicalVolume* experimentalHallLogic;
    G4VPhysicalVolume* experimentalHallPhys;
    MaterialsManager* materialsManager;
	VANDLEBar* vandleBar;

    CERNVandleArray* cernVandleArray;
    //ArgoneVandleArray* argoneVandleArray;
    
};

#endif
