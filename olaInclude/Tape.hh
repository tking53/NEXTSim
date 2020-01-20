                                                                                                                                                                                                                         
/************************************************************************
 * \file Tape.hh
 *
 * \class Tape
 * \brief Design of Tape 
 *
 * \author Christophe Sotty
 * 	   csotty@gmail.com
 * 
 *************************************************************************/

#ifndef Tape_H
#define Tape_H 1


// Include-files for detector construction
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4Trap.hh"
#include "G4Sphere.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4DisplacedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"

// Include-files for detector readout
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "CADMesh.hh"

#ifdef G4VIS_USE
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#endif

using namespace std;

// Clover Detector Class
class Tape : public G4VUserDetectorConstruction
{
    public:
      Tape(G4VPhysicalVolume* p_mother);
      ~Tape();

      G4VPhysicalVolume* Construct();

    private:
      // Physical and Logical volumes 
      //
      G4VPhysicalVolume*	mother;
      G4VPhysicalVolume*	Tape_imp_phys;
      G4VPhysicalVolume*	Tape_back_phys;

      G4LogicalVolume*		mother_log;
      G4LogicalVolume*		Tape_imp_log;
      G4LogicalVolume*		Tape_back_log;
    
      // Visualisation
      //
      G4VisAttributes* Tape_vis_att;
    
};

#endif
