// $Id: Floor.hh 29.01.2016 A Fijalkowska $
//
/// \file Floor.hh
/// \brief Definition of the Floor class
//
#ifndef Floor_H
#define Floor_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class Floor
{
  public:

    Floor(); 
    virtual ~Floor(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();                                                                                     
     MaterialsManager* materialsManager;
     //G4VisAttributes* supportVisAtt;
     G4double inch; 
};



#endif
