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
class G4CSGSolid;

class nDetMaterials;
class nDetConstruction;
class nDetDetectorMessenger;
class nDetWorldObject;
class gdmlSolid;

/** @class nDetDetectorParams
  * @brief Physical attributes of a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 16, 2019
  */
  
class nDetDetectorParams{
public:
	/** Default constructor
	  */
	nDetDetectorParams() : pmtWidth(30), pmtHeight(30), fWrappingThickness(0), fGreaseThickness(0.1), fWindowThickness(0.1), fSensitiveThickness(1), 
	                   fDetectorLength(600), fDetectorHeight(30), fDetectorWidth(30), fTrapezoidLength(0), fTrapezoidAngle(60), fDiffuserLength(0), fSegmentWidth(0), fSegmentHeight(0),
	                   fNumColumns(1), fNumRows(1), fNumColumnsPmt(-1), fNumRowsPmt(-1), scintCopyNum(1), 
	                   fPolishedInterface(true), fSquarePMTs(true), isStart(false), 
	                   detectorMaterialName("ej200"), wrappingMaterialName("mylar"),
	                   constantWidth(true), constantHeight(true),
	                   geomType("undefined"),
	                   maxBodySize(30, 30, 600),
	                   scintMaterial(NULL), wrappingMaterial(NULL), wrappingOpSurf(NULL),
	                   scintVisAtt(NULL), wrappingVisAtt(NULL),
	                   materials(NULL), fMessenger(NULL) { }
	                   
	/** Initialize the detector parameter messenger for this class
	  * @note Since there may be multiple detectors in any given setup, the nDetDetectorMessenger class
	  *       is not initialized by default and must be enabled manually using this method
	  */
	void InitializeMessenger();

	/** Set the length (Z) of the detector (in mm)
	  */
	void SetDetectorLength(const G4double &val){ fDetectorLength = val; }

	/** Set the width (X) of the detector (in mm)
	  */
	void SetDetectorWidth(const G4double &val);

	/** Set the height (Y) of the detector (in mm)
	  */
	void SetDetectorHeight(const G4double &val);

	/** Set the width (X) of scintillator segments for segmented detector (in mm)
	  * @note If @a val <= 0, assumes segments of uniform width, each with a layer of wrapping between them (if enabled). So the
	  *       total detector height, @a fDetectorWidth, is equal to fNumColumns*segmentWidth+(fNumColumns-1)*fWrappingThickness
	  */
	void SetSegmentWidth(const G4double &val);

	/** Set the height (Y) of scintillator segments for segmented detector (in mm)
	  * @note If @a val <= 0, assumes segments of uniform height, each with a layer of wrapping between them (if enabled). So the
	  *       total detector height, @a fDetectorHeight, is equal to fNumRows*segmentHeight+(fNumRows-1)*fWrappingThickness
	  */
	void SetSegmentHeight(const G4double &val);

	/** Set the length of the trapezoidal light-guide (in mm)
	  */
	void SetTrapezoidLength(const G4double &val){ fTrapezoidLength = val; }

	/** Set the angle of the trapezoidal light-guide for elliptical detectors (in degrees)
	  * 
	  * @note The trapezoid angle is defined with respect to the edge of the rectangular body, meaning that
	  *       the angle should be > 0 and < 90 degrees. Angles outside of this range will be ignored.
	  */
	void SetTrapezoidAngle(const G4double &val){ if(val > 0 && val < 90) fTrapezoidAngle = val; }

	/** Set the length of the light diffuser (in mm)
	  */
	void SetDiffuserLength(const G4double &val){ fDiffuserLength = val; }

	/** Set the thickness of the inner and outer reflective wrapping (in mm)
	  */
	void SetMylarThickness(const G4double &val){ fWrappingThickness = val; }

	/** Set the thickness of optical grease layers (in mm)
	  */
	void SetGreaseThickness(const G4double &val){ fGreaseThickness = val; }

