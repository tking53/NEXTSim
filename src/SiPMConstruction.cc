//
// Created by David Pérez Loureiro on 4/26/17.
//

#include "SiPMConstruction.hh"
#include "G4SIunits.hh"
#include "G4Box.hh"

SiPMConstruction::SiPMConstruction():fname("SiPM"),fSiPM_Dimensions(6*mm),fSiPM_Thickness(0.07*mm),
                                     fwindow_Thickness(0.35*mm),fPosition(G4ThreeVector(0,0,0)),fRotation(NULL) {
    ;
}

SiPMConstruction::~SiPMConstruction() {

    ;
}


G4VPhysicalVolume * SiPMConstruction::Build_SiPM(G4RotationMatrix *matrix, G4ThreeVector pos,
                                                 G4LogicalVolume *parent) {

    G4Box SiPMchip(fname,fSiPM_Dimensions/2,fSiPM_Dimensions/2,fSiPM_Thickness/2);

    //fSiPM_Logical= new G4LogicalVolume(&SiPMchip,);


}