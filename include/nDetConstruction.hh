#ifndef NDET_CONSTRUCTION_HH
#define NDET_CONSTRUCTION_HH

#include <vector>

#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "gdmlSolid.hh"
#include "nistDatabase.hh"
#include "centerOfMass.hh"

// Class declarations
class nDetConstructionMessenger;
class G4LogicalVolume;
class G4VPhysicalVolume;
class SiPMSD;
class nDetSD;
class G4Element;
class G4Material;
class G4MaterialPropertiesTable;
class G4OpticalSurface;
class G4LogicalSkinSurface;
class G4LogicalBorderSurface;
class G4VPhysicalVolume;
class G4AssemblyVolume;
class G4PVPlacement;
class G4VSolid;
class G4Box;
class G4VisAttributes;
class G4Step;

class userAddLayer;
class userAddDetector;

///////////////////////////////////////////////////////////////////////////////
// class nDetConstruction
///////////////////////////////////////////////////////////////////////////////

class nDetConstruction : public G4VUserDetectorConstruction{
  public:
	~nDetConstruction();

	nDetConstruction(nDetConstruction const &copy); // Not Implemented
	
	nDetConstruction &operator=(nDetConstruction const &copy); // Not Implemented

	static nDetConstruction &getInstance();

	G4VPhysicalVolume* Construct();

	G4VPhysicalVolume* ConstructDetector();
    
	void AddGeometry(const G4String &geom);

    void SetSiPM_dimension(G4double dim){ SiPM_dimension = dim; }
    
    void SetDetectorLength(G4double val){ fDetectorLength = val; }
    
    void SetDetectorWidth(G4double val){ fDetectorWidth = val; }   
    
    void SetDetectorThickness(G4double val){ fDetectorThickness = val; }   
    
    void SetTrapezoidLength(G4double val){ fTrapezoidLength = val; }

    void SetDiffuserLength(G4double val){ fDiffuserLength = val; }

    void SetMylarThickness(G4double val){ fMylarThickness = val; }
    
    void SetGreaseThickness(G4double val){ fGreaseThickness = val; }
    
	void SetWindowThickness(G4double val){ fWindowThickness = val; }
    
	void SetNumColumns(const G4int &val){ fNumColumns = val; }
	
	void SetNumRows(const G4int &val){ fNumRows = val; }

	void SetNumPmtColumns(const G4int &val){ fNumColumnsPmt = val; }
	
	void SetNumPmtRows(const G4int &val){ fNumRowsPmt = val; }

    void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);

    bool setPmtSpectralResponse(const char *fname);

	bool setPmtGainMatrix(const char *fname);

	void SetPosition(const G4ThreeVector &position){ detectorPosition = position; }	

	void SetPositionCylindrical(const G4ThreeVector &position);
	
	void SetPositionSpherical(const G4ThreeVector &position);

	void SetRotation(const G4ThreeVector &rotation);

	void SetDetectorMaterial(const G4String &material){ detectorMaterial = material; }

	void SetWrappingMaterial(const G4String &material){ wrappingMaterial = material; }

	void SetShadowBarSize(const G4ThreeVector &size);
	
	void SetShadowBarPosition(const G4ThreeVector &pos);

	bool SetShadowBarMaterial(const G4String &material);
	
	void SetPolishedInterface(const bool &state){ fPolishedInterface = state; }

	G4double GetDetectorLength() const { return fDetectorLength; }
	
	G4double GetDetectorWidth() const { return fDetectorWidth; }
	
	G4double GetDetectorThickness() const { return fDetectorThickness; }
	
	G4ThreeVector GetDetectorSize() const { return G4ThreeVector(fDetectorWidth, fDetectorThickness, fDetectorLength); }

	G4double GetDetectorPosX() const { return detectorPosition.getX(); }
	
	G4double GetDetectorPosY() const { return detectorPosition.getY(); }

	G4double GetDetectorPosZ() const { return detectorPosition.getZ(); }
	
	G4ThreeVector GetDetectorPos() const { return detectorPosition; }

	G4RotationMatrix GetDetectorRot() const { return detectorRotation; }

	G4int GetNumPmtColumns() const { return fNumColumnsPmt; }
	
	G4int GetNumPmtRows() const { return fNumRowsPmt; }

	nDetConstructionMessenger *GetMessenger(){ return fDetectorMessenger; }

	centerOfMass *GetCenterOfMassL(){ return &center[0]; }
	
	centerOfMass *GetCenterOfMassR(){ return &center[1]; }

	pmtResponse *GetPmtResponseL(){ return center[0].getPmtResponse(); }
	
	pmtResponse *GetPmtResponseR(){ return center[1].getPmtResponse(); }

	std::vector<userAddDetector> GetUserDetectors() const { return userDetectors; }

    bool PmtIsSegmented() const { return (fNumColumnsPmt > 0 && fNumRowsPmt > 0); }

	bool AddDetectedPhoton(const G4Step *step, const double &mass=1);

    void ClearGeometry();
    
    void UpdateGeometry();

    void AddGDML(const G4String &input);

	void AddLightGuideGDML(const G4String &input);

	void AddGrease(const G4String &input);

	void AddDiffuser(const G4String &input);

	void AddLightGuide(const G4String &input);
	
	void AddDetectorArray(const G4String &input);
	
	void SetLightYieldMultiplier(const G4double &input){ fLightYieldScale = input; }

