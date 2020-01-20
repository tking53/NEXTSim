// $Id: ArgoneLN2Tank.hh 10.05.2017 A Fijalkowska $
//
/// \file ArgoneLN2Tank.hh
/// \brief Definition of the ArgoneLN2Tank class
//
#ifndef ArgoneLN2Tank_H
#define ArgoneLN2Tank_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

class ArgoneLN2Tank
{
  public:

    ArgoneLN2Tank(); 
    virtual ~ArgoneLN2Tank(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);

               
  private:
     void SetBasicSizes();
                        
    G4LogicalVolume* MakeTankShell();
    void AddLN2(G4LogicalVolume* tankShellLogVol);                                                                                       
    MaterialsManager* materialsManager;
    G4double zdet[4];
  	G4double innR[4]; 
  	G4double outR[4];

};



#endif
