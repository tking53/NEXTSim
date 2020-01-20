// $Id: CERNFloor.hh 03.05.2017 A Fijalkowska $
//
/// \file CERNFloor.hh
/// \brief Definition of the CERNFloor class
//
#ifndef CERNFloor_H
#define CERNFloor_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class CERNFloor
{
  public:

    CERNFloor(); 
    virtual ~CERNFloor(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();     
     void MakeFloor();                                                        
                                                                                     
     MaterialsManager* materialsManager;
     G4LogicalVolume* floorLogVol;
     G4double floorThickness;
     G4double floorWidth;
     G4double floorDistance;
};



#endif

