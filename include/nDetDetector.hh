#ifndef NDET_DETECTOR_HH
#define NDET_DETECTOR_HH

#include "globals.hh"
#include "G4RotationMatrix.hh"

#include "gdmlSolid.hh"
#include "centerOfMass.hh"

class G4LogicalVolume;
class G4PVPlacement;
class G4OpticalSurface;
class G4Material;

class nDetMaterials;
class nDetConstruction;
class nDetDetectorMessenger;
class userAddLayer;
class gdmlSolid;

enum GEOMTYPES {GEOM_MODULE, GEOM_ELLIPSE, GEOM_RECTANGLE, GEOM_CYLINDER, GEOM_TEST};

/** @class nDetDetectorParams
  * @brief Physical attributes of a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 16, 2019
  */
  
class nDetDetectorParams{
  public:
	G4double pmtWidth; ///< The width (x-axis) of the PMT (in mm)
	G4double pmtHeight; ///< The height (y-axis) of the PMT (in mm)
	G4double fWrappingThickness; ///< Thickness of the inner and outer detector wrapping (in mm)
	G4double fGreaseThickness; ///< Thickness of all optical grease layers (in mm)
	G4double fWindowThickness; ///< Thickness of all optical window layers (in mm)
	G4double fSensitiveThickness; ///< Thickness of the optical photon-sensitive surfaces (in mm)
	G4double fDetectorLength; ///< Size of the detector along the z-axis (in mm)
	G4double fDetectorHeight; ///< Size of the detector along the y-axis (in mm)
	G4double fDetectorWidth; ///< Size of the detector along the x-axis (in mm)
	G4double fTrapezoidLength; ///< Thickness of the trapezoids used as light guides (in mm)
	G4double fDiffuserLength; ///< Thickness of straight diffusers (in mm)
	
	G4int fNumColumns; ///< Current number of scintillator columns (x-axis) for modular detectors
	G4int fNumRows; ///< Current number of scintillator rows (y-axis) for modular detectors
	G4int fNumColumnsPmt; ///< Current number of PMT anode columns (x-axis) for PSPmts
	G4int fNumRowsPmt; ///< Current number of PMT anode rows (y-axis) for PSPmts
	G4int scintCopyNum; ///< Current scintillator segment copy number

	G4bool fPolishedInterface; /*!< Flag indicating that a polished interface should be used between scintillators and optical grease
	                                otherwise a Lambertian surface will be used for the optical interface */
	G4bool fSquarePMTs; ///< Flag indicating that the added PMTs are square. If set to false, the PMTs will be circular, and the parameter @a pmtWidth will be used as the diameter
	G4bool isStart; ///< Flag indicating that this detector is used as a start signal for timing
	
	G4String detectorMaterial; ///< String indicating the material to use for the detector scintillator body
	G4String wrappingMaterial; ///< String indicating the material to use for the inner and outer detector wrapping

	G4ThreeVector detectorPosition; ///< Position of detector in the lab frame
	G4RotationMatrix detectorRotation; ///< Rotation of detector in the lab frame
	
	nDetDetectorMessenger *fMessenger; ///< Geant messenger to use for this class
	
	nDetDetectorParams() : pmtWidth(30), pmtHeight(30), fWrappingThickness(0), fGreaseThickness(0.1), fWindowThickness(0.1), fSensitiveThickness(1), 
	                   fDetectorLength(600), fDetectorHeight(30), fDetectorWidth(30), fTrapezoidLength(0), fDiffuserLength(0),
	                   fNumColumns(1), fNumRows(1), fNumColumnsPmt(-1), fNumRowsPmt(-1), scintCopyNum(1), 
	                   fPolishedInterface(true), fSquarePMTs(true), isStart(false), 
	                   detectorMaterial("ej200"), wrappingMaterial("mylar"),
	                   fMessenger(NULL) { }
	                   
	/** Initialize the detector parameter messenger for this class
	  * @note Since there may be multiple detectors in any given setup, the nDetDetectorMessenger class
	  *       is not initialized by default and must be enabled manually using this method
	  */
	void InitializeMessenger();

