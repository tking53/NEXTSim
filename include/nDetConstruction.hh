#ifndef NDET_CONSTRUCTION_HH
#define NDET_CONSTRUCTION_HH

#include <vector>

#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "gdmlSolid.hh"
#include "centerOfMass.hh"
#include "nDetDetector.hh"
#include "nDetMaterials.hh"

// Class declarations
class nDetConstructionMessenger;
class G4LogicalVolume;
class G4Material;
class G4VPhysicalVolume;
class G4PVPlacement;

/** @class nDetConstruction
  * @brief Handles construction of NEXTSim detector setups
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 10, 2019
  */

class nDetConstruction : public G4VUserDetectorConstruction{
  public:
	/** Destructor
	  */
	~nDetConstruction();

	/** Copy constructor. Not implemented for singleton class
	  */
	nDetConstruction(nDetConstruction const &copy);

	/** Assignment operator. Not implemented for singleton class
	  */	
	nDetConstruction &operator=(nDetConstruction const &copy);

	/** Return an instance of this class
	  */
	static nDetConstruction &getInstance();

	/** Construct the detector setup
	  * @note This method does nothing on its own, all the work is done by ConstructDetector()
	  * @return A pointer to the world physical volume
	  */
	G4VPhysicalVolume* Construct();

	/** Build the world volume and place all detectors defined in the detector list
	  * @return A pointer to the world physical volume
	  */
	G4VPhysicalVolume* ConstructDetector();

	/** Add a detector geometry to the list of detectors
	  * @note See nDetDetector::setGeometry() for accepted geometry names
	  * @return True if the specified type is recognized and return false otherwise
	  */
	bool AddGeometry(const G4String &geom);

