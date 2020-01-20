// $Id: LeribssBeam.hh 29.01.2016 A Fijalkowska $
//
/// \file LeribssBeam.hh
/// \brief Definition of the LeribssBeam class
//
#ifndef LeribssBeam_H
#define LeribssBeam_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class LeribssBeam
{
  public:

    LeribssBeam(); 
    virtual ~LeribssBeam(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
     
     void MakeIBeam(G4RotationMatrix *pRot, 
                     G4ThreeVector &tlate, 
                     const G4String &pName, 
                     G4LogicalVolume *pMotherLogical,  
                     G4int pCopyNo);
    void MakeCeilIBeam(G4RotationMatrix *pRot, 
                       G4ThreeVector &tlate, 
                       const G4String &pName, 
                       G4LogicalVolume *pMotherLogical,  
                       G4int pCopyNo);                                                         
    void BeamLine(G4RotationMatrix *pRot, 
                  G4ThreeVector &tlate, 
                  const G4String &pName, 
                  G4LogicalVolume *pMotherLogical,  
                  G4int pCopyNo);                                                                                         
     MaterialsManager* materialsManager;
     //G4VisAttributes* supportVisAtt;
     
     G4double inch; 
};



#endif