	/** Set the length (Z) of the detector (in mm)
	  */
	void SetDetectorLength(G4double val){ fDetectorLength = val; }

	/** Set the width (X) of the detector (in mm)
	  */
	void SetDetectorWidth(G4double val){ fDetectorWidth = val; }   

	/** Set the height (Y) of the detector (in mm)
	  */
	void SetDetectorHeight(G4double val){ fDetectorHeight = val; }   

	/** Set the length of the trapezoidal light-guide (in mm)
	  */
	void SetTrapezoidLength(G4double val){ fTrapezoidLength = val; }

	/** Set the length of the light diffuser (in mm)
	  */
	void SetDiffuserLength(G4double val){ fDiffuserLength = val; }

	/** Set the thickness of the inner and outer reflective wrapping (in mm)
	  */
	void SetMylarThickness(G4double val){ fWrappingThickness = val; }

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
	
	/** Enable or disable a polished optical interface between the edge of the detector and the optical grease layer
	  */
	void SetPolishedInterface(const bool &state){ fPolishedInterface = state; }

	/** Enable or disable square PMTs
	  * @note If set to false, the PMTs will be circular
	  */
	void SetSquarePMTs(const bool &state){ fSquarePMTs = state; }

	/** Set the width and height of the PMT optical surface
	  * @param width The width (x-dimension) in mm
	  * @param height The height (y-dimension) in mm. If less than or equal to zero, the width is used for the height
	  */
	void SetPmtDimension(const G4double &width, const G4double &height=-1);

	/** Set the width and height of the PMT optical surface
	  * @param input Space-delimited string with the format "<width(mm)> [height(mm)]"
	  *              If the height is not specified, the height will be equal to the width
	  */
	void SetPmtDimension(const G4String &input);

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

	/** Set or unset this detector as a start signal for timing
	  */
	void SetAsStart(const bool &enabled){ isStart = enabled; }

	/** Get the length (Z) of the detector (in mm)
	  */
	G4double GetDetectorLength() const { return fDetectorLength; }

	/** Get the width (X) of the detector (in mm)
	  */
	G4double GetDetectorWidth() const { return fDetectorWidth; }

	/** Get the height (Y) of the detector (in mm)
	  */
	G4double GetDetectorHeight() const { return fDetectorHeight; }

	/** Get a vector containing the width, height, and length of the detector (all in mm)
	  */
	G4ThreeVector GetDetectorSize() const { return G4ThreeVector(fDetectorWidth, fDetectorHeight, fDetectorLength); }

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

	/** Get the uniform width of scintillator segments for segmented detectors
	  * @note Assumes segments of uniform width, each with a layer of wrapping between them (if enabled). So the
	  *       total detector height, @a fDetectorWidth, is equal to fNumColumns*segmentWidth+(fNumColumns-1)*fWrappingThickness
	  */
	G4double GetSegmentWidth() const { return (fDetectorWidth-(fNumColumns-1)*fWrappingThickness)/fNumColumns; }

	/** Get the uniform height of scintillator segments for segmented detectors
	  * @note Assumes segments of uniform height, each with a layer of wrapping between them (if enabled). So the
	  *       total detector height, @a fDetectorHeight, is equal to fNumRows*segmentHeight+(fNumRows-1)*fWrappingThickness
	  */	
	G4double GetSegmentHeight() const { return (fDetectorHeight-(fNumRows-1)*fWrappingThickness)/fNumRows; }

	/** Return true if this is a start detector and return false otherwise
	  */
	bool GetIsStart() const { return isStart; }

	/** Return true if the PMT is segmented and return false otherwise
	  */
	bool PmtIsSegmented() const { return (fNumColumnsPmt > 0 && fNumRowsPmt > 0); }
	
	/** Return true if the wrapping thickness is positive and non-zero and return false otherwise
	  */
	bool WrappingEnabled() const { return (fWrappingThickness > 0); }
};

/** @class nDetDetector
  * @brief An individual detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 10, 2019
  */

