// $Id: ArgoneVandleArray.hh 26.03.2017 A Fijalkowska $
//
/// \file ArgoneVandleArray.hh
/// \brief Definition of the ArgoneVandleArray class
//
#ifndef ArgoneVandleArray_H
#define ArgoneVandleArray_H 1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "VANDLEBar.hh"

class ArgoneVandleArray
{
  public:

    ArgoneVandleArray(); 
    virtual ~ArgoneVandleArray(); 
    virtual void ConstructSDandField();    
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
     void PlaceSmallBars(G4RotationMatrix *pRot, 
                         G4ThreeVector &tlate, 
                         const G4String &pName, 
                         G4LogicalVolume *pMotherLogical,  
                         G4int pCopyNo = 0);
               
               
     void PlaceMediumBars(G4RotationMatrix *pRot, 
                          G4ThreeVector &tlate, 
                          const G4String &pName, 
                          G4LogicalVolume *pMotherLogical,  
                          G4int pCopyNo = 0);
     VANDLEBar* smallBar;
     VANDLEBar* mediumBar;
     std::vector<G4RotationMatrix*> rotations;
     
     G4double smallArchRadius;
     G4double mediumArchRadius;
  	 G4int nrOfSmallModules;
  	 G4int nrOfMediumModules; 
  	
};

#endif
