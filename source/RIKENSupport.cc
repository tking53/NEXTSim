
// $Id: RIKENSupport.cc 07.05.2017 A Fijalkowska $
//
/// \file RIKENSupport.cc

//
//

#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "RIKENSupport.hh"
#include "CADMesh.hh"

RIKENSupport::RIKENSupport()
{
    materialsManager = MaterialsManager::GetInstance();
    
    argSupportVisAtt = new G4VisAttributes( G4Colour(248./255.,248./255.,255./255.)); 
	argSupportVisAtt->SetForceAuxEdgeVisible(true);// Can see outline when drawn with lines
	argSupportVisAtt->SetForceSolid(true);
	//SetBasicSizes();
} 

RIKENSupport::~RIKENSupport()
{

}


void RIKENSupport::Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo)
{

    frameMaterial = materialsManager->GetAluminum();
    

   std::string file1 = "/ARCHIVE/Ddata/geant4_stl/vandle/riken2018/RIKEN_VANDLE_FULL_Simplified_Part_new.stl";
   CADMesh * rebMesh = new CADMesh((char*)file1.c_str());
   rebMesh->SetScale(cm);

  // CADMesh* rebMesh = new CADMesh("/ARCHIVE/Ddata/geant4_stl/vandle/isolde/VerticalFrame.stl", cm, offset, false);
   G4VSolid* ribSolid =  rebMesh->TessellatedMesh();
   G4LogicalVolume* ribLogic = new G4LogicalVolume(ribSolid, frameMaterial, "RIKENRrameRib");
   ribLogic->SetVisAttributes(argSupportVisAtt);
   
   G4RotationMatrix ribRot(*pRot);
   ribRot.rotateX(-90.0*deg);
   ribRot.rotateY(180.0*deg);
   G4ThreeVector ribPos(0 ,-5.0*cm, -7.0*cm);
   ribPos = tlate + ribPos;
   G4Transform3D transfRib(ribRot,ribPos);     
   new G4PVPlacement(transfRib,ribLogic,"frameRibs", pMotherLogical, 0, 0);
	
	
  // PlaceSkeleton(pRot, tlate, pName, pMotherLogical, pCopyNo);
//112.9284

}


G4VSolid* RIKENSupport::AddHolders(G4VSolid*ribsSolid)
{

	
}

void RIKENSupport::PlaceSkeleton(G4RotationMatrix *pRot, 
                                G4ThreeVector &tlate, 
                                const G4String &pName, 
                                G4LogicalVolume *pMotherLogical,  
                                G4int pCopyNo)
{
						
							
}
