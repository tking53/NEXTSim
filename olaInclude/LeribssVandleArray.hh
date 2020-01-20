// $Id: LeribssVandleArray.hh 26.01.2016 A Fijalkowska $
//
/// \file LeribssVandleArray.hh
/// \brief Definition of the LeribssVandleArray class
//
#ifndef LeribssVandleArray_H
#define LeribssVandleArray_H 1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "VANDLEBar.hh"

class LeribssVandleArray
{
  public:

    LeribssVandleArray(); 
    virtual ~LeribssVandleArray(); 
    virtual void ConstructSDandField();    
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
               
  private:
     void SetBasicSizes();
     VANDLEBar* vandleBar;
     
     G4double inch;
     G4double radius;
     G4int moduleSize;
     G4int nrOfModules;
};

#endif