class nDetDetector : public nDetDetectorParams {
  public:
	/** Default constructor
	  */
	nDetDetector() : nDetDetectorParams(),
	                 assembly_logV(NULL), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0),
	                 parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0),
	                 checkOverlaps(false), geomType(GEOM_RECTANGLE), materials(NULL) { }
	
	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  * @param matptr Pointer to the Geant materials handler class which will be used for detector construction
	  */
	nDetDetector(nDetConstruction *detector, nDetMaterials *matptr);

	/** Destructor
	  */
	~nDetDetector();
	
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
	G4ThreeVector *getPosition(){ return &detectorPosition; }

	/** Get a pointer to the rotation matrix used for this detector
	  */	
	G4RotationMatrix *getRotation(){ return &detectorRotation; }
	
	/** Enable or disable checking overlaps with existing geometry when placing objects
	  */
	void setCheckOverlaps(const bool &enabled){ checkOverlaps = enabled; }
	
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

	/** Set the detector geometry 
	  * @note Currently supported geometry names are @a next, @a module, @a ellipse, @a rectangle, @a cylinder, and @a test
	  * @return True if the specified type is recognized and return false otherwise
	  */
	bool setGeometry(const G4String &geom);

	/** Build the detector body, add component layers, and attach photo-sensitive surfaces
	  */
	void construct();

	/** Add a user defined layer to the queue
	  * @note When buildAllLayers() is called, the layers will be added on a first-in-first-out basis
	  */
	void addLayer(userAddLayer *layer){ userLayers.push_back(layer); }

	/** Add all layers added by addLayer() to the detector construction
	  */
	void buildAllLayers();
	
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

	/** Load a GDML model from a file using parameters from a space-delimited input string and place it into the current detector assembly
	  * @note See loadGDML(const G4String &) for input string syntax
	  */
	void addGDML(const G4String &input);

	/** Load a light guide model from a file using parameters from a space-delimited input string and place it into the current detector assembly
	  * @note See loadLightGuide(const G4String &) for input string syntax
	  */
	void addLightGuideGDML(const G4String &input);

	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note See applyGreaseLayer(const G4String &) for input string syntax
	  */
	void addGreaseLayer(const G4String &input);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See applyDiffuserLayer(const G4String &) for input string syntax
	  */
	void addDiffuserLayer(const G4String &input);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See applyLightGuide(const G4String &) for input string syntax
	  */
	void addLightGuideLayer(const G4String &input);

	/** Build the assembly volume for the current detector
	  * @param boundingBox The returned X, Y, and Z size of the assembly volume for the current detector
	  * @return A pointer to the logical volume of the assembly of the current detector
	  */
	G4LogicalVolume *constructAssembly();

	/** Add a daughter to the logical volume of the detector body
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param pos The X, Y, and Z position offset of the daughter within the detector body (all in mm)
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  * @return A pointer to the physical volume of the daughter
	  */
	G4PVPlacement *addToDetectorBody(G4LogicalVolume *volume, const G4String &name="", const G4ThreeVector &pos=G4ThreeVector(0,0,0), G4RotationMatrix *rot=NULL);

	/** Add a particle-sensitive daughter to the logical volume of the detector body
	  * @note The copy number of the new physical volume will be automatically set by this method
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param pos The X, Y, and Z position offset of the daughter within the detector body (all in mm)
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  * @return A pointer to the physical volume of the daughter
	  */
	G4PVPlacement *addSegmentToBody(G4LogicalVolume *volume, const G4String &name="", const G4ThreeVector &pos=G4ThreeVector(0,0,0), G4RotationMatrix *rot=NULL);

	/** 
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  * @return A pointer to the physical volume of the daughter
	  */
	G4PVPlacement *addLeftComponent(G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** 
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  * @return A pointer to the physical volume of the daughter
	  */
	G4PVPlacement *addRightComponent(G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** 
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  */
	void addMirroredComponents(G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** 
	  * @param phys1 Returned pointer to the physical volume of the left component
	  * @param phys2 Returned pointer to the physical volume of the right component
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  */
	void addMirroredComponents(G4PVPlacement* &phys1, G4PVPlacement* &phys2, G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** Apply a grease layer to the current detector assembly using the current detector width and height
	  */
	void applyGreaseLayer();

	/** Apply a grease layer to the current detector assembly
	  * @param x The width of the grease layer (in mm)
	  * @param y The height of the grease layer (in mm)
	  * @param thickness The thickness of the grease layer (in mm). If not specified, @a fGreaseThickness is used
	  */
	void applyGreaseLayer(const G4double &x, const G4double &y, double thickness=0);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using the current detector width and height
	  */
	void applyDiffuserLayer();

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

	/** Load a GDML model from a file and place it into the assembly
	  * @param solid Pointer to a gdml model loaded from a file
	  */
	void loadGDML(gdmlSolid *solid);

	/** Load a GDML light guide model from a file and place it into the assembly
	  * 
	  * This method performs the same as loadGDML() except that it also defines logical border surfaces on all intersecting faces
	  *
	  * @param solid Pointer to a gdml model loaded from a file
	  * @param rotation Rotation of the gdml model about the X, Y, and Z axes (all in radians)
	  */
	void loadLightGuide(gdmlSolid *solid, const G4ThreeVector &rotation);

  private:
	G4LogicalVolume *assembly_logV; ///< Pointer to the logical volume of the mother of the detector
	G4PVPlacement* assembly_physV; ///< Pointer to the physical volume of the mother of the detector

	G4double layerSizeX; ///< Width of the current user add layer (in mm)
	G4double layerSizeY; ///< Height of the current user add layer (in mm)
	G4double offsetZ; ///< Offset of the current user add layer along the z-axis (in mm)
	
	G4int parentCopyNum; ///< Copy number of the mother of the detector
	G4int firstSegmentCopyNum; ///< Copy number of the first scintillator segment
	G4int lastSegmentCopyNum; ///< Copy number of the last scintillator segment

	bool checkOverlaps; ///< Flag indicating that Geant should check for overlaps between all placed objects

	int geomType; ///< Integer value indicating the of the detector geometry
	
	nDetMaterials *materials; ///< Pointer to the NEXTSim Geant materials container

	centerOfMass cmL; ///< Center-of-mass calculator for the left PMT
	centerOfMass cmR; ///< Center-of-mass calculator for the right PMT

	std::vector<userAddLayer*> userLayers; ///< Vector of all layers added by the user

	std::vector<G4PVPlacement*> scintBody_physV; ///< Vector of all physical volumes of all scintillator segments

	/** Build a segmented detector module
	  *
	  * The detector will have @a fNumColumns columns (horizontal) and @a fNumRows rows (vertical). If @a fWrappingThickness is
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
	
	/** Build PMTs for the current detector and place them in the assembly volume
	  * 
	  * Each PMT consists of an optical grease layer, a quartz window, and a photo-sensitive surface. Additionally,
	  * a reflective wrapping layer is added around the outside of the PMT if @a fWrappingThickness is greater than zero.
	  */
	void constructPSPmts();

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
	userAddLayer() : argStr(), size(), nReqArgs(0) { }

	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	userAddLayer(const G4String &arg_) : argStr(arg_), size(), nReqArgs(0) { }

	/** Function pointer constructor
	  * @param arg_ Argument string from the input macro
	  * @param ptr_ Pointer to a nDetDetector member function taking a const G4String as an argument.
	  *             This is the function which will be executed when execute() is called
	  */	
	userAddLayer(const G4String &arg_, const int &nargs_) : argStr(arg_), size(), nReqArgs(nargs_) { }

	/** Destructor
	  */
	virtual ~userAddLayer(){ }

	/** Get the size of the layer along the X-axis (in mm)
	  */
	G4double getSizeX() const { return size.getX(); }
	
	/** Get the size of the layer along the Y-axis (in mm)
	  */
	G4double getSizeY() const { return size.getY(); }

	/** Get the size of the layer along the Z-axis (in mm)
	  */
	G4double getSizeZ() const { return size.getZ(); }

	/** Return the argument string from the input macro
	  */
	G4String dump() const { return argStr; }

	/** Get the number of arguments expected from the user input string
	  */
	unsigned int getNumRequiredArgs() const { return nReqArgs; }

	/** Get the number of arguments passed to decodeString()
	  */
	unsigned int getNumSuppliedArgs() const { return nUserArgs; }

	/** Read a user input string and decode the relevant values
	  */
	virtual bool decodeString() = 0;
	
	/** Construct the layer volume and place it into a detector
	  */
	virtual void construct(nDetDetector *obj) = 0;

	/** Return a string containing proper input string syntax
	  */
	virtual std::string syntaxStr() const = 0;

  protected:
	G4String argStr; ///< Argument string from the macro supplied by the user

	G4ThreeVector size; ///< X, Y, and Z dimensions of the volume

	unsigned int nReqArgs; ///< The number of required user arguments which are expected
	unsigned int nUserArgs; ///< The number of user arguments passed to decodeString()
};

class greaseLayer : public userAddLayer {
  public:
	greaseLayer(const G4String &arg_) : userAddLayer(arg_, 2), x(0), y(0), thickness(0) { }

	/** Destructor
	  */	
	~greaseLayer(){ }
	
	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width> <height> [thickness]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width     | The width of the grease layer (in mm)
	  * | height    | The height of the grease layer (in mm)
	  * | thickness | The thickness of the grease layer (in mm). If not specified, @a fGreaseThickness is used
	  */
	bool decodeString();

	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4double x; ///< Width of the diffuser (in mm)
	G4double y; ///< Height of the diffuser (in mm)
	G4double thickness; ///< Thickness of the diffuser (in mm)	
};

class diffuserLayer : public userAddLayer {
  public:
	diffuserLayer(const G4String &arg_) : userAddLayer(arg_, 3), x(0), y(0), thickness(0), matName("G4_SILICON_DIOXIDE") { }

	/** Destructor
	  */	
	~diffuserLayer(){ }
	
	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width> <height> <thickness> [material=G4_SILICON_DIOXIDE]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width     | The width of the diffuser layer (in mm)
	  * | height    | The height of the diffuser layer (in mm)
	  * | thickness | The thickness of the diffuser layer (in mm)
	  * | material  | Not used
	  */
	bool decodeString();

	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4double x; ///< Width of the diffuser (in mm)
	G4double y; ///< Height of the diffuser (in mm)
	G4double thickness; ///< Thickness of the diffuser (in mm)
	G4String matName; ///< Name of the Geant NIST database material
};

class lightGuideLayer : public userAddLayer {
  public:
	lightGuideLayer(const G4String &arg_) : userAddLayer(arg_, 5), x1(0), x2(0), y1(0), y2(0), thickness(0), matName("G4_SILICON_DIOXIDE") { }

	/** Destructor
	  */
	lightGuideLayer(){ }

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
	bool decodeString();

	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4double x1; ///< Width of the large side of the trapezoid (in mm)
	G4double x2; ///< Width of the small side of the trapezoid (in mm)
	G4double y1; ///< Height of the large side of the trapezoid (in mm)
	G4double y2; ///< Height of the small side of the trapezoid (in mm)
	G4double thickness; ///< Thickness of the trapezoid (in mm)
	G4String matName; ///< Name of the Geant NIST database material
};

class gdmlLayer : public userAddLayer {
  public:
	gdmlLayer(const G4String &arg_) : userAddLayer(arg_, 8) { }

	/** Destructor
	  */	
	~gdmlLayer(){ }

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
	bool decodeString();

	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4String filename; ///< Path to the input GDML file
	G4String material; ///< NIST database name of the material to use for the model
	G4ThreeVector position; ///< X, Y, and Z position of the center of the model (all in mm)
	G4ThreeVector rotation; ///< Rotation about the X, Y, and Z axes (all in degrees)

	gdmlSolid solid; ///< Solid model loaded from an external gdml file
};

class gdmlLightGuideLayer : public userAddLayer {
  public:
	gdmlLightGuideLayer(const G4String &arg_) : userAddLayer(arg_, 5) { }

	/** Destructor
	  */	
	~gdmlLightGuideLayer(){ }

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
	bool decodeString();

	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;
	
  private:
	G4String filename; ///< Path to the input GDML file
	G4String material; ///< NIST database name of the material to use for the model
	G4ThreeVector rotation; ///< Rotation about the X, Y, and Z axes (all in degrees)
  
	gdmlSolid solid; ///< Solid model loaded from an external gdml file
};

#endif