	/** Setup segmented PMTs
	  * @param col_ Number of columns (along the width)
	  * @param row_ Number of rows (along the height)
	  * @param width_ Width of the active area of the photo-sensitive surface (in mm)
	  * @param height_ Height of the active area of the photo-sensitive surface (in mm)
	  */
	void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);

	/** Load PMT quantum efficiency spectrum from a file
	  * @note See centerOfMass::loadSpectralResponse() for information on required file contents
	  * @param fname Path to file containing anode quantum efficiency spectrum
	  * @return True if the quantum efficiency is loaded correctly and return false otherwise
	  */
	bool setPmtSpectralResponse(const char *fname);

	/** Load segmented PMT anode gain matrix from a file
	  * @note See centerOfMass::loadGainMatrix() for information on required file contents
	  * @param fname Path to file containing a matrix of anode gain percentages
	  * @return True if the anode gain matrix is loaded correctly and return false otherwise
	  */
	bool setPmtGainMatrix(const char *fname);

	/** Set the X, Y, and Z size of the shadow bar (all in mm)
	  */
	void SetShadowBarSize(const G4ThreeVector &size);
	
	/** Set the X, Y, and Z position of the center of the shadow bar (all in mm)
	  */
	void SetShadowBarPosition(const G4ThreeVector &pos);

	/** Set the material used for the shadow bar
	  * @param material The NIST database name of the material to use
	  * @return True if the material exists in the database and return false otherwise
	  */
	bool SetShadowBarMaterial(const G4String &material);
	
	/** Get a pointer to the messenger used for this class
	  */
	nDetConstructionMessenger *GetMessenger(){ return fDetectorMessenger; }

	/** Return a pointer to the optical photon center-of-mass calculator for the left PMT
	  */
	centerOfMass *GetCenterOfMassL(){ return &center[0]; }

	/** Return a pointer to the optical photon center-of-mass calculator for the right PMT
	  */
	centerOfMass *GetCenterOfMassR(){ return &center[1]; }

	/** Return a pointer to the PMT response for the left PMT
	  */
	pmtResponse *GetPmtResponseL(){ return center[0].getPmtResponse(); }

	/** Return a pointer to the PMT response for the right PMT
	  */
	pmtResponse *GetPmtResponseR(){ return center[1].getPmtResponse(); }

	/** Get a copy of the current vector of detectors
	  */
	std::vector<nDetDetector> GetUserDetectors() const { return userDetectors; }

	/** Get the current user defined detector parameters
	  */
	nDetDetectorParams GetDetectorParameters() const { return params; }

	/** Get a pointer to the current detector
	  */
	nDetDetector *getCurrentDetector(){ return currentDetector; }

	/** Clear all volumes, surfaces, and detectors
	  */
	void ClearGeometry();

	/** Place all detectors into the world and copy the list of detectors to all user run actions
	  */	
	void UpdateGeometry();

	/** Load a GDML model from a file using parameters from a space-delimited input string and place it into the world volume
	  * @note See loadGDML(const G4String &) for input string syntax
	  */
	void LoadGDML(const G4String &input);

	/** Load a GDML model from a file using parameters from a space-delimited input string and place it into the current detector assembly
	  * @note See loadGDML(const G4String &) for input string syntax
	  */
	void AddGDML(const G4String &input);

	/** Load a light guide model from a file using parameters from a space-delimited input string and place it into the current detector assembly
	  * @note See loadLightGuide(const G4String &) for input string syntax
	  */
	void AddLightGuideGDML(const G4String &input);

	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note See applyGreaseLayer(const G4String &) for input string syntax
	  */
	void AddGrease(const G4String &input);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See applyDiffuserLayer(const G4String &) for input string syntax
	  */
	  void AddDiffuser(const G4String &input);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See applyLightGuide(const G4String &) for input string syntax
	  */
	void AddLightGuide(const G4String &input);
	
	/** Add an array of detectors in a cylindrical setup
	  * @note String syntax: <geom> <r0> <startTheta> <stopTheta> <Ndet> <br>
	  * | Parameter  | Description |
	  * |------------|-------------|
	  * | geom       | Name of detector geometry (see nDetDetector::setGeometry() for valid types)
	  * | startTheta | The angle of the first detector (in degrees)
	  * | stopTheta  | The angle of the final detector (in degrees)
	  * | Ndet       | Number of detectors in the array
	  */
	void AddDetectorArray(const G4String &input);

	/** Set the light yield multiplier for scintillator photon production
	  * @param yield The fraction of the light yield to use for optical photon production in scintillators (default is 1)
	  */	
	void SetLightYieldMultiplier(const G4double &yield);
	
  private:
	nDetConstructionMessenger *fDetectorMessenger; ///< Geant messenger to use for this class
	
	G4double expHallX; ///< Size of the world along the x-axis (in mm)
	G4double expHallY; ///< Size of the world along the y-axis (in mm)
	G4double expHallZ; ///< Size of the world along the z-axis (in mm)

	nDetDetectorParams params; ///< Physical attributes of the current detector assembly

	G4bool fCheckOverlaps; ///< Flag indicating that Geant should check for overlaps between all placed objects

	G4LogicalVolume* expHall_logV; ///< Logical volume of the world
	G4VPhysicalVolume* expHall_physV; ///< Physical volume of the world

	std::vector<gdmlSolid> solids; ///< Vector of all loaded gdml solids

	G4ThreeVector shadowBarSize; ///< Size of the shadow object in 3d space
	G4ThreeVector shadowBarPos; ///< Position of the shadow object in 3d space
	G4Material *shadowBarMaterial; ///< Pointer to the material used for the shadow object

	std::vector<nDetDetector> userDetectors; ///< Vector of all detectors added by the user
	
	nDetDetector *currentDetector; ///< Pointer to the current detector added by the user

	centerOfMass center[2]; ///< Objects used to compute the detected optical photon center-of-mass position for the left and right PMT

	nDetMaterials materials; ///< Object containing all Geant materials and elements which will be used in detector construction

	/** Default constructor. Private for singleton class
	  */
	nDetConstruction();

	/** Build the experiment hall physical and logical volumes
	  */
	void buildExpHall();

	/** Load a GDML model from a file using parameters from a space-delimited input string
	  * @note String syntax: <filename> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString>
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | filename  | Filename of the input GDML file
	  * | posX(Y,Z) | X, Y, and Z position of the center of the model (in mm)
	  * | rotX(Y,Z) | Rotation about the X, Y, and Z axes (in degrees)
	  * | matString | The NIST database name of the material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	gdmlSolid *loadGDML(const G4String &input);

	/** Load a GDML model from a file
	  *
	  * The model will not be placed into the setup until ConstructDetector() is called
	  *
	  * @param fname Filename of the input GDML file
	  * @param position Vector containing the position of the model (in mm)
	  * @param rotation Vector containing the rotation about the X, Y, and Z axes (in degrees)
	  * @param material String containing the name of the Geant material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	gdmlSolid *loadGDML(const G4String &fname, const G4ThreeVector &position, const G4ThreeVector &rotation, const G4String &material);
};

#endif
