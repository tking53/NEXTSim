/************************************************************************
 * \file CloverSingleBuchDetector.hh
 *
 * \class CloverSingleBuchDetector   
 * \brief Design of a HPGe crystal of a clover detector (Bucharest)
 *                                                                                                                                                                                                                       
 * \author csotty@gmail.com
 *
 *************************************************************************/

#ifndef CloverSingleBuchDetector_H
#define CloverSingleBuchDetector_H 1

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
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "CloverSingleBuchSD.hh"
#include "G4ios.hh"
#include "CADMesh.hh"

#ifdef G4VIS_USE
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#endif


class CloverSingleBuchDetector : public G4VUserDetectorConstruction
{
    public:
      CloverSingleBuchDetector(G4VPhysicalVolume* p_mother, G4Transform3D p_transformation, G4int p_cl_nb, G4int p_cr_nb, G4VisAttributes* pvis_att);

      ~CloverSingleBuchDetector();

      G4VPhysicalVolume* Construct();

      
    private:
      G4String name;

      // Solid, Physical, Logical, CADMesh and Sensitive volumes of the CloverSingle detector
      //
      G4VSolid* CloverSingle_sol;

      CADMesh* mesh_CloverSingle;

      G4VPhysicalVolume* CloverSingle_phys;

      G4LogicalVolume* CloverSingle_log;

      CloverSingleBuchSD* cloverSingleBuchSD;
  
      // Physical and Logical volumes of the environnement
      //
      G4VPhysicalVolume* mother;

      // Position and Orientation
      //
      G4Transform3D transformation;


      // Visualisation
      //
      G4VisAttributes* det_vis_att;
      G4int cl_nb;
      G4int cr_nb;
};

#endif
    
