// $Id: CERNTapeBox.cc 03.05.2017 A Fijalkowska $
//
/// \file CERNTapeBox.cc
/// \brief Implementation of the CERNTapeBox class
//
//

#include "CERNTapeBox.hh"

#include "G4Transform3D.hh"
#include "globals.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

CERNTapeBox::CERNTapeBox()
{
	SetBasicSizes();	
    materialsManager = MaterialsManager::GetInstance();
    MakeTapeBox();
} 

CERNTapeBox::~CERNTapeBox()
{
	
}

void CERNTapeBox::SetBasicSizes()
{
     tapeBoxXSize = 41.3*cm;
     tapeBoxYSize = 31.6*cm;
     tapeBoxZSize = 61.2*cm;
     tapeBoxThickness = 1*cm;
     tapeBoxXPos = 86.55*cm;
     
     canalXSize = 45*cm;
     canalYSize = 3*cm;
     canalZSize = 6*cm;
     canalThickness = 0.5*cm;
     
     ring1Radius = 5*cm;
     ring1Height = 3.6*cm;
     ring2Radius = 3*cm;
     ring2Height = 4.9*cm;
     ring3Radius = 4*cm;
     ring3Height = 1*cm;
     ringThickness = canalThickness;
}


void CERNTapeBox::MakeTapeBox()
{


	G4Material* tapeBoxMaterial = materialsManager->GetAluminum();
    G4Box* tapeBox1 = new G4Box("tapeBox1", 
                                 tapeBoxXSize/2., 
                                 tapeBoxYSize/2., 
                                 tapeBoxZSize/2.);
                                     
    G4Box* tapeBox2 = new G4Box("tapeBox2", 
                                 tapeBoxXSize/2. - tapeBoxThickness, 
                                 tapeBoxYSize/2. - tapeBoxThickness, 
                                 tapeBoxZSize/2. - tapeBoxThickness);
    G4SubtractionSolid* tapeBox12 = new G4SubtractionSolid("tapeBox12", 
                                                          tapeBox1, 
                                                          tapeBox2);                                
    

    G4VSolid* boxCanal = AddCanal(tapeBox12);
    G4VSolid* tapeBox = AddRings(boxCanal);
    //G4VSolid* frameSolid4 = AddSideBottomBars(frameSolid3);
    //G4VSolid* frameSolid5 = AddFrontBottomBars(frameSolid4);
                                                                                                                                         
    tapeBoxLogVol = new G4LogicalVolume(tapeBox, tapeBoxMaterial, "tapeBoxLogVol");
     
    G4VisAttributes* tapeBoxVisAtt = new G4VisAttributes( G4Colour(0.3,0.3,0.3));
	tapeBoxVisAtt->SetForceAuxEdgeVisible(true);// Can see outline when drawn with lines
	tapeBoxVisAtt->SetForceSolid(true);
	tapeBoxLogVol->SetVisAttributes(tapeBoxVisAtt);

	   	
}


G4VSolid* CERNTapeBox::AddCanal(G4VSolid* box)
{
	G4Box* canalSolid1 = new G4Box("canalSolid1", 
                                    canalXSize/2., 
                                    canalYSize, 
                                    canalZSize/2.);
                                     
    G4Box* canalSolid2 = new G4Box("canalSolid2", 
                                    canalXSize/2., 
                                    canalYSize/2.- canalThickness, 
                                    canalZSize/2. - canalThickness);
                                  
    G4SubtractionSolid* canalSolid = new G4SubtractionSolid("canalSolid", 
                                                             canalSolid1, 
                                                             canalSolid2); 
                                                                     
    G4ThreeVector canalPos((tapeBoxXSize+canalXSize)/2., 0, 0);
    G4UnionSolid* tapeBoxAndCanal = new G4UnionSolid("tapeBoxAndCanal", 
                                                      box,
                                                      canalSolid,
                                                      0,
                                                      canalPos);
    return tapeBoxAndCanal;
	
}

G4VSolid* CERNTapeBox::AddRings(G4VSolid* box)
{
	G4Tubs* ring1Solid = new G4Tubs("ring1Solid", 
                                     ring1Radius - ringThickness, 
                                     ring1Radius, 
                                     ring1Height/2.,
                                     0,
                                     360*deg);
    
    G4RotationMatrix rotRing;
    rotRing.rotateY(90.*deg);
    G4double ring1XPos = (tapeBoxXSize + ring1Height)/2 +canalXSize;
    G4ThreeVector ring1Pos(ring1XPos,0.0, 0.);
    G4Transform3D ring1Transf(rotRing,ring1Pos);
    G4UnionSolid* tapeBoxRing1 = new G4UnionSolid("tapeBoxRing1", 
                                                   box, 
                                                   ring1Solid, 
                                                   ring1Transf); 
       
       
                                                                        
 	G4Tubs* ring2Solid = new G4Tubs("ring2Solid", 
                                    ring2Radius - ringThickness, 
                                    ring2Radius, 
                                    ring2Height/2.,
                                    0,
                                    360*deg);                                    
    G4double ring2XPos = ring1XPos + (ring1Height+ring2Height)/2.;
    G4ThreeVector ring2Pos(ring2XPos,0.0, 0.);
    G4Transform3D ring2Transf(rotRing,ring2Pos);                              
    G4UnionSolid* tapeBoxRing2 = new G4UnionSolid("tapeBoxRing2", 
                                                   tapeBoxRing1, 
                                                   ring2Solid, 
                                                   ring2Transf); 

	
 	G4Tubs* ring3Solid = new G4Tubs("ring3Solid", 
                                    ring3Radius - ringThickness, 
                                    ring3Radius, 
                                    ring3Height/2.,
                                    0,
                                    360*deg);                                    
    G4double ring3XPos = ring2XPos + (ring2Height+ring3Height)/2.;
    G4ThreeVector ring3Pos(ring3XPos,0.0, 0.);
    G4Transform3D ring3Transf(rotRing,ring3Pos);                              
    G4UnionSolid* tapeBoxRing3 = new G4UnionSolid("tapeBoxRin3", 
                                                   tapeBoxRing2, 
                                                   ring3Solid, 
                                                   ring3Transf); 

	
	

     
     return tapeBoxRing3;
}

void CERNTapeBox::Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo)
{

    new G4PVPlacement(pRot,
                      tlate,
		             tapeBoxLogVol,
		             pName, 
	                 pMotherLogical,
	                 0,
	                 pCopyNo);
}




