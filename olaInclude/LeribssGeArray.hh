// $Id: LeribssGeArray.hh 24.01.2016 A Fijalkowska $
//
/// \file LeribssGeArray.hh
/// \brief Definition of the LeribssGeArray class
//
#ifndef LeribssGeArray_H
#define LeribssGeArray_H 1
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "HPGe.hh"

class LeribssGeArray
{
  public:

    LeribssGeArray(); 
    virtual ~LeribssGeArray();     
    void Place(G4RotationMatrix *pRot, 
               G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical,  
               G4int pCopyNo = 0);
    virtual void ConstructSDandField();           
  private:
     void SetBasicSizes();
     HPGe* HPGeDet;
     
     G4double HPGeXOffset;
};

#endif
