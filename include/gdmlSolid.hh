#include <vector>
#include <string>

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4OpticalSurface.hh"

#include "globals.hh"

#ifdef USE_GDML
class G4GDMLParser;
#endif

/** @class gdmlSegment
  * @brief A daughter segment of a gdmlSolid
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 12, 2019
  */

class gdmlSegment{
  public:
	/** Default constructor
	  */
	gdmlSegment();

#ifdef USE_GDML
	/** GDML constructor
	  * @param parser Pointer to Geant GDML file parser
	  * @param daughter ID number of the daughter segment in the parent logical volume
	  */
	gdmlSegment(G4GDMLParser *parser, const G4int &daughter);
#endif

	/** Get the X-axis unit vector in the frame of the segment
	  */
	G4ThreeVector getUnitX() const { return unit[0]; }

	/** Get the Y-axis unit vector in the frame of the segment
	  */
  	G4ThreeVector getUnitY() const { return unit[1]; }

	/** Get the Z-axis unit vector in the frame of the segment
	  */
	G4ThreeVector getUnitZ() const { return unit[2]; }

	/** Get the position of the center of the segment
	  */
	G4ThreeVector getOffset() const { return pos; }

	/** Get the position of the center of the segment in the frame of an external rotation matrix
	  */	
	G4ThreeVector getOffset(const G4RotationMatrix &matrix) const ;

	/** Get a pointer to the rotation matrix of this segment's frame
	  */
	G4RotationMatrix *getRotationMatrix(){ return rot; }

	/** Get a pointer to the physical volume of the segment (if it has been placed into an assembly)
	  */
	G4VPhysicalVolume *getPhysicalVolume(){ return physV; }

	/** Get a pointer to the logical volume of the segment
	  */	
	G4LogicalVolume *getLogicalVolume(){ return logV; }

	/** Set the rotation of the segment frame using an external rotation matrix
	  */
	void rotate(const G4RotationMatrix &matrix);

	/** Set the rotation of the segment frame about the X, Y, and Z axes (all in degrees)
	  */
	void rotate(const double &x, const double &y, const double &z);

	/** Transform an input vector into the rotated frame of this segment and return the result
	  */
	G4ThreeVector transform(const G4ThreeVector &vec) const ;

  private:
	G4RotationMatrix *rot; ///< Rotation of the segment in 3d space
	G4VPhysicalVolume *physV;  ///< Physical volume of the segment
	G4LogicalVolume *logV; ///< Logical volume of the segment
	
	G4ThreeVector pos; ///< Position of the center of the segment
	G4ThreeVector unit[3]; ///< Local unit vectors for the x, y, and z axes
};

/** @class gdmlSolid
  * @brief Used to load a 3d model from an input GDML file
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 12, 2019
  *
  * Contains a collection of gdmlSegment objects representing a GDML model
  */

class gdmlSolid{
  public:
	/** Default constructor
	  */
	gdmlSolid() : name(), parent(NULL), parent_logV(NULL), parent_physV(NULL), world(NULL), worldLogV(NULL), nDaughters(0) { }
	
	/** GDML file constructor
	  * @param fname Path to the GDML input file
	  * @param materialName The name of the model material in the NIST database
	  */
	gdmlSolid(const char *fname, const G4String &materialName);

	/** Get the name of the model from the file
	  */
	std::string getName() const { return name; }

	/** Get the number of sub-segment daughters which comprise the model
	  */
	size_t getNumDaughters() const { return nDaughters; }

	/** Return true if the model is loaded and return false otherwise
	  */
	bool isLoaded() const { return (nDaughters > 0); }

	/** Get the width (X-axis) of the model in its local frame
	  */
	double getWidth() const ;

	/** Get the thickness (Y-axis) of the model in its local frame
	  */
	double getThickness() const ;

	/** Get the length (Z-axis) of the model in its local frame
	  */
	double getLength() const ;

	/** Get the maximum size of the model in the X-direction (in mm)
	  */
	double getSizeX() const { return 2*size.getX(); }

	/** Get the maximum size of the model in the Y-direction (in mm)
	  */
	double getSizeY() const { return 2*size.getY(); }

	/** Get the maximum size of the model in the Z-direction (in mm)
	  */
	double getSizeZ() const { return 2*size.getZ(); }	

	/** Get the X, Y, and Z position of the center of the model (all in mm)
	  */
	G4ThreeVector *getPosition(){ return &position; }

	/** Get a pointer to the rotation matrix representing the rotated local frame
	  */	
	G4RotationMatrix *getRotation(){ return &rotation; }

	/** Get a pointer to the bounding box of the model
	  *
	  * The size of the bounding box is equal to the maximum size of the model along all three axes
	  */
	G4Box *getBoundingBox(){ return parent; }

	/** Get a pointer to the logical volume of the parent
	  */	
	G4LogicalVolume *getLogicalVolume(){ return parent_logV; }

	/** Get a pointer to the physical volume of the parent
	  */
	G4PVPlacement *getPhysicalVolume(){ return parent_physV; }

	/** Get a pointer to the logical volume of the parent
	  */
	G4LogicalVolume *getWorldLogicalVolume(){ return worldLogV; }