	/** Set the thickness of the PMT quartz windows (in mm)
	  */
	void SetWindowThickness(const G4double &val){ fWindowThickness = val; }

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
	
	/** Set the current scintillator segment copy number
	  */
	void SetScintillatorCopyNumber(const G4int &val){ scintCopyNum = val; }
	
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
	void SetDetectorMaterial(const G4String &material);

	/** Set the name of the optical reflector material
	  * @note See nDetMaterials::getUserWrappingMaterial() for valid material names
	  */
	void SetWrappingMaterial(const G4String &material);

	/** Set or unset this detector as a start signal for timing
	  */
	void SetAsStart(const bool &enabled){ isStart = enabled; }

	/** Set this detector as non-segmented
	  */
	void SetUnsegmented();
	
	/** Set the pointer to the materials container object
	  */
	void SetMaterials(nDetMaterials *matptr){ materials = matptr; }

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

	/** Get the width of the PMT along the x-axis (in mm)
	  */
	G4double GetPmtWidth() const { return pmtWidth; }

	/** Get the height of the PMT along the y-axis (in mm)
	  */
	G4double GetPmtHeight() const { return pmtHeight; }

	/** Get the uniform width of scintillator segments for segmented detectors (in mm)
	  */
	G4double GetSegmentWidth() const { return fSegmentWidth; }

	/** Get the uniform height of scintillator segments for segmented detectors (in mm)
	  */	
	G4double GetSegmentHeight() const { return fSegmentHeight; }

	/** Return true if this is a start detector and return false otherwise
	  */
	bool GetIsStart() const { return isStart; }

	/** Return true if the detector is segmented and return false otherwise
	  */
	bool IsSegmented() const { return (fNumColumns > 1 || fNumRows > 1); }

	/** Return true if the PMT is segmented and return false otherwise
	  */
	bool PmtIsSegmented() const { return (fNumColumnsPmt > 0 && fNumRowsPmt > 0); }
	
	/** Return true if the wrapping thickness is positive and non-zero and return false otherwise
	  */
	bool WrappingEnabled() const { return (fWrappingThickness > 0); }

	/** Print all detector parameters
	  */
	void Print() const ;

protected:
	G4double pmtWidth; ///< The width (x-axis) of the PMT (in mm)
	G4double pmtHeight; ///< The height (y-axis) of the PMT (in mm)
	G4double fWrappingThickness; ///< Thickness of the inner and outer detector wrapping (in mm)
	G4double fGreaseThickness; ///< Thickness of all optical grease layers (in mm)
	G4double fWindowThickness; ///< Thickness of all optical window layers (in mm)
	G4double fSensitiveThickness; ///< Thickness of the optical photon-sensitive surfaces (in mm)
	G4double fDetectorLength; ///< Size of the detector along the z-axis (in mm)
	G4double fDetectorHeight; ///< Size of the detector along the y-axis (in mm)
	G4double fDetectorWidth; ///< Size of the detector along the x-axis (in mm)
	G4double fTrapezoidLength; ///< Length of the trapezoids used as light guides (in mm)
	G4double fTrapezoidAngle; ///< Angle of the trapezoidal light-guide for elliptical detectors (in degrees)
	G4double fDiffuserLength; ///< Length of straight diffusers (in mm)
	G4double fSegmentWidth; ///< Uniform width of scintillator segments for segmented detectors (in mm)
	G4double fSegmentHeight; ///< Uniform height of scintillator segments for segmented detectors (in mm)
	
	G4int fNumColumns; ///< Current number of scintillator columns (x-axis) for modular detectors
	G4int fNumRows; ///< Current number of scintillator rows (y-axis) for modular detectors
	G4int fNumColumnsPmt; ///< Current number of PMT anode columns (x-axis) for PSPmts
	G4int fNumRowsPmt; ///< Current number of PMT anode rows (y-axis) for PSPmts
	G4int scintCopyNum; ///< Current scintillator segment copy number

