// $Id: CERNFrame.cc 03.05.2017 A Fijalkowska $
//
/// \file CERNFrame.cc
/// \brief Implementation of the CERNFrame class
//
//

#include "CERNFrame.hh"

#include "G4Transform3D.hh"
#include "globals.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

CERNFrame::CERNFrame()
{
	SetBasicSizes();	
    materialsManager = MaterialsManager::GetInstance();
    MakeFrame();
} 

CERNFrame::~CERNFrame()
{
	
}

void CERNFrame::SetBasicSizes()
{
    frameThickness = 1*cm;
    frameWidth = 5*cm;
    frameLength = 122.5*cm;
	wingLength = 100.5*cm;
    wingAngle = 61.12*deg;
    wingXpos = -44*cm;
    wingYpos = 24.3*cm + frameLength/2.;
    legLength = 11.5*cm;
    legXPos = -94.25*cm;
    legYPos = 109.79*cm;
    sideBottomBarLength = 60*cm;
}


void CERNFrame::MakeFrame()
{


	G4Material* frameMaterial = materialsManager->GetStainSteel();
    G4Box* frameSolid1A = new G4Box("frameSolid1A", 
                                     frameWidth/2., 
                                     frameLength/2., 
                                     frameWidth/2.);
                                     
    G4Box* frameSolid1B = new G4Box("frameSolid1A", 
                                     frameWidth/2.- frameThickness, 
                                     frameLength/2., 
                                     frameWidth/2. - frameThickness);
    G4SubtractionSolid* frameSolid1 = new G4SubtractionSolid("frameSolid1", 
                                                              frameSolid1A, 
                                                              frameSolid1B);                                
    

    G4VSolid* frameSolid2 = AddWings(frameSolid1);
    //G4VSolid* frameSolid3 = AddLegs(frameSolid2); //legs
    //G4VSolid* frameSolid4 = AddSideBottomBars(frameSolid3);
    G4VSolid* frameSolid4 = AddSideBottomBars(frameSolid2);
    G4VSolid* frameSolid5 = AddFrontBottomBars(frameSolid4);
                                                                                                                                         
    frameLogVol = new G4LogicalVolume(frameSolid5, frameMaterial, "frameLogVol");
     
    G4VisAttributes* frameVisAtt = new G4VisAttributes( G4Colour(0.0,0.0,1));
	frameVisAtt->SetForceAuxEdgeVisible(true);// Can see outline when drawn with lines
	frameVisAtt->SetForceSolid(true);
	frameLogVol->SetVisAttributes(frameVisAtt);

	   	
}

G4VSolid* CERNFrame::AddWings(G4VSolid* frame)
{
    G4Box* wingSolid1 = new G4Box("wingSolid1", 
                                   frameWidth/2., 
                                   wingLength/2., 
                                   frameWidth/2.);
                                     
    G4Box* wingSolid2 = new G4Box("wingSolid2", 
                                     frameWidth/2.- frameThickness, 
                                     wingLength/2., 
                                     frameWidth/2. - frameThickness);
    G4SubtractionSolid* wingSolid = new G4SubtractionSolid("wingSolid", 
                                                             wingSolid1, 
                                                             wingSolid2); 
    G4RotationMatrix wing1Rot;
    wing1Rot.rotateZ(wingAngle);
    G4ThreeVector wing1Pos(wingXpos,wingYpos,0);
    G4Transform3D wing1Transf(wing1Rot,wing1Pos);
    G4UnionSolid* frameSolidWing1 = new G4UnionSolid("frameSolidWing1", 
                                                   frame,
                                                   wingSolid,
                                                   wing1Transf);
                                                   
    G4RotationMatrix wing2Rot;
    wing2Rot.rotateZ(-wingAngle);
    G4ThreeVector wing2Pos(wingXpos,-wingYpos,0);
    G4Transform3D wing2Transf(wing2Rot,wing2Pos);
    G4UnionSolid* frameSolidWing12 = new G4UnionSolid("frameSolidWing12", 
                                                   frameSolidWing1,
                                                   wingSolid,
                                                   wing2Transf);
     return frameSolidWing12;
}


