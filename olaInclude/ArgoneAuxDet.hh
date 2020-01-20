// $Id: ArgoneFrame.hh 10.05.2017 A Fijalkowska $
//
/// \file ArgoneFrame.hh
/// \brief Definition of the ArgoneFrame class
//
#ifndef ArgoneAuxDet_H
#define ArgoneAuxDet_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"

class ArgoneAuxDet
{
  public:

    ArgoneAuxDet(); 
    virtual ~ArgoneAuxDet(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);

               
  private:
     void SetBasicSizes();
                        
    // G4VisAttributes* argSupportVisAtt;                                                                                        
     MaterialsManager* materialsManager;


};



#endif