private:
    nDetConstructionMessenger *fDetectorMessenger;
    
    G4double expHallX; ///< Size of the world along the x-axis (in mm)
    G4double expHallY; ///< Size of the world along the y-axis (in mm)
    G4double expHallZ; ///< Size of the world along the z-axis (in mm)

    G4double SiPM_dimension; ///< Half of the width and height of the PMT (in mm)
    G4double fMylarThickness; ///< Thickness of the inner and outer detector wrapping (in mm)
    G4double fGreaseThickness; ///< Thickness of all optical grease layers (in mm)
    G4double fWindowThickness; ///< Thickness of all optical window layers (in mm)
    G4double fSensitiveThickness; ///< Thickness of the optical photon-sensitive surfaces (in mm)
    G4double fDetectorLength; ///< Size of the detector along the z-axis (in mm)
    G4double fDetectorThickness; ///< Size of the detector along the y-axis (in mm)
    G4double fDetectorWidth; ///< Size of the detector along the x-axis (in mm)
    G4double fTrapezoidLength; ///< Thickness of the trapezoids used as light guides (in mm)
    G4double fDiffuserLength; ///< Thickness of straight diffusers (in mm)
    G4double fLightYieldScale; ///< Scaling parameter used for scintillation light yield (default=1)

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

    // Logical and physical volume
	G4LogicalVolume* expHall_logV; ///< Logical volume of the world
	G4VPhysicalVolume* expHall_physV; ///< Physical volume of the world

    // Materials and elements
    G4Element* fH; ///< Hydrogen
    G4Element* fC; ///< Carbon
    G4Element* fO; ///< Oxygen
    G4Element* fF; ///< Flourine
    G4Element* fSi; ///< Silicon
    G4Element* fAl; ///< Aluminium

    G4Material* fAir; ///< Material corresponding to air
    G4Material* fVacuum; ///< Material corresponding to natural vacuum
    G4Material* fTeflon; ///< Material corresponding to teflon
    G4Material* fEJ200; ///< Material corresponding to EJ-200 scintillator
    G4Material* fEJ276; ///< Material corresponding to EJ-276 scintillator
    G4Material* fGrease; ///< Material corresponding to optical grease
    G4Material* fSiO2; ///< Material corresponding to quartz
    G4Material* fSil; ///< Material corresponding to silicon
    G4Material* fMylar; ///< Material corresponding to aluminized mylar
    G4Material* fAcrylic; ///< Material corresponding to acrylic
	G4Material* fAluminum; ///< Material corresponding to aluminum

    // Material table properties
    G4MaterialPropertiesTable* fAirMPT; ///< Material properties table for air
    G4MaterialPropertiesTable* fTeflonMPT; ///< Material properties table for teflon
    G4MaterialPropertiesTable* fEJ200MPT; ///< Material properties table for EJ-200 scintillator
    G4MaterialPropertiesTable* fEJ276MPT; ///< Material properties table for EJ-276 scintillator
    G4MaterialPropertiesTable* fGreaseMPT; ///< Material properties table for optical grease
    G4MaterialPropertiesTable* fSiO2MPT; ///< Material properties table for quartz
    G4MaterialPropertiesTable* fSilMPT; ///< Material properties table for silicon
	G4MaterialPropertiesTable* fMylarMPT; ///< Material properties table for aluminized mylar
	G4MaterialPropertiesTable* fPerfectMPT; ///< Material properties table for a perfect reflector
	G4MaterialPropertiesTable* fAluminumMPT; ///< Material properties table for aluminum
	G4MaterialPropertiesTable* fEsrMPT; ///< Material properties table for 3M Enhanced Specular Reflector

    // Optical Surfaces
    G4OpticalSurface* fTeflonOpticalSurface; ///< Optical surface for teflon
    G4OpticalSurface* fSiliconPMOpticalSurface; ///< Optical surface for silicon
    G4OpticalSurface* fMylarOpticalSurface; ///< Optical surface for aluminized mylar
    G4OpticalSurface* fEsrOpticalSurface; ///< Optical surface for 3M Enhanced Specular Reflector
	G4OpticalSurface* fPerfectOpticalSurface; ///< Optical surface for a perfect reflector
	G4OpticalSurface* fGreaseOpticalSurface; ///< Optical surface for optical grease

	// Visual attributes
	G4VisAttributes *assembly_VisAtt; ///< Visual attributes for the mother assembly
	G4VisAttributes *sensitive_VisAtt; ///< Visual attributes for the photo-sensitive surface
	G4VisAttributes *window_VisAtt; ///< Visual attributes for the optical window
	G4VisAttributes *grease_VisAtt; ///< Visual attributes for the optical grease
	G4VisAttributes *wrapping_VisAtt; ///< Visual attributes for the inner/outer detector wrappint
	G4VisAttributes *scint_VisAtt; ///< Visual attributes for the scintillator material
	G4VisAttributes *shadow_VisAtt; ///< Visual attributes for the shadow object

	std::vector<gdmlSolid> solids; ///< Vector of all loaded gdml solids

	nistDatabase nist; ///< Database used for pre-defined NIST element and material lookups

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

	bool materialsAreDefined; ///< Flag indicating that the hard-coded materials have been defined

	centerOfMass center[2]; ///< Objects used to compute the detected optical photon center-of-mass position for the left and right PMT

	// Private constructor.
	nDetConstruction();

    // member functions
    void buildExpHall();

    void buildModule();
    
    void buildEllipse();

	void buildRectangle();
    
    void buildTestAssembly();
    
    void DefineMaterials();

	G4LogicalVolume *constructAssembly(G4ThreeVector &boundingBox);

    void constructPSPmts();

	void applyGreaseLayer(const G4String &input);

    void applyGreaseLayer();

	void applyGreaseLayer(const G4double &x, const G4double &y, double thickness=0);

    void applyDiffuserLayer();

	void applyDiffuserLayer(const G4String &input);

    void applyDiffuserLayer(const G4double &x, const G4double &y, const double &thickness);

    void applyLightGuide();

	void applyLightGuide(const G4String &input);

    void applyLightGuide(const G4double &x2, const G4double &y2);

    void applyLightGuide(const G4double &x1, const G4double &x2, const G4double &y1, const G4double &y2, const double &thickness);
    
	void loadGDML(const G4String &input);
    
	G4LogicalVolume *loadGDML(const G4String &fname, const G4ThreeVector &position, const G4ThreeVector &rotation, const G4String &material);

	void loadLightGuide(const G4String &input);

	G4LogicalVolume *loadLightGuide(const G4String &fname, const G4ThreeVector &rotation, const G4String &material, G4OpticalSurface *surface);
    
    G4ThreeVector getPSPmtBoundingBox();
    
    G4Material *getUserDetectorMaterial();
    
    G4Material *getUserSurfaceMaterial();
    
    G4OpticalSurface *getUserOpticalSurface();
};

