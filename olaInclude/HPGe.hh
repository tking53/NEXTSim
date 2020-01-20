// $Id: HPGe.hh 17.01.2016 A Fijalkowska $
//
/// \file HPGe.hh
/// \brief Definition of the HPGe class
//
#ifndef HPGe_H
#define HPGe_H 1

#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4AssemblyVolume.hh"
#include "G4UnionSolid.hh"
class HPGe
{
  public:

    HPGe();    
    void Place(G4RotationMatrix *pRot, 
               const G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
    
    void Place(G4Transform3D &pTransform, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
                          
    G4AssemblyVolume* GetHPGeAssembly() {return HPGeAssembly;}
    virtual void ConstructSDandField();
  private:
     void SetSizes();
     void SetBasicSizes();
     void SetFacePartSizes();
     void SetBox2Sizes();
     void SetBox3Sizes();
     void SetCylinder4Sizes();
     void SetCylinder5Sizes();
     void SetCylinder6Sizes();
     void SetCylinder7Sizes();
     void SetColors();
     G4LogicalVolume* makeSingleCrystal();
     void placeSingleCrystal(G4LogicalVolume* crystalLogVol,
                             G4AssemblyVolume* assemblyHPGe, 
                             G4double rotAngle, 
                             G4ThreeVector &position);
     G4AssemblyVolume* makeAllCrystals();
     void makeHPGeFace(G4AssemblyVolume* assemblyHPGe); 
     void placeCrystals(G4AssemblyVolume* assemblyHPGe);
     void makeHPGeBox2(G4AssemblyVolume* assemblyHPGe);
     void makeHPGeBox3(G4AssemblyVolume* assemblyHPGe);
     void makeHPGeCylinder4(G4AssemblyVolume* assemblyHPGe);
     void makeHPGeCylinder5(G4AssemblyVolume* assemblyHPGe);
     void makeHPGeCylinder6(G4AssemblyVolume* assemblyHPGe);
     void makeHPGeCylinder7(G4AssemblyVolume* assemblyHPGe);
     G4AssemblyVolume* makeHPGe();
  
     G4UnionSolid* roundedBox(G4double length,  
                              G4double thickness,
                              G4double radius, 
                              G4double zz);
     G4UnionSolid* roundedFrontFace(G4double thickness, 
                                    G4double radius, 
                                    G4double zz);    
     G4UnionSolid* roundedFrontFaceHole(G4double thickness, 
                                        G4double radius, 
                                        G4double zz, 
                                        G4double radiusHole); 
                                       
                                                            
	 G4LogicalVolume* HPGeLogic; 
	 G4LogicalVolume* HPGeCrystalLogic;
	 G4LogicalVolume* HPGeFacePartLogic;
	 G4AssemblyVolume* HPGeAssembly;                   
     MaterialsManager* materialsManager;
     
     G4VisAttributes* HPGeVisAtt;
     G4VisAttributes* HPGeFVisAtt;
     

     G4double totalOffsetX;
     G4double totalOffsetY;
     G4double totalOffsetZ; 
	
     G4double crystalInnerRad;
  	 G4double crystalOuterRad;
  	 G4double crystalLength; 	
  	 G4double crystalTubeCutX;
	 G4double crystalTubeCutY;
	 G4double crystalTubeCutZ;	
	 G4double crystalDistance;
	 G4double crystalCenterDistance;
	 

	 G4double box1_x;        
  	 G4double box1_z;
  	 G4double box1_rad;
  	 G4double box1_thickness;
	 G4double faceZOffset;
	 
	 
	 G4double box2_x;        
  	 G4double box2_z;
  	 G4double box2_rad;
  	 G4double box2_thickness;
  	 G4double faceb_rad;
  	 
  	 G4double box3_x;        
  	 G4double box3_z;
  	 G4double box3_rad;
  	 G4double box3_thickness; 
  	 
  	 
     G4double cyl4_inn_rad;
  	 G4double cyl4_out_rad;
  	 G4double cyl4_length;
  	 
     G4double cyl5_inn_rad;
     G4double cyl5_out_rad;
     G4double cyl5_length;
     
     
     G4double cyl6_inn_rad;
  	 G4double cyl6_out_rad;
  	 G4double cyl6_length;
  	 
     G4double cyl7_inn_rad;
  	 G4double cyl7_out_rad;
  	 G4double cyl7_length;


};

#endif							 
