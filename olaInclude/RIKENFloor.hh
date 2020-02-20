// $Id: RIKENFloor.hh 03.05.2017 A Fijalkowska $
//
/// \file RIKENFloor.hh
/// \brief Definition of the RIKENFloor class
//
#ifndef RIKENFloor_H
#define RIKENFloor_H 1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class RIKENFloor
{
  public:

    RIKENFloor(); 
    virtual ~RIKENFloor(); 
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

