// $Id: LeribssSupport.hh 26.01.2016 A Fijalkowska $
//
/// \file LeribssSupport.hh
/// \brief Definition of the LeribssSupport class
//
#ifndef LeribssSupport_H
#define LeribssSupport_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Material.hh"
class LeribssSupport
{
  public:

    LeribssSupport(); 
    virtual ~LeribssSupport(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
     G4UnionSolid* roundedHalfPlate(G4double height, 
                                    G4double width, 
                                    G4double thickness, 
                                    G4double radius);
     
     void MakeDetectorPlate(G4RotationMatrix *pRot, 
                               G4ThreeVector &tlate, 
                               const G4String &pName, 
                               G4LogicalVolume *pMotherLogical,  
                               G4int pCopyNo);
                               
     G4SubtractionSolid* MakeBracket();
     G4SubtractionSolid* MakeCeilingSupportBox(); 
     G4SubtractionSolid* MakeCeilSupport();
     G4UnionSolid* MatekTotalCeilingSupport(G4SubtractionSolid* ceilSupportBox, 
                                            G4SubtractionSolid* ceilSupport);	
                                                       
     G4SubtractionSolid* MakeCeilingSupportBracket();
     G4UnionSolid* MakeHPGeHolderExtra();
     G4UnionSolid* MakeHPGeBoxRing(G4UnionSolid* HPGeholderBoxExtra2);
     G4UnionSolid* MakeHPGeHolderPlate(G4UnionSolid* HPGeholderNoHolder);
     void MakeHPGeHolder(G4RotationMatrix *pRot, 
                         G4ThreeVector &tlate, 
                         const G4String &pName, 
                         G4LogicalVolume *pMotherLogical,  
                         G4int pCopyNo);
                               
     void PlaceTubes(G4RotationMatrix *pRot, 
                     G4ThreeVector &tlate, 
                     const G4String &pName, 
                     G4LogicalVolume *pMotherLogical,  
                     G4int pCopyNo = 0);
      
     void MakeSupportRing(G4RotationMatrix *pRot, 
                          G4ThreeVector &tlate, 
                          const G4String &pName, 
                          G4LogicalVolume *pMotherLogical,  
                          G4int pCopyNo);
     void MakeRailSupport(G4RotationMatrix *pRot, 
                          G4ThreeVector &tlate, 
                          const G4String &pName, 
                          G4LogicalVolume *pMotherLogical,  
                          G4int pCopyNo);                                
                                                                                              
     MaterialsManager* materialsManager;
     G4VisAttributes* supportVisAtt;
     G4Material* aluminiumMaterial;
     
     G4double inch; 
     G4double boxDist;
     G4double tubeLength;
     
          //boxM        	
     G4double half_length_xm;    
     G4double half_length_ym;    
     G4double half_thickness_zm;      
     G4double upCut;   
     G4double bCut;  

     //boxUpCut     
     G4double boxUpCutX;    
     G4double boxUpCutY;    
     G4double boxUpCutZ;
     G4double boxBCutAngle;

     //boxBCut
     G4double boxBCutX;      
     G4double boxBCutY; 
     G4double boxBCutZ;

     G4double boxOffset;
    
    
     
     G4double bracketBase_x;
	 G4double bracketBase_y;
	 G4double bracketBase_z;
	 
	 G4double detPlate_x;
	 G4double detPlate_y;
	 G4double detPlate_z;
	 
	 G4double boxOut_x;
	 G4double boxOut_y;
	 G4double boxOut_z;
	 
	 
	 G4double ceilSuppBase_x;
	 G4double ceilSuppBase_y;
	 G4double ceilSuppBase_z;
	 
	 
	 G4double ceilBracketBase_x;
	 G4double ceilBracketBase_y;
	 G4double ceilBracketBase_z;
	 
	 G4double HPGeholderBox_x;
	 G4double HPGeholderBox_y;
	 G4double HPGeholderBox_z;
	 
	 
	 G4double HPGeholderBoxExtra_x;
	 G4double HPGeholderBoxExtra_y;
	 G4double HPGeholderBoxExtra_z;
};



#endif
