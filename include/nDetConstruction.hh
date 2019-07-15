#ifndef NDET_CONSTRUCTION_HH
#define NDET_CONSTRUCTION_HH

#include <vector>

#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "gdmlSolid.hh"
#include "centerOfMass.hh"
#include "nDetMaterials.hh"

// Class declarations
class nDetConstructionMessenger;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4OpticalSurface;
class G4VPhysicalVolume;
class G4PVPlacement;
class G4Box;
class G4VisAttributes;
class G4Step;

class userAddLayer;
class userAddDetector;

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
	  * @param geom String containing the name of the geometry type (next, module, ellipse, rectangle, cylinder, test)
	  * @return True if the specified type is recognized and return false otherwise
	  */
	bool AddGeometry(const G4String &geom);

	/** Set the width and height of the PMT optical surface
	  * @param input Space-delimited string with the format "<width(mm)> [height(mm)]"
	  *              If the height is not specified, the height will be equal to the width
	  */
	void SetPmtDimension(const G4String &input);

	/** Set the width and height of the PMT optical surface
	  * @param width The width (x-dimension) in mm
	  * @param height The height (y-dimension) in mm. If less than or equal to zero, the width is used for the height
	  */
	void SetPmtDimension(const G4double &width, const G4double &height=-1);

	/** Set the length (Z) of the detector (in mm)
	  */
	void SetDetectorLength(G4double val){ fDetectorLength = val; }

	/** Set the width (X) of the detector (in mm)
	  */
	void SetDetectorWidth(G4double val){ fDetectorWidth = val; }   

	/** Set the thickness (Y) of the detector (in mm)
	  */
	void SetDetectorThickness(G4double val){ fDetectorThickness = val; }   

	/** Set the length of the trapezoidal light-guide (in mm)
	  */
	void SetTrapezoidLength(G4double val){ fTrapezoidLength = val; }

	/** Set the length of the light diffuser (in mm)
	  */
	void SetDiffuserLength(G4double val){ fDiffuserLength = val; }

	/** Set the thickness of the inner and outer reflective wrapping (in mm)
	  */
	void SetMylarThickness(G4double val){ fMylarThickness = val; }

	/** Set the thickness of optical grease layers (in mm)
	  */
	void SetGreaseThickness(G4double val){ fGreaseThickness = val; }

	/** Set the thickness of the PMT quartz windows (in mm)
	  */
	void SetWindowThickness(G4double val){ fWindowThickness = val; }

	/** Set the number of columns for segmented detectors
	  */
	void SetNumColumns(const G4int &val){ fNumColumns = val; }

	/** Set the number of rows for segmented detectors
	  */
	void SetNumRows(const G4int &val){ fNumRows = val; }

	/** Set the number of columns for segmented PMTs
	  */
	void SetNumPmtColumns(const G4int &val){ fNumColumnsPmt = val; }

	/** Set the number of rows for segmented PMTs
	  */	
	void SetNumPmtRows(const G4int &val){ fNumRowsPmt = val; }

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

	/** Set the position of the center of the detector in cartesian coordinates (all in mm)
	  */
	void SetPosition(const G4ThreeVector &position){ detectorPosition = position; }	

	/** Set the position of the center of the detector in cylindrical coordinates
	  *
	  * The three input vector coordinates are: r(cm) theta(deg) y(cm)
	  */
	void SetPositionCylindrical(const G4ThreeVector &position);
	
	/** Set the position of the center of the detector in spherical coordinates
	  *
	  * The three input vector coordinates are: r(cm) theta(deg) phi(deg)
	  */
	void SetPositionSpherical(const G4ThreeVector &position);

	/** Set the detector rotation angles about the X, Y, and Z axes (all in degrees)
	  */
	void SetRotation(const G4ThreeVector &rotation);

	/** Set the name of the detector scintillator material
	  * @note See nDetMaterials::getUserDetectorMaterial() for valid material names
	  */
	void SetDetectorMaterial(const G4String &material){ detectorMaterial = material; }

	/** Set the name of the optical reflector material
	  * @note See nDetMaterials::getUserWrappingMaterial() for valid material names
	  */
	void SetWrappingMaterial(const G4String &material){ wrappingMaterial = material; }

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
	
	/** Enable or disable a polished optical interface between the edge of the detector and the optical grease layer
	  */
	void SetPolishedInterface(const bool &state){ fPolishedInterface = state; }

	/** Enable or disable square PMTs
	  * @note If set to false, the PMTs will be circular
	  */
	void SetSquarePMTs(const bool &state){ fSquarePMTs = state; }

	/** Get the length (Z) of the detector (in mm)
	  */
	G4double GetDetectorLength() const { return fDetectorLength; }

	/** Get the width (X) of the detector (in mm)
	  */
	G4double GetDetectorWidth() const { return fDetectorWidth; }

	/** Get the thickness (Y) of the detector (in mm)
	  */
	G4double GetDetectorThickness() const { return fDetectorThickness; }

	/** Get a vector containing the width, thickness, and length of the detector (all in mm)
	  */
	G4ThreeVector GetDetectorSize() const { return G4ThreeVector(fDetectorWidth, fDetectorThickness, fDetectorLength); }

	/** Get the X position of the center of the detector (in mm)
	  */
	G4double GetDetectorPosX() const { return detectorPosition.getX(); }

	/** Get the Y position of the center of the detector (in mm)
	  */
	G4double GetDetectorPosY() const { return detectorPosition.getY(); }
	
	/** Get the Z position of the center of the detector (in mm)
	  */
	G4double GetDetectorPosZ() const { return detectorPosition.getZ(); }

	/** Get a vector containing the position of the center of the detector (all in mm)
	  */	
	G4ThreeVector GetDetectorPos() const { return detectorPosition; }

	/** Get a copy of the rotation matrix used for the detector
	  */
	G4RotationMatrix GetDetectorRot() const { return detectorRotation; }

	/** Get the number of segmented PMT columns
	  */
	G4int GetNumPmtColumns() const { return fNumColumnsPmt; }

	/** Get the number of segmented PMT rows
	  */
	G4int GetNumPmtRows() const { return fNumRowsPmt; }

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
	std::vector<userAddDetector> GetUserDetectors() const { return userDetectors; }

	/** Get a pointer to the logical parent volume of the current detector
	  */
	G4LogicalVolume *getCurrentAssembly(){ return currentAssembly; }
	
	/** Get a pointer to the current detector
	  */
	userAddDetector *getCurrentDetector(){ return currentDetector; }

	/** Return true if the PMT is segmented and return false otherwise
	  */
	bool PmtIsSegmented() const { return (fNumColumnsPmt > 0 && fNumRowsPmt > 0); }

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
	  * | geom       | Name of detector geometry (see addGeometry() for valid types)
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

	G4double pmtWidth; ///< The width (x-axis) of the PMT (in mm)
	G4double pmtHeight; ///< The height (y-axis) of the PMT (in mm)
	G4double fMylarThickness; ///< Thickness of the inner and outer detector wrapping (in mm)
	G4double fGreaseThickness; ///< Thickness of all optical grease layers (in mm)
	G4double fWindowThickness; ///< Thickness of all optical window layers (in mm)
	G4double fSensitiveThickness; ///< Thickness of the optical photon-sensitive surfaces (in mm)
	G4double fDetectorLength; ///< Size of the detector along the z-axis (in mm)
	G4double fDetectorThickness; ///< Size of the detector along the y-axis (in mm)
	G4double fDetectorWidth; ///< Size of the detector along the x-axis (in mm)
	G4double fTrapezoidLength; ///< Thickness of the trapezoids used as light guides (in mm)
	G4double fDiffuserLength; ///< Thickness of straight diffusers (in mm)

	G4double currentLayerSizeX; ///< Size of the current user add layer along the x-axis (in mm)
	G4double currentLayerSizeY; ///< Size of the current user add layer along the y-axis (in mm)
	G4double currentOffsetZ; ///< Offset of the current user add layer along the z-axis (in mm)

	G4int fNumColumns; ///< Current number of scintillator columns (x-axis) for modular detectors
	G4int fNumRows; ///< Current number of scintillator rows (y-axis) for modular detectors
	G4int fNumColumnsPmt; ///< Current number of PMT anode columns (x-axis) for PSPmts
	G4int fNumRowsPmt; ///< Current number of PMT anode rows (y-axis) for PSPmts
	G4int scintCopyNum; ///< Current scintillator segment copy number

	G4bool fPolishedInterface; /*!< Flag indicating that a polished interface should be used between scintillators and optical grease
	                                otherwise a Lambertian surface will be used for the optical interface */
	G4bool fCheckOverlaps; ///< Flag indicating that Geant should check for overlaps between all placed objects
	G4bool fSquarePMTs; ///< Flag indicating that the added PMTs are square. If set to false, the PMTs will be circular, and the parameter @a pmtWidth will be used as the diameter

	G4LogicalVolume* expHall_logV; ///< Logical volume of the world
	G4VPhysicalVolume* expHall_physV; ///< Physical volume of the world

	G4VisAttributes *assembly_VisAtt; ///< Visual attributes for the mother assembly
	G4VisAttributes *sensitive_VisAtt; ///< Visual attributes for the photo-sensitive surface
	G4VisAttributes *window_VisAtt; ///< Visual attributes for the optical window
	G4VisAttributes *grease_VisAtt; ///< Visual attributes for the optical grease
	G4VisAttributes *wrapping_VisAtt; ///< Visual attributes for the inner/outer detector wrappint
	G4VisAttributes *scint_VisAtt; ///< Visual attributes for the scintillator material
	G4VisAttributes *shadow_VisAtt; ///< Visual attributes for the shadow object

	std::vector<gdmlSolid> solids; ///< Vector of all loaded gdml solids

	G4String detectorMaterial; ///< String indicating the material to use for the detector scintillator body
	G4String wrappingMaterial; ///< String indicating the material to use for the inner and outer detector wrapping

	G4ThreeVector detectorPosition; ///< Position of detector in the lab frame
	G4RotationMatrix detectorRotation; ///< Rotation of detector in the lab frame

	G4ThreeVector shadowBarSize; ///< Size of the shadow object in 3d space
	G4ThreeVector shadowBarPos; ///< Position of the shadow object in 3d space
	G4Material *shadowBarMaterial; ///< Pointer to the material used for the shadow object

	std::vector<G4PVPlacement*> scintBody_physV; ///< Vector of all physical volumes of all scintillator segments

	std::vector<userAddDetector> userDetectors; ///< Vector of all detectors added by the user
	
	userAddDetector *currentDetector; ///< Pointer to the current detector added by the user
	G4LogicalVolume *currentAssembly; ///< Pointer to the mother logical volume of the current detector

	centerOfMass center[2]; ///< Objects used to compute the detected optical photon center-of-mass position for the left and right PMT

	nDetMaterials materials; ///< Object containing all Geant materials and elements which will be used in detector construction

	/** Default constructor. Private for singleton class
	  */
	nDetConstruction();

	/** Build the experiment hall physical and logical volumes
	  */
	void buildExpHall();

	/** Build a segmented detector module
	  *
	  * The detector will have @a fNumColumns columns (horizontal) and @a fNumRows rows (vertical). If @a fMylarThickness is
	  * greater than zero, the user selected wrapping material will be applied between all segments and around the outside
	  * of the detector.
	  */
	void buildModule();

	/** Build a single-segment elliptical detector (football)
	  * 
	  * The parameter @a fDetectorLength is used as the length of the central rectangular body while @a fTrapezoidLength is
	  * used for the length of the trapezoids on either side. The total detector length is equal to 2*fTrapezoidLength+fDetectorLength.
	  * The internal trapezoid angle is 60 degrees (hard-coded).
	  */	
	void buildEllipse();

	/** Build a single-segment rectangular detector
	  */
	void buildRectangle();
	
	/** Build a single-segment cylindrical detectector
	  *
	  * The parameter @a fDetectorWidth is used as the diameter of the cylindrical body, @a fDetectorheight is not used
	  */
	void buildCylinder();
	
	/** Build test assembly
	  * @note Not currently implemented
	  */
	void buildTestAssembly();
	
	/** Build the assembly volume for the current detector
	  * @param boundingBox The returned X, Y, and Z size of the assembly volume for the current detector
	  * @return A pointer to the logical volume of the assembly of the current detector
	  */
	G4LogicalVolume *constructAssembly(G4ThreeVector &boundingBox);

	/** Build PMTs for the current detector and place them in the assembly volume
	  * 
	  * Each PMT consists of an optical grease layer, a quartz window, and a photo-sensitive surface. Additionally,
	  * a reflective wrapping layer is added around the outside of the PMT if @a fMylarThickness is greater than zero.
	  */
	void constructPSPmts();

	/** Load a model from a GDML file using parameters from a space-delimited input string and place it into the assembly
	  * @note See loadGDML(const G4String &input) for input string syntax
	  */
	void applyGDML(const G4String &input);

	/** Load a light-guide model from a GDML file using parameters from a space-delimited input string and place it into the assembly
	  * @note See loadLightGuide(const G4String &input) for input string syntax
	  */
	void applyGDMLlightGuide(const G4String &input);

	/** Apply a grease layer to the current detector assembly using the current detector width and height
	  */
	void applyGreaseLayer();

	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width> <height> [thickness]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width     | The width of the grease layer (in mm)
	  * | height    | The height of the grease layer (in mm)
	  * | thickness | The thickness of the grease layer (in mm). If not specified, @a fGreaseThickness is used
	  */
	void applyGreaseLayer(const G4String &input);

	/** Apply a grease layer to the current detector assembly
	  * @param x The width of the grease layer (in mm)
	  * @param y The height of the grease layer (in mm)
	  * @param thickness The thickness of the grease layer (in mm). If not specified, @a fGreaseThickness is used
	  */
	void applyGreaseLayer(const G4double &x, const G4double &y, double thickness=0);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using the current detector width and height
	  */
	void applyDiffuserLayer();

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width> <height> <thickness> [material=G4_SILICON_DIOXIDE]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width     | The width of the diffuser layer (in mm)
	  * | height    | The height of the diffuser layer (in mm)
	  * | thickness | The thickness of the diffuser layer (in mm)
	  * | material  | Not used
	  */
	void applyDiffuserLayer(const G4String &input);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly
	  * @param x The width of the diffuser layer (in mm)
	  * @param y The height of the diffuser layer (in mm)
	  * @param thickness The thickness of the diffuser layer (in mm)
	  */
	void applyDiffuserLayer(const G4double &x, const G4double &y, const double &thickness);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly reducing the current detector width and height
	  * down to the width and height of the PMT
	  */
	void applyLightGuide();

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width1> <width2> <height1> <height2> <thickness> [material=G4_SILICON_DIOXIDE]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width1    | The width of the large side of the trapezoid (in mm)
	  * | width2    | The width of the small side of the trapezoid (in mm)
	  * | height1   | The height of the large side of the trapezoid (in mm)
	  * | height2   | The height of the small side of the trapezoid (in mm)
	  * | thickness | The thickness of the light-guide layer (in mm)
	  * | material  | Not used
	  */
	void applyLightGuide(const G4String &input);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly reducing the current detector width and height
	  * down to a user specified width and height
	  * @param x2 The width of the small side of the trapezoid (in mm)
	  * @param y2 The height of the amll side of the trapezoid (in mm)
	  */
	void applyLightGuide(const G4double &x2, const G4double &y2);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly
	  * @param x1 The width of the large side of the trapezoid (in mm)
	  * @param x2 The width of the small side of the trapezoid (in mm)
	  * @param y1 The height of the large side of the trapezoid (in mm)
	  * @param y2 The height of the amll side of the trapezoid (in mm)
	  * @param thickness The thickness of the light guide (in mm)
	  */
	void applyLightGuide(const G4double &x1, const G4double &x2, const G4double &y1, const G4double &y2, const double &thickness);

	/** Load a GDML model from a file using parameters from a space-delimited input string and place it into the assembly
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

	/** Load a GDML model from a file and place it into the assembly
	  * @param fname Filename of the input GDML file
	  * @param position Vector containing the position of the model (in mm)
	  * @param rotation Vector containing the rotation about the X, Y, and Z axes (in degrees)
	  * @param material String containing the name of the Geant material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	gdmlSolid *loadGDML(const G4String &fname, const G4ThreeVector &position, const G4ThreeVector &rotation, const G4String &material);

	/** Load a light guide model from a file using parameters from a space-delimited input string and place it into the assembly
	  *
	  * This method performs the same as loadGDML() except that it also defines logical border surfaces on all
	  * intersecting faces and automatically places the model into the assembly at the current layer position
	  *
	  * @note String syntax: <filename> <rotX> <rotY> <rotZ> <matString>
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | filename  | Filename of the input GDML file
	  * | rotX(Y,Z) | Rotation about the X, Y, and Z axes (in degrees)
	  * | matString | The NIST database name of the material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	gdmlSolid *loadLightGuide(const G4String &input);

	/** Load a GDML light guide model from a file and place it into the assembly
	  * 
	  * This method performs the same as loadGDML() except that it also defines logical border surfaces on all intersecting faces
	  *
	  * @param fname Filename of the input GDML file
	  * @param rotation Vector containing the rotation about the X, Y, and Z axes (in degrees)
	  * @param material String containing the name of the Geant material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	gdmlSolid *loadLightGuide(const G4String &fname, const G4ThreeVector &rotation, const G4String &material, G4OpticalSurface *surface);

	/** Get a vector containing the X, Y, and Z size of the PMT bounding box
	  */	
	G4ThreeVector getPSPmtBoundingBox();
	
	/** Get a pointer to the user-defined scintillator material
	  */
	G4Material* getUserDetectorMaterial();

	/** Get a pointer to the user-defined surface material
	  */
	G4Material* getUserSurfaceMaterial();

	/** Get a pointer to the user-defined optical surface
	  */
	G4OpticalSurface* getUserOpticalSurface();
};

