#ifndef NDETCONSTRUCTION_HH
#define NDETCONSTRUCTION_HH

#include <vector>
#include <deque>

#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "centerOfMass.hh"
#include "gdmlSolid.hh"
#include "nistDatabase.hh"

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

class userAddLayer;

///////////////////////////////////////////////////////////////////////////////
// class nDetConstruction
///////////////////////////////////////////////////////////////////////////////

class nDetConstruction : public G4VUserDetectorConstruction
{
  public:
    nDetConstruction(const G4double &scale=1);
    
   ~nDetConstruction();

  public:
    G4VPhysicalVolume* Construct();
    
    G4VPhysicalVolume* ConstructDetector();
    
    virtual void ConstructSDandField();
    
    void SetGeometry(G4String geom){ fGeometry = geom; }

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

	G4double GetDetectorLength() const { return fDetectorLength; }
	
	G4double GetDetectorWidth() const { return fDetectorWidth; }
	
	G4double GetDetectorThickness() const { return fDetectorThickness; }
	
	G4ThreeVector GetDetectorSize() const { return G4ThreeVector(fDetectorWidth, fDetectorThickness, fDetectorLength); }

	G4double GetDetectorPosX() const { return detectorPosition.getX(); }
	
	G4double GetDetectorPosY() const { return detectorPosition.getY(); }

	G4double GetDetectorPosZ() const { return detectorPosition.getZ(); }
	
	G4ThreeVector GetDetectorPos() const { return detectorPosition; }

	G4RotationMatrix *GetDetectorRot(){ return &detectorRotation; }

    G4String GetGeometry(){ return fGeometry; }

	G4int GetNumPmtColumns() const { return fNumColumnsPmt; }
	
	G4int GetNumPmtRows() const { return fNumRowsPmt; }

	void GetSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const ;

    centerOfMass *GetCenterOfMassPositiveSide(){ return &center[0]; }
  
    centerOfMass *GetCenterOfMassNegativeSide(){ return &center[1]; }

	nDetConstructionMessenger *GetMessenger(){ return fDetectorMessenger; }

    bool PmtIsSegmented() const { return (fNumColumnsPmt > 0 && fNumRowsPmt > 0); }
    
    bool AddDetectedPhoton(const G4Step *step, const double &mass=1);

	void GetDetectedPhotons(size_t &numLeft, size_t &numRight);

	void Clear();
    
    void UpdateGeometry();

    void AddGDML(const G4String &input);

	void AddLightGuideGDML(const G4String &input);

	void AddGrease(const G4String &input);

	void AddDiffuser(const G4String &input);

	void AddLightGuide(const G4String &input);

private:
    nDetConstructionMessenger *fDetectorMessenger;
    
    G4double expHallX;
    G4double expHallY;
    G4double expHallZ;

    G4double assemblyBoxX;
    G4double assemblyBoxY;
    G4double assemblyBoxZ;

    G4double assemblyPx;
    G4double assemblyPy;
    G4double assemblyPz;

    G4double ej200X;
    G4double ej200Y;
    G4double ej200Z;

    G4double qwSiPMx;
    G4double qwSiPMy;
    G4double qwSiPMz;

    G4double psSiPMx;
    G4double psSiPMy;
    G4double psSiPMz;

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

    centerOfMass center[2];

    G4bool fCheckOverlaps;
    G4String fGeometry;

    // logical and physical volume
    G4LogicalVolume* expHall_logV;
    G4LogicalVolume* assembly_logV;

    G4LogicalVolume* ej200_logV; // ej200 scintillator
    G4LogicalVolume* mylar_logV;
    G4LogicalVolume* grease_logV;
    G4LogicalVolume* qwSiPM_logV;
    G4LogicalVolume* psSiPM_logV;

    G4VPhysicalVolume* expHall_physV;
    G4VPhysicalVolume* assembly_physV;

    SiPMSD *fSiPMSD;
    nDetSD *fScintSD;

    //Materials and elements
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

    //Material table properties
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

    //Optical Surfaces
    G4OpticalSurface* fTeflonOpticalSurface;
    G4OpticalSurface* fSiliconPMOpticalSurface;
    G4OpticalSurface* fMylarOpticalSurface;
    G4OpticalSurface* fEsrOpticalSurface;
	G4OpticalSurface* fPerfectOpticalSurface;

    //Logical Skins
    G4LogicalSkinSurface* fWrapSkinSurface;
    G4LogicalSkinSurface* fSiPMSkinSurface;
    G4LogicalBorderSurface* fMylarSurface;

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

	// Deque of layers added by the user.
	std::deque<userAddLayer> userLayers;

    // member functions
    void buildExpHall();

    void buildModule();
    
    void buildEllipse();

	void buildRectangle();
    
    void buildTestAssembly();
    
    void DefineMaterials();

	G4ThreeVector constructAssembly();

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

#endif
