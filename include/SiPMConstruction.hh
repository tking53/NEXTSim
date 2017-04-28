//
// Created by David Pérez Loureiro on 4/26/17.
//
//New Class to construct the SiPM Assembly


#ifndef NEXTMULTIPLESCATTERING_SIPMCONSTRUCTION_HH
#define NEXTMULTIPLESCATTERING_SIPMCONSTRUCTION_HH

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"


class G4Box;

class SiPMConstruction {

    public:
    /**
	 * Constructor.
	 */
    SiPMConstruction();
    /**
	 * Destructor.
	 */
    ~SiPMConstruction();
    /**
     * Sets the dimensions of the detector
     * @param dimension - the size of the detector.
     * @param thickness - the thickness of the detector.
     */
    void SetSiPM_Dimensions (G4double dimension, G4double thickness){
        fSiPM_Dimensions = dimension;
        fSiPM_Thickness = thickness; }
    /**
     * Sets the Thickness of the window
     * @param thickness - the thickness of the window.
     */
    void SetWindow_Thickness (G4double thickness){ fwindow_Thickness = thickness;}
    /**
     * Sets the Name of the detector
     * @param name - the name of the detector.
     */
    void SetSiPM_Name (G4String name){fname =  name;}
    /**
	 * @return G4double - the dimension of the detector.
	 */
    G4double GetSiPM_Dimension(){ return  fSiPM_Dimensions;}
    /**
     * @return G4double - the thickness of the detector.
     */

    G4VPhysicalVolume    *Build_SiPM(G4RotationMatrix *matrix,G4ThreeVector pos, G4LogicalVolume* parent);

    private:

    G4String             fname;
    G4double             fSiPM_Dimensions;
    G4double             fSiPM_Thickness;
    G4double             fwindow_Thickness;

    G4LogicalVolume      *fSiPM_Logical;
    G4ThreeVector        fPosition;
    G4RotationMatrix     *fRotation;
};


#endif //NEXTMULTIPLESCATTERING_SIPMCONSTRUCTION_HH