///////////////////////////////////////////////////////////////////////////////
// class userAddLayer
///////////////////////////////////////////////////////////////////////////////

class userAddLayer{
  public:
	userAddLayer() : argStr(), ptr(NULL) { }
	
	userAddLayer(const G4String &arg_, void (nDetConstruction::* ptr_)(const G4String &)) : argStr(arg_), ptr(ptr_) { }

	void execute(nDetConstruction *obj){ (obj->*ptr)(argStr); }

	G4String dump() const { return argStr; }

  private:
	G4String argStr; ///< Argument string from the macro supplied by the user

	void (nDetConstruction::* ptr)(const G4String &); ///< Pointer to a nDetConstruction member function taking a const G4String as an argument
};

///////////////////////////////////////////////////////////////////////////////
// class userAddDetector
///////////////////////////////////////////////////////////////////////////////

class userAddDetector{
  public:
	userAddDetector() : assembly_logV(NULL), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0), parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), numColumns(1), numRows(1) { }
	
	userAddDetector(G4LogicalVolume *logical) : assembly_logV(logical), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0), parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), numColumns(1), numRows(1) { }
	
	userAddDetector(G4LogicalVolume *logical, G4VPhysicalVolume *physical) : assembly_logV(logical), assembly_physV(physical), layerSizeX(0), layerSizeY(0), offsetZ(0), parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), numColumns(1), numRows(1) { }
	
	G4LogicalVolume *getLogicalVolume(){ return assembly_logV; }

	void getCurrentOffset(G4double &x_, G4double &y_, G4double &z_);
	
	G4int getParentCopyNumber() const { return parentCopyNum; }
	
	G4int getLeftPmtCopyNumber() const { return 2*parentCopyNum; }
	
	G4int getRightPmtCopyNumber() const { return 2*parentCopyNum+1; }
	
	G4ThreeVector *getPosition(){ return &position; }
	
	G4RotationMatrix *getRotation(){ return &rotation; }
	
	void setPositionAndRotation(const G4ThreeVector &pos, const G4RotationMatrix &rot);

	void setCurrentOffset(const G4double &x_, const G4double &y_, const G4double &z_);

	void setCurrentOffsetZ(const G4double &z_){ offsetZ = z_; }

	void setParentCopyNumber(const G4int &num){ parentCopyNum = num; }
	
	void setSegmentedDetector(const G4int &col, const G4int &row, const G4int &startCopyNum=0);

	void addLayer(const userAddLayer &layer){ userLayers.push_back(layer); }

	void buildAllLayers(nDetConstruction *detector);
	
	void placeDetector(G4LogicalVolume *parent);

	void clear();

	bool checkCopyNumber(const G4int &num) const { return (num >= firstSegmentCopyNum && num < lastSegmentCopyNum); }

	bool checkPmtCopyNumber(const G4int &num) const { return (num == 2*parentCopyNum || num == 2*parentCopyNum+1); }
	
	bool checkPmtCopyNumber(const G4int &num, bool &isLeft) const ;

	bool getSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const ;
	
	bool empty() const { return (cmL.empty() && cmR.empty()); }
	
	centerOfMass *getCenterOfMassL(){ return &cmL; }
	
	centerOfMass *getCenterOfMassR(){ return &cmR; }

	pmtResponse *getPmtResponseL(){ return cmL.getPmtResponse(); }
	
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

	G4ThreeVector position; ///< Position of the detector in the lab frame
	G4RotationMatrix rotation; ///< Rotation of the detector in the lab frame
	
	centerOfMass cmL; ///< Center-of-mass calculator for the left PMT
	centerOfMass cmR; ///< Center-of-mass calculator for the right PMT
	
	std::vector<userAddLayer> userLayers; ///< Vector of all layers added by the user
};

#endif
