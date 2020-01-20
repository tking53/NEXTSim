// $Id: CERNVandleArray.hh 28.04.2017 A Fijalkowska $
//
/// \file CERNVandleArray.hh
/// \brief Definition of the CERNVandleArray class
//
#ifndef CERNVandleArray_H
#define CERNVandleArray_H 1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "VANDLEBar.hh"

class CERNVandleArray
{
  public:

    CERNVandleArray(); 
    virtual ~CERNVandleArray(); 
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