	/** Read 
	  * @param fname Path to the GDML input file
	  * @param materialName The name of the model material in the NIST database
	  * @param checkOverlaps Flag indicating that geant should check for overlaps when placing sub-segments into the parent assembly
	  * @return A pointer to the logical volume of the parent which contains all the sub-segment daughters
	  */
	G4LogicalVolume *read(const char *fname, const G4String &materialName, const bool &checkOverlaps=false);

	/** Place the parent assembly into an external logical volume
	  * @note Uses the internal position and rotation of the model when placing sub-segments
	  * @param parent_ Pointer to the logical volume in which the parent assembly will be placed
	  * @param checkOverlaps Flag indicating that geant should check for overlaps when placing the parent assembly into the external volume
	  * @return A pointer to the physical volume of the placed model
	  */
	G4PVPlacement *placeSolid(G4LogicalVolume *parent_, const bool &checkOverlaps=false);

	/** Place the parent assembly into an external logical volume
	  * @param rot Pointer a rotation matrix representing the desired rotation frame of the model in the external assembly
	  * @param pos Position of the center of the model in the frame of the external assembly
	  * @param parent_ Pointer to the logical volume in which the parent assembly will be placed
	  * @param checkOverlaps Flag indicating that geant should check for overlaps when placing the parent assembly into the external volume
	  * @return A pointer to the physical volume of the placed model
	  */
	G4PVPlacement *placeSolid(G4RotationMatrix *rot, const G4ThreeVector &pos, G4LogicalVolume *parent_, const bool &checkOverlaps=false);

	/** Individually place each sub-segment into an external logical volume
	  * @param rot Pointer a rotation matrix representing the desired rotation frame of the sub-segment in the external assembly
	  * @param pos Position of the center of the sub-segment in the frame of the external assembly
	  * @param parent_ Pointer to the logical volume in which the sub-segments will be placed
	  * @param volumes Vector where physical volumes of placed sub-segments will be stored
	  * @param checkOverlaps Flag indicating that geant should check for overlaps when placing sub-segments into the external assembly
	  */
	void placeSolid(G4RotationMatrix *rot, const G4ThreeVector &pos, G4LogicalVolume *parent_, std::vector<G4PVPlacement*> &volumes, const bool &checkOverlaps=false);

	/** Set logical surface borders between all segment placements and themselves
	  * @param borderName The name of the border surface
	  * @param surface The optical surface to use for the logical border
	  */
	void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface);

	/** Set logical surface borders between all segment placements and an external physical volume @a phys
	  * @param borderName The name of the border surface
	  * @param surface The optical surface to use for the logical border
	  * @param phys Vector of pointers to physical volumes
	  */
	void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, G4PVPlacement *phys);

	/** Set logical surface borders between a set of physical volumes @a phys and themselves
	  * @note A logical border will not be defined between a volume and itself
	  * @param borderName The name of the border surface
	  * @param surface The optical surface to use for the logical border
	  * @param phys Vector of pointers to physical volumes
	  */
	static void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &phys);

	/** Set logical surface borders between a set of physical volumes @a p1 and a physical volume @a p2
	  * @param borderName The name of the border surface
	  * @param surface The optical surface to use for the logical border
	  * @param p1 Vector of pointers to physical volumes
	  * @param p2 Pointer to a physical placement which 
	  */
	static void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &p1, G4PVPlacement *p2);

	/** Set logical surface borders between two sets of physical volumes @a p1 and @a p2
	  * @note If a physical volume exists in both lists, a logical border will not be defined between that volume and itself
	  * @param borderName The name of the border surface
	  * @param surface The optical surface to use for the logical border
	  * @param p1 Vector of pointers to physical volumes
	  * @param p2 Vector of pointers to physical volumes
	  */
	static void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &p1, const std::vector<G4PVPlacement*> &p2);

	/** Set the X, Y, and Z positions of center of parent assembly volume (all in mm)
	  */
	void setPosition(const G4ThreeVector &pos_){ position = pos_; }

	/** Set the rotation of the local parent assembly frame about the X, Y, and Z axes (all in radians)
	  */
	void setRotation(const G4ThreeVector &rot_);

	/** Set the rotation of the local parent assembly frame about the X, Y, and Z axes (all in degrees)
	  */
	void rotate(const double &x, const double &y, const double &z);

	/** Clear all geometry volumes and logical borders
	  */
	void clear();

  private:
	std::string name; ///< Name of the world logical volume
  
	G4Box *parent; ///< Bounding box for the parent volume
	G4LogicalVolume *parent_logV; ///< Logical volume of the parent
	G4PVPlacement *parent_physV; ///< Physical volume of the parent
  
	G4VPhysicalVolume *world; ///< Physical volume of the world
	G4LogicalVolume *worldLogV; ///< Logical volume of the world
  
	std::vector<G4PVPlacement*> placements; ///< Vector of physical volumes of all children
	std::vector<G4LogicalBorderSurface*> borders; ///< Vector of logical border surfaces between all children

	std::vector<gdmlSegment> daughters; ///< Vector of all daughter segments
	
	size_t nDaughters; ///< Number of daughter segments
	
	G4ThreeVector position; ///< Position of center of parent volume
	G4RotationMatrix rotation; ///< Rotation of parent volume in 3d space
	
	G4ThreeVector size; ///< Size of the parent volume along the x, y, and z axes
};
