
// $Id: ArgoneFrame.cc 07.05.2017 A Fijalkowska $
//
/// \file ArgoneFrame.cc
/// \brief Implementation of the ArgoneFrame class
//
//

#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "ArgoneFrame.hh"

ArgoneFrame::ArgoneFrame()
{
    materialsManager = MaterialsManager::GetInstance();
    SetBasicSizes();
    argSupportVisAtt = new G4VisAttributes( G4Colour(248./255.,248./255.,255./255.)); 
	argSupportVisAtt->SetForceAuxEdgeVisible(true);// Can see outline when drawn with lines
	argSupportVisAtt->SetForceSolid(true);
} 

ArgoneFrame::~ArgoneFrame()
{

}

void ArgoneFrame::SetBasicSizes()
{
    boxB_x = 183.2356/2.0*cm;    
    boxB_y = 112.9284/2.0*cm;    
    boxB_z = 2.54/4.0*cm;
	
}

G4VSolid* ArgoneFrame::GetRibsSolid()
{	
	return MakeRibsSolid();
}

void ArgoneFrame::Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo)
{

    frameMaterial = materialsManager->GetAluminum();
	G4VSolid* ribsSolid =  MakeRibsSolid();
    G4VSolid* ribsTotSolid = AddHolders(ribsSolid); 

    /*G4LogicalVolume* ribLogical = new G4LogicalVolume(ribsSolid, 
                                                      frameMaterial, 
                                                      "ribsTotSolid");*/
                                                      
                                                      
                                                      
    G4LogicalVolume* ribLogical = new G4LogicalVolume(ribsTotSolid, 
                                                      frameMaterial, 
                                                      "ribsTotSolid"); 
	ribLogical->SetVisAttributes(argSupportVisAtt); 
	//left
	G4double distRibs = 45.593/2.0*cm;
	const G4ThreeVector posBoxBL(0.0,boxB_y,boxB_z+distRibs);
	G4ThreeVector posLeft = tlate + posBoxBL;
	
	G4String frameLeftName = pName+"_frameLeftPhys";
	
	new G4PVPlacement( pRot, 
	                   posLeft, 
	                   ribLogical, 
	                   frameLeftName, 
	                   pMotherLogical,
	                   0, 
	                   pCopyNo ); 
	//right
	G4RotationMatrix rotBoxBR;
	rotBoxBR.rotateY(180.0*deg);
	const G4ThreeVector posBoxBR(0.0,boxB_y,-(boxB_z+distRibs));
	G4ThreeVector posRight = tlate + posBoxBR;
	//todo add rotation!!!!!!! 07.05.2017
	G4Transform3D transformBoxBR(rotBoxBR,posRight);
	G4String frameRightName = pName+"_frameRightPhys";
	new G4PVPlacement( transformBoxBR, 
	                   ribLogical, 
	                   frameRightName, 
	                   pMotherLogical, 
	                   0, 
	                   pCopyNo); 
	
	
	
	
	PlaceSkeleton(pRot, tlate, pName, pMotherLogical, pCopyNo);


}



