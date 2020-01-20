// $Id: ArgoneFrame.hh 26.01.2016 A Fijalkowska $
//
/// \file ArgoneFrame.hh
/// \brief Definition of the ArgoneFrame class
//
#ifndef CERNSupport_H
#define CERNSupport_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"
#include "ArgoneFrame.hh"

class CERNSupport
{
  public:

    CERNSupport(); 
    virtual ~CERNSupport(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
     void PlaceSkeleton(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo = 0);
                        
     G4VSolid* AddHolders(G4VSolid*ribsSolid);
     G4VisAttributes* argSupportVisAtt;                                                                                        
     MaterialsManager* materialsManager;
     ArgoneFrame* argoneFrame;
     G4Material* frameMaterial;
     G4double ribX;
     G4double ribY;
     G4double ribZ;
};



#endif