G4VSolid* CERNFrame::AddLegs(G4VSolid* frame)
{
	G4Box* legSolid1 = new G4Box("legSolid1", 
                                  frameWidth/2., 
                                  legLength/2., 
                                  frameWidth/2.);
                                     
    G4Box* legSolid2 = new G4Box("legSolid2", 
                                  frameWidth/2.- frameThickness, 
                                  legLength/2., 
                                  frameWidth/2. - frameThickness);
                                  
    G4SubtractionSolid* legSolid = new G4SubtractionSolid("legSolid", 
                                                           legSolid1, 
                                                            legSolid2); 
	G4RotationMatrix legRot;
    legRot.rotateZ(90*deg);
    G4ThreeVector leg1Pos(legXPos,legYPos,0);
    G4Transform3D leg1Transf(legRot,leg1Pos);
    G4UnionSolid* frameSolidLeg1 = new G4UnionSolid("frameSolidLeg1", 
                                                   frame,
                                                   legSolid,
                                                   leg1Transf);

    G4ThreeVector leg2Pos(legXPos,-legYPos,0);
    G4Transform3D leg2Transf(legRot,leg2Pos);
    G4UnionSolid* frameSolidLeg12 = new G4UnionSolid("frameSolidLeg12", 
                                                   frameSolidLeg1,
                                                   legSolid,
                                                   leg2Transf);   
    return frameSolidLeg12;                                               
                                                 
}

G4VSolid* CERNFrame::AddSideBottomBars(G4VSolid* frame)
{
	G4Box* sideBottomBarSolid1 = new G4Box("sideBottomBarSolid1", 
                                            frameWidth/2., 
                                            sideBottomBarLength/2., 
                                            frameWidth/2.);
                                     
    G4Box* sideBottomBarSolid2 = new G4Box("sideBottomBarSolid2", 
                                            frameWidth/2.- frameThickness, 
                                            sideBottomBarLength/2., 
                                            frameWidth/2. - frameThickness);
                                  
    G4SubtractionSolid* sideBottomBarSolid = new G4SubtractionSolid("sideBottomBarSolid", 
                                                                     sideBottomBarSolid1, 
                                                                     sideBottomBarSolid2); 
                                                                     
    G4RotationMatrix barRot;
    barRot.rotateX(90*deg);
    G4ThreeVector bar1Pos(wingXpos*2,legYPos-frameWidth,0);
    G4Transform3D bar1Transf(barRot,bar1Pos);
    G4UnionSolid* frameBottomBarSolid1 = new G4UnionSolid("frameBottomBarSolid1", 
                                                           frame,
                                                           sideBottomBarSolid,
                                                           bar1Transf);

    G4ThreeVector bar2Pos(wingXpos*2,-legYPos+frameWidth,0);
    G4Transform3D bar2Transf(barRot,bar2Pos);
    G4UnionSolid* frameBottomBarSolid12 = new G4UnionSolid("frameBottomBarSolid12", 
                                                            frameBottomBarSolid1,
                                                            sideBottomBarSolid,
                                                            bar2Transf);   
    return frameBottomBarSolid12;
	
}

G4VSolid* CERNFrame::AddFrontBottomBars(G4VSolid* frame)
{
	G4Box* frontBottomBarSolid1 = new G4Box("frontBottomBarSolid1", 
                                            frameWidth/2., 
                                            legYPos, 
                                            frameWidth/2.);
                                     
    G4Box* frontBottomBarSolid2 = new G4Box("sideBottomBarSolid2", 
                                            frameWidth/2.- frameThickness, 
                                            legYPos, 
                                            frameWidth/2. - frameThickness);
                                  
    G4SubtractionSolid* frontBottomBarSolid = new G4SubtractionSolid("frontBottomBarSolid", 
                                                                      frontBottomBarSolid1, 
                                                                      frontBottomBarSolid2); 
                                                                     
    G4ThreeVector barPos(wingXpos*2,0,sideBottomBarLength/2.);
    G4UnionSolid* frameBottomFrontBarSolid = new G4UnionSolid("frameBottomFrontBarSolid", 
                                                           frame,
                                                           frontBottomBarSolid,
                                                           0,
                                                           barPos);

  
    return frameBottomFrontBarSolid;
	
}

void CERNFrame::Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo)
{

    new G4PVPlacement(pRot,
                      tlate,
		             frameLogVol,
		             pName, 
	                 pMotherLogical,
	                 0,
	                 pCopyNo);
}

