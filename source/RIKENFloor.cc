// $Id: RIKENFloor.cc 03.05.2017 A Fijalkowska $
//
/// \file RIKENFloor.cc
/// \brief Implementation of the RIKENFloor class
//
//

#include "RIKENFloor.hh"

#include "G4Transform3D.hh"
#include "globals.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "nDetMaterials.hh"

RIKENFloor::RIKENFloor()
{
    SetBasicSizes();
    materialsManager = MaterialsManager::GetInstance();
    MakeFloor();
}

RIKENFloor::~RIKENFloor()
{
}

void RIKENFloor::SetBasicSizes()
{
    floorThickness = 10 * cm;
    floorWidth = 2 * m;
    floorDistance = 1.245 * m;
}

void RIKENFloor::MakeFloor()
{
    G4Material *concreteMaterial = materialsManager->GetConcrete();
    G4Box *boxFloor = new G4Box("FloorSolid", floorThickness, floorWidth, floorWidth);
    floorLogVol = new G4LogicalVolume(boxFloor, concreteMaterial, "floorLogVol");
    G4VisAttributes *floorVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.5));
    floorVisAtt->SetForceAuxEdgeVisible(true); // Can see outline when drawn with lines
    floorVisAtt->SetForceSolid(true);
    floorLogVol->SetVisAttributes(floorVisAtt);
}

void RIKENFloor::Place(G4RotationMatrix *pRot,
                       G4ThreeVector &tlate,
                       const G4String &pName,
                       G4LogicalVolume *pMotherLogical,
                       G4int pCopyNo)
{
    tlate[0] = tlate[0] - floorThickness / 2.;
    new G4PVPlacement(pRot,
                      tlate,
                      floorLogVol,
                      pName,
                      pMotherLogical,
                      0,
                      pCopyNo);
}
