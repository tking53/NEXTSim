// $Id: ArgoneFrame.hh 26.01.2016 A Fijalkowska $
//
/// \file ArgoneFrame.hh
/// \brief Definition of the ArgoneFrame class
//
#ifndef ArgoneFrame_H
#define ArgoneFrame_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

class ArgoneFrame
{
  public:

    ArgoneFrame(); 
    virtual ~ArgoneFrame(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
    G4VSolid* GetRibsSolid();
    G4double GetRibsX() {return boxB_x;}
    G4double GetRibsY() {return boxB_y;}
    G4double GetRibsZ() {return boxB_z;}
               
  private:
     void SetBasicSizes();
     void PlaceSkeleton(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo = 0);
                        
     G4VSolid* MakeRibsSolid(); 
     G4VSolid* AddHolders(G4VSolid*ribsSolid);
     G4VisAttributes* argSupportVisAtt;                                                                                        
     MaterialsManager* materialsManager;
     G4Material* frameMaterial;
     
     G4double boxB_x;    
     G4double boxB_y;    
     G4double boxB_z;

};



#endif