/** @class userAddLayer
  * @brief Component layers which are added to detector assemblies
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 10, 2019
  */

class userAddLayer{
  public:
	/** Default constructor
	  */
	userAddLayer() : argStr(), ptr(NULL) { }

	/** Function pointer constructor
	  * @param arg_ Argument string from the input macro
	  * @param ptr_ Pointer to a nDetConstruction member function taking a const G4String as an argument.
	  *             This is the function which will be executed when execute() is called
	  */	
	userAddLayer(const G4String &arg_, void (nDetConstruction::* ptr_)(const G4String &)) : argStr(arg_), ptr(ptr_) { }

	/** Execute the function pointer
	  * @param obj Pointer to an nDetConstruction object which is used to call the function pointer
	  */
	void execute(nDetConstruction *obj){ (obj->*ptr)(argStr); }

	/** Return the argument string from the input macro
	  */
	G4String dump() const { return argStr; }

  private:
	G4String argStr; ///< Argument string from the macro supplied by the user

	void (nDetConstruction::* ptr)(const G4String &); ///< Pointer to a nDetConstruction member function taking a const G4String as an argument
};

/** @class userAddDetector
  * @brief An individual detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 10, 2019
  */

class userAddDetector{
  public:
	/** Default constructor
	  */
	userAddDetector() : assembly_logV(NULL), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0), parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), numColumns(1), numRows(1) { }
	
	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  */
	userAddDetector(nDetConstruction *detector);
	
	/** Get a pointer to the logical volume of the detector assembly
	  */
	G4LogicalVolume *getLogicalVolume(){ return assembly_logV; }

	/** Get the width and height of the current layer as well as the current offset along the detector length
	  * @param x_ Width of the current layer (in mm)
	  * @param y_ Height of the current layer (in mm)
	  * @param z_ Offset along the detector length (in mm)
	  */
	void getCurrentOffset(G4double &x_, G4double &y_, G4double &z_);
	
	/** Get the copy number of this detector (i.e. the detector ID)
	  */
	G4int getParentCopyNumber() const { return parentCopyNum; }
	
	/** Get the copy number of the left PMT
	  */
	G4int getLeftPmtCopyNumber() const { return 2*parentCopyNum; }
	
	/** Get the copy number of the right PMT
	  */
	G4int getRightPmtCopyNumber() const { return 2*parentCopyNum+1; }
	
	/** Get a pointer to the vector representing the central position of this detector
	  */
	G4ThreeVector *getPosition(){ return &position; }

	/** Get a pointer to the rotation matrix used for this detector
	  */	
	G4RotationMatrix *getRotation(){ return &rotation; }
	
	/** Set the position and the rotation of this detector
	  * @param pos The position of the center of the detector (in mm)
	  * @param rot The rotation matrix
	  */
	void setPositionAndRotation(const G4ThreeVector &pos, const G4RotationMatrix &rot);

	/** Set the width and height of the current layer as well as the current offset along the detector length
	  * @param x_ Width of the current layer (in mm)
	  * @param y_ Height of the current layer (in mm)
	  * @param z_ Offset along the detector length (in mm)
	  */
	void setCurrentOffset(const G4double &x_, const G4double &y_, const G4double &z_);

	/** Current offset along the detector length (Z-axis, in mm)
	  */
	void setCurrentOffsetZ(const G4double &z_){ offsetZ = z_; }

	/** Set the copy number of this detector (i.e. the detector ID)
	  */
	void setParentCopyNumber(const G4int &num){ parentCopyNum = num; }

	/** Setup a segmented detector
	  * @param col_ Number of columns (along the width)
	  * @param row_ Number of rows (along the height)
	  * @param startCopyNum The copy number of the first segment of this detector
	  */
	void setSegmentedDetector(const G4int &col, const G4int &row, const G4int &startCopyNum=0);

	/** Set or unset this detector as a start signal for timing
	  */
	void setAsStart(const bool &enabled){ isStart = enabled; }

	/** Add a user defined layer to the queue
	  * @note When buildAllLayers() is called, the layers will be added on a first-in-first-out basis
	  */
	void addLayer(const userAddLayer &layer){ userLayers.push_back(layer); }

	/** Add all layers added by addLayer() to the detector construction
	  */
	void buildAllLayers(nDetConstruction *detector);
	
	/** Place this detector assembly into a parent logical volume
	  * @param Pointer to the logical volume of the parent assembly
	  */
	void placeDetector(G4LogicalVolume *parent);

	/** Clear both the left and right photon center-of-mass calculators
	  */
	void clear();

	/** Copy the center-of-mass calculator for the left and right PMTs
	  * @param left Left PMT center-of-mass calculator
	  * @param right Right PMT center-of-mass calculator
	  */	
	void copyCenterOfMass(const centerOfMass &left, const centerOfMass &right);

	/** Check if a scintillator copy number is part of this detector
	  * @param num The copy number of a scintillator segment
	  * @return True if the copy number is part of this detector and return false otherwise
	  */
	bool checkCopyNumber(const G4int &num) const { return (num >= firstSegmentCopyNum && num < lastSegmentCopyNum); }

	/** Check if a PMT copy number is part of this detector
	  * @param num The copy number of a scintillator segment
	  * @return True if the copy number is part of this detector and return false otherwise
	  */
	bool checkPmtCopyNumber(const G4int &num) const { return (num == 2*parentCopyNum || num == 2*parentCopyNum+1); }

	/** Check if a PMT copy number is part of this detector
	  * @param num The copy number of a scintillator segment
	  * @param isLeft Returned flag indicating that the copy number refers to the left PMT
	  * @return True if the copy number is part of this detector and return false otherwise
	  */	
	bool checkPmtCopyNumber(const G4int &num, bool &isLeft) const ;

	/** Get the column and row corresponding to a segment copy number in a segmented detector
	  * @param copyNum The copy number of a scintillator segment
	  * @param col The segmented detector column corresponding to the copy number
	  * @param row The segmented detector row corresponding to the copy number
	  * @return True if the copy number is found to be part of a detector and return false otherwise
	  */	
	bool getSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const ;

	/** Retrun true if both the left and right photon center-of-mass calculators are empty and return false otherwise
	  */
	bool empty() const { return (cmL.empty() && cmR.empty()); }

	/** Return true if this is a start detector and return false otherwise
	  */
	bool getIsStart() const { return isStart; }

	/** Return a pointer to the optical photon center-of-mass calculator for the left PMT
	  */
	centerOfMass *getCenterOfMassL(){ return &cmL; }

	/** Return a pointer to the optical photon center-of-mass calculator for the right PMT
	  */	
	centerOfMass *getCenterOfMassR(){ return &cmR; }

	/** Return a pointer to the PMT response for the left PMT
	  */
	pmtResponse *getPmtResponseL(){ return cmL.getPmtResponse(); }

	/** Return a pointer to the PMT response for the right PMT
	  */
	pmtResponse *getPmtResponseR(){ return cmR.getPmtResponse(); }

  private:
	G4LogicalVolume *assembly_logV; ///< Pointer to the logical volume of the mother of the detector
	G4VPhysicalVolume* assembly_physV; ///< Pointer to the physical volume of the mother of the detector

	G4double layerSizeX; ///< Width of the current user add layer (in mm)
	G4double layerSizeY; ///< Height of the current user add layer (in mm)
	G4double offsetZ; ///< Offset of the current user add layer along the z-axis (in mm)
	
	G4int parentCopyNum; ///< Copy number of the mother of the detector
	G4int firstSegmentCopyNum; ///< Copy number of the first scintillator segment
	G4int lastSegmentCopyNum; ///< Copy number of the last scintillator segment

	G4int numColumns; ///< Number of scintillator segment columns
	G4int numRows; ///< Number of scintillator segment rows

	bool isStart; ///< Flag indicating that this detector is used as a start signal for timing

	G4ThreeVector position; ///< Position of the detector in the lab frame
	G4RotationMatrix rotation; ///< Rotation of the detector in the lab frame
	
	centerOfMass cmL; ///< Center-of-mass calculator for the left PMT
	centerOfMass cmR; ///< Center-of-mass calculator for the right PMT
	
	std::vector<userAddLayer> userLayers; ///< Vector of all layers added by the user
};

#endif
