                                                                                                                                                                                                                         
/************************************************************************
 * \file Polyhedron.hh
 *
 * \class Polyhedron
 * \brief Design of Polyhedron frame holding the HPGe detectors
 *
 * \author csotty@gmail.com
 * 
 *************************************************************************/

#ifndef Polyhedron_H
#define Polyhedron_H 1


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
class Polyhedron : public G4VUserDetectorConstruction
{
    public:
      Polyhedron(G4VPhysicalVolume* p_mother, G4double p_rho, G4double p_theta, G4double p_phi, G4double p_spin);
      ~Polyhedron();

      G4VPhysicalVolume* Construct();

    private:
      // Solid, Physical, Logical and CADMesh  volumes of the Capsules and Carbon window
      //
      vector<G4VSolid* >		Polyhedron_current_sol;
      vector<CADMesh* >			mesh_Polyhedron_current;
      vector<G4VPhysicalVolume* >	Polyhedron_current_phys;
      vector<G4LogicalVolume* >		Polyhedron_current_log;

      // Physical and Logical volumes of the environnement   
      //
      G4VPhysicalVolume*	det_env;
      G4VPhysicalVolume*	mother;

      G4LogicalVolume*		det_env_log;
      G4LogicalVolume*		mother_log;
    
      // Position and Orientation
      //
      G4double	rho;
      G4double	phi;
      G4double	theta;
      G4double	spin;
      G4RotationMatrix	Polyhedron_rotation;
      G4Transform3D*	Polyhedron_transformation;
   
      // Visualisation
      //
      G4VisAttributes* Polyhedron_vis_att;
    
};

#endif
