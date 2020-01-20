// $Id: LeribssBetaDet.hh 01.02.2017 A Fijalkowska $
//
/// \file LeribssBetaDet.hh
/// \brief Definition of the LeribssBetaDet class
//
#ifndef LeribssBetaDet_H
#define LeribssBetaDet_H1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4Material.hh"
class LeribssBetaDet
{
  public:

    LeribssBetaDet(); 
    virtual ~LeribssBetaDet(); 
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
     
     void BetaLine(G4RotationMatrix *pRot, 
                     G4ThreeVector &tlate, 
                     const G4String &pName, 
                     G4LogicalVolume *pMotherLogical,  
                     G4int pCopyNo);
                     
     void LightCollection(G4RotationMatrix *pRot, 
                          G4ThreeVector &tlate, 
                          const G4String &pName, 
                          G4LogicalVolume *pMotherLogical,  
                          G4int pCopyNo); 
     void PMTtube(G4RotationMatrix *pRot, 
                  G4ThreeVector &tlate, 
                  const G4String &pName, 
                  G4LogicalVolume *pMotherLogical,  
                  G4int pCopyNo);     
                                                                                                         
     MaterialsManager* materialsManager;
     G4VisAttributes* betaDetVisAtt;
     
     G4double inch; 
     G4double betaInnRad;
     G4double betaOutRad;
     G4double betaLength;
     
     G4double lightBoxX;	
  	 G4double lightBoxY;
  	 G4double lightBoxZ;
  	 G4double coneLargeRadius;
     G4double coneSmallRadius;
     G4double coneLength;
};



#endif
