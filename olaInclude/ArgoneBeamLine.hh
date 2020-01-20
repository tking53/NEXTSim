// $Id: ArgoneBeamLine.hh 10.05.2017 A Fijalkowska $
//
/// \file ArgoneBeamLine.hh
/// \brief Definition of the ArgoneBeamLine class
//
#ifndef ArgoneBeamLine_H
#define ArgoneBeamLine_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

class ArgoneBeamLine
{
  public:

    ArgoneBeamLine(); 
    virtual ~ArgoneBeamLine(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();                                                                                      
     MaterialsManager* materialsManager;
     G4double inch;

};



#endif