G4VSolid* ArgoneFrame::MakeRibsSolid()
{
    //main solid

	G4Box* boxB = new G4Box("boxB", boxB_x, boxB_y, boxB_z);

	
	//create round cut
	G4double radCut = 102.362*cm;
    G4double lCut = 2.0*boxB_z;    	   
	G4Tubs* cutSol = new G4Tubs("cutSol", 0.0, radCut, lCut, 0.0*M_PI, 2.0*M_PI);
    const G4ThreeVector posRCut(0.0,-boxB_y,0.0);
    G4SubtractionSolid* ribRCutSol = new G4SubtractionSolid("ribRCutSol", 
                                                            boxB, 
                                                            cutSol, 
                                                            0, 
                                                            posRCut);
     
    //create square cut
    G4double shiftCut = 56.472074*cm;
    G4double boxCut_x = 2.0*boxB_x;    
    G4double boxCut_y = boxB_y;    
    G4double boxCut_z = 2.0*boxB_z;
	G4Box* boxCutSol = new G4Box("boxCutSol", boxCut_x, boxCut_y, boxCut_z);
    const G4ThreeVector posBCut(0.0,-shiftCut,0.0);
    G4SubtractionSolid* ribSolRB = new G4SubtractionSolid("ribSolRB", 
                                                           ribRCutSol, 
                                                           boxCutSol, 
                                                           0, 
                                                           posBCut);

     //make 1st cir 5.70992
	 G4double radC1 = 5.70992*cm;
     G4double lC1 = 2.0*boxB_z;
     G4double xC1 = 83.533742*cm;    
     G4double yC1 = 18.217134*cm;   	
	 G4Tubs* cutC1 = new G4Tubs("cutC1", 0.0, radC1, lC1, 0.0*M_PI, 2.0*M_PI);	
     //left
	 const G4ThreeVector posC1L(xC1,yC1,0.0);
     G4SubtractionSolid* ribSolC1L = new G4SubtractionSolid("ribSolC1L", 
                                                             ribSolRB, 
                                                             cutC1, 
                                                             0, 
                                                             posC1L);
     //right
     const G4ThreeVector posC1R(-xC1,yC1,0.0);
     G4SubtractionSolid* ribSolRBC1 = new G4SubtractionSolid("ribSolRBC1", 
                                                              ribSolC1L, 
                                                              cutC1, 
                                                              0, 
                                                              posC1R);

     //make 2nd cir 15.101824
	 G4double radC2 = 15.101824*cm;
     G4double lC2 = 2.0*boxB_z;
     G4double xC2 = 74.141838*cm;    
     G4double yC2 = 39.597076*cm;   	    
	 G4Tubs* cutC2 = new G4Tubs("cutC2", 0.0, radC2, lC2, 0.0*M_PI, 2.0*M_PI);	
	 //left
	 const G4ThreeVector posC2L(xC2,yC2,0.0);
     G4SubtractionSolid* ribSolC2L = new G4SubtractionSolid("ribSolC2L", 
                                                             ribSolRBC1, 
                                                             cutC2, 
                                                             0, 
                                                             posC2L);
     //right
     const G4ThreeVector posC2R(-xC2,yC2,0.0);
     G4SubtractionSolid* ribSolRBC1C2 = new G4SubtractionSolid("ribSolRBC1C2", 
                                                                ribSolC2L, 
                                                                cutC2, 
                                                                0, 
                                                                posC2R);

     //make 3rd cir 8.107934
	G4double radC3 = 8.107934*cm;
	G4double lC3 = 2.0*boxB_z;
	G4double xC3 = 49.360074*cm;    
	G4double yC3 = 46.590966*cm;   	 
    G4Tubs* cutC3 = new G4Tubs("cutC3", 0.0, radC3, lC3, 0.0*M_PI, 2.0*M_PI);	
	//left
	const G4ThreeVector posC3L(xC3,yC3,0.0);
	G4SubtractionSolid* ribSolC3L = new G4SubtractionSolid("ribSolC3L", 
	                                                        ribSolRBC1C2, 
	                                                        cutC3, 
	                                                        0, 
	                                                        posC3L);
	//right
	const G4ThreeVector posC3R(-xC3,yC3,0.0);
	G4SubtractionSolid* ribSolRBC1C2C3 = new G4SubtractionSolid("ribSolRBC1C2C3", 
	                                                             ribSolC3L, 
	                                                             cutC3, 
	                                                             0, 
	                                                             posC3R);

	//make 4th cir 3.700272
	G4double radC4 = 3.700272*cm;
	G4double lC4 = 2.0*boxB_z;
	G4double xC4 = 22.531578*cm;    
	G4double yC4 = 50.998628*cm;   	
	///////////////////////////////////////////////////////    
	G4Tubs* cutC4 = new G4Tubs("cutC4", 0.0, radC4, lC4, 0.0*M_PI, 2.0*M_PI);	
	//left
	const G4ThreeVector posC4L(xC4,yC4,0.0);
	G4SubtractionSolid* ribSolC4L = new G4SubtractionSolid("ribSolC4L", 
	                                                        ribSolRBC1C2C3, 
	                                                        cutC4, 
	                                                        0, 
	                                                        posC4L);
	//right
	const G4ThreeVector posC4R(-xC4,yC4,0.0);
	G4SubtractionSolid* ribSolC4R = new G4SubtractionSolid("ribSolC4R", 
	                                                        ribSolC4L, 
	                                                        cutC4, 
	                                                        0, 
	                                                        posC4R);
     
    return  ribSolC4R;

	
}


