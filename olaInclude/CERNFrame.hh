// $Id: CERNFrame.hh 03.05.2017 A Fijalkowska $
//
/// \file CERNFrame.hh
/// \brief Definition of the CERNFrame class
//
#ifndef CERNFrame_H
#define CERNFrame_H1
#include "G4RotationMatrix.hh"

#include "G4VSolid.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class CERNFrame
{
  public:

    CERNFrame(); 
    virtual ~CERNFrame(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();     
     void MakeFrame();                                                        
     G4VSolid* AddLegs(G4VSolid* frame);  
     G4VSolid* AddWings(G4VSolid* frame);
     G4VSolid* AddSideBottomBars(G4VSolid* frame);  
     G4VSolid* AddFrontBottomBars(G4VSolid* frame);  
                                                                                   
     MaterialsManager* materialsManager;
     G4LogicalVolume* frameLogVol;

     G4double frameThickness;
     G4double frameWidth;
     G4double frameLength;
     G4double wingLength;
     G4double wingAngle;
     G4double wingXpos;
     G4double wingYpos;
     G4double legLength;
     G4double legXPos;
     G4double legYPos;
     G4double sideBottomBarLength;
};



#endif

