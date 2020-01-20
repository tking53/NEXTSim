// $Id: ArgoneFrame.hh 10.05.2017 A Fijalkowska $
//
/// \file ArgoneFrame.hh
/// \brief Definition of the ArgoneFrame class
//
#ifndef ArgoneHPGeHolder_H
#define ArgoneHPGeHolder_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

class ArgoneHPGeHolder
{
  public:

    ArgoneHPGeHolder(); 
    virtual ~ArgoneHPGeHolder(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
                      
     G4VSolid* MakeMainHPGeHolderSolid(); 
     G4VSolid* AddSupplementHPGeHolderSolid(G4VSolid* mainHolder);  
     G4VSolid* AddHPGeHolderRing(G4VSolid* suppHPGeHolderSolid);   
     G4VSolid* AddHPGeHolderPlate(G4VSolid* HPGeHolderRingSolid);                                                                          
     MaterialsManager* materialsManager;
     G4Material* frameMaterial;
     
     G4double HPGeholderBox_x;
	 G4double HPGeholderBox_y;
	 G4double HPGeholderBox_z;
     G4double inch;
};



#endif
