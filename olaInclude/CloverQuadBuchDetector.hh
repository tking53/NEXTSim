                                                                                                                                                                                                                         
/************************************************************************
 * \file CloverQuadBuchDetector.hh
 *
 * \class CloverQuadBuchDetector   
 * \brief Design of a HPGe crystal of a clover detector (Bucharest)
 *
 * \author csotty@gmail.com
 * 
 *************************************************************************/

#ifndef CloverQuadBuchDetector_H
#define CloverQuadBuchDetector_H 1


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
#include "CloverSingleBuchDetector.hh"

// Include-files for detector readout
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "CADMesh.hh"

#ifdef G4VIS_USE
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#endif

// Clover Detector Class
class CloverQuadBuchDetector : public G4VUserDetectorConstruction
{
    public:
      CloverQuadBuchDetector(G4VPhysicalVolume* p_mother, G4double p_rho, G4double p_theta, G4double p_phi, G4double p_spin, G4int p_cl_nb);
      ~CloverQuadBuchDetector();

      G4VPhysicalVolume* Construct();

    private:
      // Crystals defined by the CloverSingleDetector class
      //
      CloverSingleBuchDetector* Crystal0;
      CloverSingleBuchDetector* Crystal1;
      CloverSingleBuchDetector* Crystal2;
      CloverSingleBuchDetector* Crystal3;

      // Solid, Physical, Logical and CADMesh  volumes of the Capsules and Carbon window
      //
      G4VSolid* Capsule_sol;

      CADMesh* mesh_Capsule;

      G4VPhysicalVolume* Capsule_phys;

      G4LogicalVolume* Capsule_log;

      // Physical and Logical volumes of the environnement   
      //
      G4VPhysicalVolume* det_env;
      G4VPhysicalVolume* mother;

      G4LogicalVolume* det_env_log;
      G4LogicalVolume* mother_log;
    
      // Position and Orientation
      //
      G4double rho;
      G4double phi;
      G4double theta;
      G4double spin;
      
   
      // Visualisation
      //
      G4VisAttributes* Crystal0_vis_att;
      G4VisAttributes* Crystal1_vis_att;
      G4VisAttributes* Crystal2_vis_att;
      G4VisAttributes* Crystal3_vis_att;
      const G4VisAttributes* Capsule_vis_att;
    
      // Numbering of the clovers 
      //
      G4int cl_nb;
};

#endif
