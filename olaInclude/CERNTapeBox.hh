// $Id: CERNTapeBox.hh 04.05.2017 A Fijalkowska $
//
/// \file CERNTapeBox.hh
/// \brief Definition of the CERNTapeBox class
//
#ifndef CERNTapeBox_H
#define CERNTapeBox_H1
#include "G4RotationMatrix.hh"

#include "G4VSolid.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class CERNTapeBox
{
  public:

    CERNTapeBox(); 
    virtual ~CERNTapeBox(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();     
     void MakeTapeBox();                                                        
     G4VSolid* AddCanal(G4VSolid* box);  
     G4VSolid* AddRings(G4VSolid* box);
     //G4VSolid* AddSideBottomBars(G4VSolid* frame);  
     //G4VSolid* AddFrontBottomBars(G4VSolid* frame);  
                                                                                   
     MaterialsManager* materialsManager;
     G4LogicalVolume* tapeBoxLogVol;

     G4double tapeBoxXSize;
     G4double tapeBoxYSize;
     G4double tapeBoxZSize;
     G4double tapeBoxThickness;
     G4double tapeBoxXPos;
     
     G4double canalXSize;
     G4double canalYSize;
     G4double canalZSize;
     G4double canalThickness;
     
     G4double ring1Radius;
     G4double ring1Height;
     G4double ring2Radius;
     G4double ring2Height;
     G4double ring3Radius;
     G4double ring3Height;
     G4double ringThickness;
};



#endif

