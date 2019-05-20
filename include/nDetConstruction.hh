#ifndef NDETCONSTRUCTION_HH
#define NDETCONSTRUCTION_HH

#include <vector>
#include <deque>

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

    void SetHexagonRadius(G4double val){ fHexagonRadius = val; }
        
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

	G4RotationMatrix *GetDetectorRot(){ return &detectorRotation; }

	G4int GetNumPmtColumns() const { return fNumColumnsPmt; }
	
	G4int GetNumPmtRows() const { return fNumRowsPmt; }

	void GetSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const ;    
		
	nDetConstructionMessenger *GetMessenger(){ return fDetectorMessenger; }

    bool PmtIsSegmented() const { return (fNumColumnsPmt > 0 && fNumRowsPmt > 0); }

	bool AddDetectedPhoton(const G4Step *step, const double &mass=1);

    void ClearGeometry();
    
    void UpdateGeometry();

    void AddGDML(const G4String &input);

	void AddLightGuideGDML(const G4String &input);

	void AddGrease(const G4String &input);

	void AddDiffuser(const G4String &input);

	void AddLightGuide(const G4String &input);
	
	void SetLightYieldMultiplier(const G4double &input){ fLightYieldScale = input; }

private:
    nDetConstructionMessenger *fDetectorMessenger;
    
    G4double expHallX;
    G4double expHallY;
    G4double expHallZ;

    G4double SiPM_dimension;
    G4double fTeflonThickness;
    G4double fMylarThickness;
    G4double fGreaseThickness;
    G4double fWindowThickness;
    G4double fSensitiveThickness;
    G4double fDetectorLength;
    G4double fDetectorThickness;
    G4double fDetectorWidth;
    G4double fTrapezoidLength;
    G4double fDiffuserLength;
    G4double fHexagonRadius;
    G4double fLightYieldScale;

	G4double currentLayerSizeX;
	G4double currentLayerSizeY;    
	G4double currentOffsetZ;

    G4int fNdetectors;
	G4int fNumColumns;
	G4int fNumRows;
	G4int fNumColumnsPmt;
	G4int fNumRowsPmt;

	G4bool fPolishedInterface;
    G4bool fCheckOverlaps;

    // Logical and physical volume
	G4LogicalVolume* expHall_logV;
	G4VPhysicalVolume* expHall_physV;

    // Materials and elements
    G4Element* fH;
    G4Element* fC;
    G4Element* fO;
    G4Element* fF;
    G4Element* fSi;
    G4Element* fAl;

    G4Material* fAir;
    G4Material* fVacuum;
    G4Material* fTeflon;
    G4Material* fEJ200;
    G4Material* fEJ276;
    G4Material* fGrease;
    G4Material* fSiO2;
    G4Material* fSil;
    G4Material* fMylar;
    G4Material* fAcrylic;
	G4Material* fAluminum;

    // Material table properties
    G4MaterialPropertiesTable* fAirMPT;
    G4MaterialPropertiesTable* fTeflonMPT;
    G4MaterialPropertiesTable* fEJ200MPT;
    G4MaterialPropertiesTable* fEJ276MPT;
    G4MaterialPropertiesTable* fGreaseMPT;
    G4MaterialPropertiesTable* fSiO2MPT;
    G4MaterialPropertiesTable* fSilMPT;
	G4MaterialPropertiesTable* fMylarMPT;
	G4MaterialPropertiesTable* fPerfectMPT;
	G4MaterialPropertiesTable* fAluminumMPT;
	G4MaterialPropertiesTable* fEsrMPT;

    // Optical Surfaces
    G4OpticalSurface* fTeflonOpticalSurface;
    G4OpticalSurface* fSiliconPMOpticalSurface;
    G4OpticalSurface* fMylarOpticalSurface;
    G4OpticalSurface* fEsrOpticalSurface;
	G4OpticalSurface* fPerfectOpticalSurface;
	G4OpticalSurface* fGreaseOpticalSurface;

	// Visual attributes
	G4VisAttributes *assembly_VisAtt;
	G4VisAttributes *sensitive_VisAtt;
	G4VisAttributes *window_VisAtt;
	G4VisAttributes *grease_VisAtt;
	G4VisAttributes *wrapping_VisAtt;
	G4VisAttributes *scint_VisAtt;
	G4VisAttributes *shadow_VisAtt;

	// Loaded gdml solids.
	std::vector<gdmlSolid> solids;

	// Database of elements and materials.
	nistDatabase nist;

	G4String detectorMaterial;
	G4String wrappingMaterial;

	G4ThreeVector detectorPosition; // Position of detector in lab frame.
	G4RotationMatrix detectorRotation; // Rotation of detector.

	// Shadow bar
	G4ThreeVector shadowBarSize;
	G4ThreeVector shadowBarPos;
	G4Material *shadowBarMaterial;

	// Optical grease placement.
	std::vector<G4PVPlacement*> scintBody_physV;

	// Deque of detectors added by the user.
	std::deque<userAddDetector> userDetectors;
	
	// Pointer to the current detector construction.
	userAddDetector *currentDetector;
	G4LogicalVolume *currentAssembly;

	bool materialsAreDefined;

	centerOfMass center[2];

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
	G4String argStr;

	void (nDetConstruction::* ptr)(const G4String &);
};

///////////////////////////////////////////////////////////////////////////////
// class userAddDetector
///////////////////////////////////////////////////////////////////////////////

class userAddDetector{
  public:
	userAddDetector() : assembly_logV(NULL), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0) { }
	
	userAddDetector(G4LogicalVolume *logical) : assembly_logV(logical), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0) { }
	
	userAddDetector(G4LogicalVolume *logical, G4VPhysicalVolume *physical) : assembly_logV(logical), assembly_physV(physical), layerSizeX(0), layerSizeY(0), offsetZ(0) { }
	
	G4LogicalVolume *getLogicalVolume(){ return assembly_logV; }

	void getCurrentOffset(G4double &x_, G4double &y_, G4double &z_);
	
	void setPositionAndRotation(const G4ThreeVector &pos, const G4RotationMatrix &rot);

	void setCurrentOffset(const G4double &x_, const G4double &y_, const G4double &z_);

	void addLayer(const userAddLayer &layer){ userLayers.push_back(layer); }

	void buildAllLayers(nDetConstruction *detector);
	
	void placeDetector(G4LogicalVolume *parent);

  private:
	G4LogicalVolume *assembly_logV;
	G4VPhysicalVolume* assembly_physV;

	G4double layerSizeX;
	G4double layerSizeY;    
	G4double offsetZ;

	G4ThreeVector position; // Position of detector in lab frame.
	G4RotationMatrix rotation; // Rotation of detector.
	
	// Deque of layers added by the user.
	std::deque<userAddLayer> userLayers;	
};

#endif
