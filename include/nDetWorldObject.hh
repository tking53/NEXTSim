#ifndef NDET_WORLD_OBJECT_HH
#define NDET_WORLD_OBJECT_HH

#include <string>

#include "globals.hh"

#include "gdmlSolid.hh"

class G4CSGSolid;
class G4LogicalVolume;
class G4PVPlacement;

class nDetDetector;
class nDetMaterials;

/** @class nDetWorldObject
  * @brief User-constructed objects which may be added to the simulation setup area
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date August 13, 2019
  */

class nDetWorldObject{
  public:
	/** Default constructor
	  */
	nDetWorldObject() : argStr(), nReqArgs(0), nUserArgs(0), geometry(NULL), geometry_logV(NULL), geometry_physV(NULL) { }

	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  * @param nargs_ The number of arguments which are required from the user input string
	  */
	nDetWorldObject(const G4String &arg_, const int &nargs_=0) : argStr(arg_), nReqArgs(nargs_), nUserArgs(0), geometry(NULL), geometry_logV(NULL), geometry_physV(NULL) { }

	/** Destructor
	  */
	virtual ~nDetWorldObject(){ }

	/** Return the argument string from the input macro
	  */
	G4String dump() const { return argStr; }

	/** Get the number of arguments expected from the user input string
	  */
	unsigned int getNumRequiredArgs() const { return nReqArgs; }

	/** Get the number of arguments passed to decodeString()
	  */
	unsigned int getNumSuppliedArgs() const { return nUserArgs; }

	/** Get the name of the world object
	  */
	G4String getName() const { return name; }

	/** Get the name of the material used for the volume of the object
	  */
	G4String getMaterialName() const { return material; }

	/** Get a pointer to the geometry of the object
	  */
	G4CSGSolid *getGeometry(){ return geometry; }

	/** Get a pointer to the logical volume of the object
	  */
	G4LogicalVolume *getLogicalVolume(){ return geometry_logV; }

	/** Get a pointer to the physical volume of the object
	  */
	G4PVPlacement *getPhysicalVolume(){ return geometry_physV; }

	/** Get the size of the layer along the X-axis (in mm)
	  */
	G4double getSizeX() const { return size.getX(); }
	
	/** Get the size of the layer along the Y-axis (in mm)
	  */
	G4double getSizeY() const { return size.getY(); }

	/** Get the size of the layer along the Z-axis (in mm)
	  */
	G4double getSizeZ() const { return size.getZ(); }

	/** Read a user input string and decode the relevant values
	  */
	bool decodeString();

	/** Read a user input string and decode the relevant values
	  */
	virtual bool decodeArgs() = 0;

	/** Construct the object and place it into a detector
	  */
	virtual void construct(nDetDetector*) = 0;

	/** Return a string containing proper input string syntax
	  */
	virtual std::string syntaxStr() const = 0;

	/** Construct the object and place it into a logical volume
	  */
	virtual void placeObject(G4LogicalVolume*, nDetMaterials*){ }

  protected:
	G4String argStr; ///< Argument string from the macro supplied by the user

	unsigned int nReqArgs; ///< The number of required user arguments which are expected
	unsigned int nUserArgs; ///< The number of user arguments passed to decodeString()  

	G4CSGSolid *geometry; ///< Pointer to the geometry of the object
	G4LogicalVolume *geometry_logV; ///< Pointer to the logical volume of the object
	G4PVPlacement *geometry_physV; ///< Pointer to the physical volume of the object

	G4String name; ///< The name of the world object
	G4String material; ///< The name of the material used for the volume of the object

	G4ThreeVector position; ///< The position of the center of the object in the world
	G4RotationMatrix rotation; ///< The rotation matrix used for the object
	
	G4ThreeVector size; ///< The physical size of the object

	std::vector<std::string> args; ///< Vector of arguments from the input string

	/** Return true if the input string is a number and return false otherwise
	  */
	bool isNumerical(const std::string &str);
	
	/** Set the internal rotation matrix using a vector with angles about the X, Y, and Z axes (all in degrees)
	  */
	void setRotationMatrix(const G4ThreeVector &vec);
};

/** @class nDetWorldPrimitive
  * @brief 
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date August 13, 2019
  */

class nDetWorldPrimitive : public nDetWorldObject {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	nDetWorldPrimitive(const G4String &arg_);

	/** Destructor
	  */	
	~nDetWorldPrimitive();
	
	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <shape> <posX(cm)> <posY(cm)> <posZ(cm)> <rotX(deg)> <rotY(deg)> <rotZ(deg)> <material> [[p1] [p2] ... [pn]]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | shape     | The name of the 3d shape to build
	  * | posX(Y,Z) | X, Y, and Z position of the center of the model (in mm)
	  * | rotX(Y,Z) | Rotation about the X, Y, and Z axes (in degrees)
	  * | matString | The NIST database name of the material to use for the model
	  */
	virtual bool decodeArgs();

	/** Not implemented
	  */
	virtual void construct(nDetDetector*){ }

	/** Return a string containing proper input string syntax
	  */
	virtual std::string syntaxStr() const ;

	/** Construct the object and place it into a logical volume
	  * @param parent A pointer to the logical volume where the object will be placed
	  * @param materials Pointer to materials object which will be used to look-up the build material
	  */
	virtual void placeObject(G4LogicalVolume *parent, nDetMaterials *materials);

	/** Select the type of 3d geometry to build
	  * @param name The name of the 3d shape
	  */
	bool setShape(const G4String &name);

  private:
	/** Geant shape type enum
	  */
	enum G4SHAPETYPE {BOX, CYLINDER, CONE, PARALLELEPIPED, TRAPEZOID, SHELL, SPHERE, TORUS};
	
	std::map<G4String, G4SHAPETYPE> shapeNameMap; ///< Map between 3d shape names and their corresponding enum values
	
	std::map<G4String, G4double> sizeArgs; ///< Map between named numerical parameters and their values
	
	G4SHAPETYPE shapeSelect; ///< 3d shape type selected by the user
	
	/** Clear all user-specified numerical arguments and set them to their default values for the selected geometry type
	  */
	void resetAllArgs();
	
	/** Check that all required numerical arguments have been set by the user
	  * @return True if all required arguments for the selected 3d geometry have been set by the user and return false otherwise
	  */
	bool checkRequiredArgs() const ;
	
	/** Build the selected geometry type using size parameters obtained from the user input string
	  * @param extName The optional name of the object
	  * @return A pointer to the new geometry object of return NULL in the event of an error
	  */
	G4CSGSolid *buildGeometry(const G4String &extName="");
};

#endif