G4VSolid* ArgoneFrame::AddHolders(G4VSolid*ribsSolid)
{
	//horizontal
    G4double boxRibH_x = 18.5674/2.0*cm;    
    G4double boxRibH_y = 2.54/2.0*cm;    
    G4double boxRibH_z = boxRibH_y;
	G4Box* boxRibH = new G4Box("boxRibH", boxRibH_x, boxRibH_y, boxRibH_z);
	G4double ribHxOff = 86.7664*cm;
	G4double ribHyOff = 3.5814*cm;
	const G4ThreeVector posBoxRibHbr(ribHxOff+boxRibH_x,ribHyOff+boxRibH_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribHbr = new G4UnionSolid("ribHbr", ribsSolid, boxRibH, 0, posBoxRibHbr);
	const G4ThreeVector posBoxRibHbl(-(ribHxOff+boxRibH_x),ribHyOff+boxRibH_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribHbl = new G4UnionSolid("ribHbl", ribHbr, boxRibH, 0, posBoxRibHbl);
	ribHyOff = 51.7652*cm;
	const G4ThreeVector posBoxRibHtr(ribHxOff+boxRibH_x,ribHyOff+boxRibH_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribHtr = new G4UnionSolid("ribHtr", ribHbl, boxRibH, 0, posBoxRibHtr);
	const G4ThreeVector posBoxRibHtl(-(ribHxOff+boxRibH_x),ribHyOff+boxRibH_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribHtl = new G4UnionSolid("ribHtl", ribHtr, boxRibH, 0, posBoxRibHtl);
	
	//verticle
	G4double boxRibV_x = boxRibH_y;    
    G4double boxRibV_y = 17.3736/2.0*cm;    
    G4double boxRibV_z = boxRibH_y;
	G4Box* boxRibV = new G4Box("boxRibV", boxRibV_x, boxRibV_y, boxRibV_z);
	G4double ribVxOff = 59.9186*cm;
	G4double ribVyOff = 70.7263*cm;//69.1134*cm;
	const G4ThreeVector posBoxRibV1(ribVxOff+boxRibV_x,ribVyOff-boxRibV_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribV1 = new G4UnionSolid("ribV1", ribHtl, boxRibV, 0, posBoxRibV1);
	const G4ThreeVector posBoxRibV4(-(ribVxOff+boxRibV_x),ribVyOff-boxRibV_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribV4 = new G4UnionSolid("ribV4", ribV1, boxRibV, 0, posBoxRibV4);
	ribVxOff = 2.4384*cm;
	const G4ThreeVector posBoxRibV2(ribVxOff+boxRibV_x,ribVyOff-boxRibV_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribV2 = new G4UnionSolid("ribV1", ribV4, boxRibV, 0, posBoxRibV2);
	const G4ThreeVector posBoxRibV3(-(ribVxOff+boxRibV_x),ribVyOff-boxRibV_y,boxB_z+boxRibH_z);
	G4UnionSolid* ribSol = new G4UnionSolid("ribSol", ribV2, boxRibV, 0, posBoxRibV3);
	

	//making brackets
	G4double boxBr_x = 122.9614/2.0*cm;    
    G4double boxBr_y = 32.3342*cm;    
    G4double boxBr_z = boxB_z;
	G4Box* boxBr = new G4Box("boxBr", boxBr_x, boxBr_y, boxBr_z);
	//create bracket round cut
	G4double radBrCut = 52.8574*cm;
    G4double lBrCut = 2.0*boxBr_z;    	  
	G4Tubs* cutBr = new G4Tubs("cutBr", 0.0, radBrCut, lBrCut, 0.0*M_PI, 2.0*M_PI);
	const G4ThreeVector posBrCut(0.0,0.0,0.0);
    G4SubtractionSolid* brCutR = new G4SubtractionSolid("brCutR", boxBr, cutBr, 0, posBrCut);
    
    G4double boxBrC_x = 2.0*boxBr_x;    
    G4double boxBrC_y = boxBr_y;    
    G4double boxBrC_z = 2.0*boxBr_z;
	G4Box* boxBrC = new G4Box("boxBrC", boxBrC_x, boxBrC_y, boxBrC_z);
	const G4ThreeVector posBrSCut(0.0,-boxBr_y,0.0);
     G4SubtractionSolid* brCutSol = new G4SubtractionSolid("brCutSol", brCutR, boxBrC, 0, posBrSCut);

    //making bracket's holders
    G4double boxBrH_x = 48.6664/2.0*cm;    
    G4double boxBrH_y = 2.54/2.0*cm;    
    G4double boxBrH_z = boxBrH_y;
	G4Box* boxBrH = new G4Box("boxBrH", boxBrH_x, boxBrH_y, boxBrH_z);
	G4double brHxOff = 56.642*cm;
	G4double brHyOff = 2.54*cm;
	const G4ThreeVector posBoxBrHbr(brHxOff+boxBrH_x,brHyOff+boxBrH_y,boxB_z+boxBrH_z);
	G4UnionSolid* brHbr = new G4UnionSolid("brHbr", brCutSol, boxBrH, 0, posBoxBrHbr);
	const G4ThreeVector posBoxBrHbl(-(brHxOff+boxBrH_x),brHyOff+boxBrH_y,boxB_z+boxBrH_z);
	G4UnionSolid* brHbl = new G4UnionSolid("brHbl", brHbr, boxBrH, 0, posBoxBrHbl);
	brHyOff = 27.2542*cm;
	const G4ThreeVector posBoxBrHtr((brHxOff+boxBrH_x),brHyOff+boxBrH_y,boxB_z+boxBrH_z);
	G4UnionSolid* brHtr = new G4UnionSolid("brHtr", brHbl, boxBrH, 0, posBoxBrHtr);
	const G4ThreeVector posBoxBrHtl(-(brHxOff+boxBrH_x),brHyOff+boxBrH_y,boxB_z+boxBrH_z);
	G4UnionSolid* brSol = new G4UnionSolid("brSol", brHtr, boxBrH, 0, posBoxBrHtl);

    const G4ThreeVector posBoxBr(0.0,-boxB_y,0.0);
	G4UnionSolid* RibBrSol = new G4UnionSolid("RibBrSol", ribSol, brSol, 0, posBoxBr);
	return RibBrSol;
	
}

void ArgoneFrame::PlaceSkeleton(G4RotationMatrix *pRot, 
                                G4ThreeVector &tlate, 
                                const G4String &pName, 
                                G4LogicalVolume *pMotherLogical,  
                                G4int pCopyNo)
{

	G4double boxVyCent = 154.305*cm;
	G4double boxVx = 5.08/2.0*cm;    
    G4double boxVy = 278.765/2.0*cm;    
    G4double boxVz = boxVx;
    G4double distV = 37.973/2.0*cm;
    G4double boxVyOff = boxVyCent - boxVy;
	G4Box* boxV = new G4Box("boxV", boxVx, boxVy, boxVz);
	
	G4LogicalVolume* boxVLogical = new G4LogicalVolume(boxV, frameMaterial, "boxVLogical"); 
    boxVLogical->SetVisAttributes(argSupportVisAtt);
     
	//making bottom horizontal parts
	G4double boxHbx = 5.08/2.0*cm;    
    G4double boxHby = boxHbx; 
    G4double boxHbz = 120.015/2.0*cm;
	G4Box* boxHb = new G4Box("boxHb", boxHbx, boxHby, boxHbz);
	
	G4LogicalVolume* boxHbLogical = new G4LogicalVolume(boxHb, frameMaterial, "boxHbLogical"); 
     boxHbLogical->SetVisAttributes(argSupportVisAtt);
     
     //making middle and top z horizontal parts     
     G4double boxHmtx = 5.08/2.0*cm;    
     G4double boxHmty = 2.54/2.0*cm; 
     G4double boxHmtz = distV;
	G4Box* boxHmt = new G4Box("boxHmt", boxHmtx, boxHmty, boxHmtz);
	
	G4LogicalVolume* boxHmtLogical = new G4LogicalVolume(boxHmt, frameMaterial, "boxHmtLogical"); 
     boxHmtLogical->SetVisAttributes(argSupportVisAtt);

	
	G4double zShift = -(boxVz+distV);
	//vert left and right
	const G4ThreeVector posBoxVR(0.0,0.0,2.0*zShift);
	G4UnionSolid* boxVLR = new G4UnionSolid("boxVLR", boxV, boxV, 0, posBoxVR);
	//hor bottom
	const G4ThreeVector posBoxHb(0.0,-(boxHby+boxVy),zShift);
	G4UnionSolid* boxVLRHb = new G4UnionSolid("boxVLRHb", boxVLR, boxHb, 0, posBoxHb);
	//hor z middle
	G4double boxHmD = (278.7396/2.0 - 77.8256 + 2.54/2.0)*cm;
	const G4ThreeVector posBoxHm(0.0,-boxHmD,zShift);
	G4UnionSolid* boxVLRHbm = new G4UnionSolid("boxVLRHbm", boxVLRHb, boxHmt, 0, posBoxHm);
	//hor z top
	G4double boxHtD = boxVy + boxHmty;
	const G4ThreeVector posBoxHt(0.0,boxHtD,zShift);
	G4UnionSolid* boxVLRHbmt = new G4UnionSolid("boxVLRHbmt", boxVLRHbm, boxHmt, 0, posBoxHt);
	//left + right
	G4double lrDist = 199.9996/2.0*cm;
	const G4ThreeVector posRight(-2.0*(lrDist+boxVx),0.0,0.0);
	G4UnionSolid* boxFLR = new G4UnionSolid("boxFLR", boxVLRHbmt, boxVLRHbmt, 0, posRight);
	///////////////////////////////////////////////////////
	//making hor x top
	G4double boxHXtx = 210.185/2.0*cm;;    
     G4double boxHXty = 2.54/2.0*cm; 
     G4double boxHXtz = 5.08/2.0*cm;
	G4Box* boxHXt = new G4Box("boxHXt", boxHXtx, boxHXty, boxHXtz);
	///////////////////////////////////////////////////////
	//place hor x top
	const G4ThreeVector posHorXTp(-(lrDist+boxVx),boxHXty+boxVy,0.0);
	G4UnionSolid* boxFLRHorXtp = new G4UnionSolid("boxFLRHorXtp", boxFLR, boxHXt, 0, posHorXTp);
	const G4ThreeVector posHorXTm(-(lrDist+boxVx),boxHXty+boxVy,-2.0*(boxVz+distV));
	G4UnionSolid* boxFLRHorXt = new G4UnionSolid("boxFLRHorXt", boxFLRHorXtp, boxHXt, 0, posHorXTm);
	///////////////////////////////////////////////////////
	//making hor x middle
	G4double boxHXmx = lrDist;    
     G4double boxHXmy = 2.54/2.0*cm; 
     G4double boxHXmz = 5.08/2.0*cm;
	G4Box* boxHXm = new G4Box("boxHXm", boxHXmx, boxHXmy, boxHXmz);
	///////////////////////////////////////////////////////
	//place hor x middle
	G4double yTop = 51.0794*cm;
	G4double yBottom = 73.9394*cm;
	//top p
	const G4ThreeVector posHorXMpt(-(lrDist+boxVx),-yTop,0.0);
	G4UnionSolid* boxFLRHorXMpt = new G4UnionSolid("boxFLRHorXMpt", boxFLRHorXt, boxHXm, 0, posHorXMpt);
	//bottom p
	const G4ThreeVector posHorXMp(-(lrDist+boxVx),-yBottom,0.0);
	G4UnionSolid* boxFLRHorXMp = new G4UnionSolid("boxFLRHorXMp", boxFLRHorXMpt, boxHXm, 0, posHorXMp);
	//top m
	const G4ThreeVector posHorXMmt(-(lrDist+boxVx),-yTop,-2.0*(boxVz+distV));
	G4UnionSolid* boxFLRHorXMmt = new G4UnionSolid("boxFLRHorXMmt", boxFLRHorXMp, boxHXm, 0, posHorXMmt);
	//bottom m
	const G4ThreeVector posHorXMm(-(lrDist+boxVx),-yBottom,-2.0*(boxVz+distV));
	G4UnionSolid* boxFLRHorXMm = new G4UnionSolid("boxFLRHorXMm", boxFLRHorXMmt, boxHXm, 0, posHorXMm);
	///////////////////////////////////////////////////////
	
	G4LogicalVolume* frameLogical = new G4LogicalVolume(boxFLRHorXMm, frameMaterial, "frameLogical"); 
     frameLogical->SetVisAttributes(argSupportVisAtt);

	const G4ThreeVector posFrame(lrDist+boxVx,-boxVyOff,boxVz+distV);
	G4ThreeVector position = posFrame + tlate;
	G4String name = pName+"_frameSkeletonPhys";
                                
                                
	new G4PVPlacement(pRot, position, frameLogical, name, pMotherLogical, 0, pCopyNo );							
							
							
							
}