	G4bool fPolishedInterface; /*!< Flag indicating that a polished interface should be used between scintillators and optical grease
	                                otherwise a Lambertian surface will be used for the optical interface */
	G4bool fSquarePMTs; ///< Flag indicating that the added PMTs are square. If set to false, the PMTs will be circular, and the parameter @a pmtWidth will be used as the diameter
	G4bool isStart; ///< Flag indicating that this detector is used as a start signal for timing
	
	G4String detectorMaterialName; ///< String indicating the material to use for the detector scintillator body
	G4String wrappingMaterialName; ///< String indicating the material to use for the inner and outer detector wrapping

	G4ThreeVector detectorPosition; ///< Position of detector in the lab frame
	G4RotationMatrix detectorRotation; ///< Rotation of detector in the lab frame

	bool constantWidth; ///< Flag indicating that the user has specified a constant detector width, i.e. segment width will be variable
	bool constantHeight; ///< Flag indicating that the user has specified a constant detector height, i.e. segment height will be variable

	G4String geomType; ///< Integer value indicating the of the detector geometry

	G4ThreeVector maxBodySize; ///< The maximum size of the detector body

	G4Material* scintMaterial; ///< Pointer to the detector scintillator material
	G4Material* wrappingMaterial; ///< Pointer to the detector wrapping material

	G4OpticalSurface* wrappingOpSurf; ///< Pointer to the wrapping optical surface
	
	G4VisAttributes* scintVisAtt; ///< Pointer to the detector scintillator visual attributes
	G4VisAttributes* wrappingVisAtt; ///< Pointer to the detector wrapping visual attributes

	nDetMaterials *materials; ///< Pointer to the NEXTSim Geant materials container
	
	nDetDetectorMessenger *fMessenger; ///< Geant messenger to use for this class
	
	/** Update the size of the detector
	  */
	void UpdateSize();
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
	                 checkOverlaps(false) { }
	
	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  * @param matptr Pointer to the Geant materials handler class which will be used for detector construction
	  */
	nDetDetector(nDetConstruction *detector, nDetMaterials *matptr);

	/** Destructor
	  */
	virtual ~nDetDetector();
	
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

	/** Build the detector body, add component layers, and attach photo-sensitive surfaces
	  */
	void construct();

	/** Add a user defined layer to the queue
	  * @note When buildAllLayers() is called, the layers will be added on a first-in-first-out basis
	  */
	void addLayer(nDetWorldObject *layer){ userLayers.push_back(layer); }

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

	/** Load a light guide model from a file using parameters from a space-delimited input string and place it into the current detector assembly
	  * @note See gdmlLightGuideLayer::decodeString() for input string syntax
	  */
	void addLightGuideGDML(const G4String &input);

	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note See greaseLayer::decodeString() for input string syntax
	  */
	void addGreaseLayer(const G4String &input);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See diffuserLayer::decodeString() for input string syntax
	  */
	void addDiffuserLayer(const G4String &input);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See lightGuideLayer::decodeString() for input string syntax
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

	/** Add a logical volume to the left side (+z) of the detector assembly
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  * @return A pointer to the physical volume of the daughter
	  */
	G4PVPlacement *addLeftComponent(G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** Add a logical volume to the right side (-z) of the detector assembly
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  * @return A pointer to the physical volume of the daughter
	  */
	G4PVPlacement *addRightComponent(G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** Add a logical volume to both the left and right sides of the detector assembly
	  * @note This method assumes that the input volume is symmetric about the z-axis and, thus, it is
	  *       not rotated when being placed on the opposite side of the detector assembly
	  * @param volume Pointer to the logical volume to add to the detector body
	  * @param offset The position offset along the length axis of the detector (in mm)
	  * @param name The name of the new physical volume. If the name is blank, the name of the input logical volume will be used
	  * @param rot Pointer to the rotation matrix of the daughter within the detector body
	  */
	void addMirroredComponents(G4LogicalVolume *volume, const G4double &offset, const G4String &name="", G4RotationMatrix *rot=NULL);

	/** Add a logical volume to both the left and right sides of the detector assembly and return their physical volumes
	  *	@note This method assumes that the input volume is symmetric about the z-axis and, thus, it is
	  *       not rotated when being placed on the opposite side of the detector assembly
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

protected:
	G4LogicalVolume *assembly_logV; ///< Pointer to the logical volume of the mother of the detector
	G4PVPlacement* assembly_physV; ///< Pointer to the physical volume of the mother of the detector

	G4double layerSizeX; ///< Width of the current user add layer (in mm)
	G4double layerSizeY; ///< Height of the current user add layer (in mm)
	G4double offsetZ; ///< Offset of the current user add layer along the z-axis (in mm)
	
	G4int parentCopyNum; ///< Copy number of the mother of the detector
	G4int firstSegmentCopyNum; ///< Copy number of the first scintillator segment
	G4int lastSegmentCopyNum; ///< Copy number of the last scintillator segment

	bool checkOverlaps; ///< Flag indicating that Geant should check for overlaps between all placed objects

	centerOfMass cmL; ///< Center-of-mass calculator for the left PMT
	centerOfMass cmR; ///< Center-of-mass calculator for the right PMT

	std::vector<nDetWorldObject*> userLayers; ///< Vector of all layers added by the user

	std::vector<G4PVPlacement*> scintBody_physV; ///< Vector of all physical volumes of all scintillator segments

	/** Prepare for the detector volume to be built. In this method, the user should set the maximum
	  * size constraints of the body of the detector (@a maxBodySize) so that the detector handler knows
	  * how large to make its bounding assembly volume.
	  * @note By default, the maximum body size is taken from @a fDetectorWidth, @a fDetectorHeight, and @a fDetectorLength
	  */
	virtual void prepareToBuild();

	/** Build the physical detector volume for the detector handler
	  */
	virtual void buildDetector(){ }
	
	/** Perform tasks after the detector assembly has been placed into the setup area
	  */
	virtual void afterPlacement(){ }

	/** Build PMTs for the current detector and place them in the assembly volume
	  * 
	  * Each PMT consists of an optical grease layer, a quartz window, and a photo-sensitive surface. Additionally,
	  * a reflective wrapping layer is added around the outside of the PMT if @a fWrappingThickness is greater than zero.
	  */
	void constructPSPmts();

	/** Return a geometric solid volume with a specified size
	  *
	  * If the class member @a fSquarePMTs is true, then a G4Box is returned, otherwise a G4Tubs is returned
	  * @note All size parameters are specified as the TOTAL size and not the half-size as with most Geant geometry methods
	  * @param name The name of the new geometric solid
	  * @param width The total size along the X-axis (in mm)
	  * @param height The total size along the Y-axis (in mm)
	  * @param length The total size along the Z-axis (in mm)
	  * @return A pointer to the new G4CSGSolid (Constructed Solid Geometry) volume 
	  */
	G4CSGSolid *getVolume(const G4String &name, const G4double &width, const G4double &height, const G4double &length);

	/** Return a geometric solid trapezoid or cone with a specified size
	  *
	  * If the class member @a fSquarePMTs is true, then a G4Trd is returned, otherwise a G4Cons is returned
	  * @note All size parameters are specified as the TOTAL size and not the half-size as with most Geant geometry methods
	  * @param name The name of the new geometric solid
	  * @param w1 The total size along the X-axis at z = -length/2 (in mm)
	  * @param w2 The total size along the X-axis at z = +length/2 (in mm)
	  * @param h1 The total size along the Y-axis at z = -length/2 (in mm)
	  * @param h2 The total size along the Y-axis at z = +length/2 (in mm)
	  * @param length The total size along the Z-axis (in mm)
	  * @return A pointer to the new G4CSGSolid (Constructed Solid Geometry) volume 
	  */
	G4CSGSolid *getLightGuideVolume(const G4String &name, const G4double &w1, const double &w2, const double &h1, const double &h2, const G4double &length);
};

#endif
