//
// $Id: nDetConstruction.hh,v 1.0 Sept., 2015 $
//  Written by Dr. Xiaodong Zhang
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef nDetConstruction_h
#define nDetConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "centerOfMass.hh"

#include "G4RotationMatrix.hh"

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class gdmlSolid{
  public:
	gdmlSolid() : parent_physV(NULL), parent_logV(NULL), nDaughters(0) { }
	
	gdmlSolid(const char *fname);

	double getWidth() const { return 2*size[0]; }
	
	double getThickness() const { return 2*size[1]; }
	
	double getLength() const { return 2*size[2]; }

	G4VPhysicalVolume *getPhysicalVolume(){ return parent_physV; }
	
	G4LogicalVolume *getLogicalVolume(){ return parent_logV; }

	G4LogicalVolume *read(const char *fname, G4OpticalSurface *surface=NULL, const bool &checkOverlaps=false);

  private:
	G4VPhysicalVolume *parent_physV;
	G4LogicalVolume *parent_logV;
  
	std::vector<G4VPhysicalVolume*> physV;  
	std::vector<G4LogicalVolume*> logV;
	
	std::vector<G4PVPlacement*> placements;
	std::vector<G4LogicalBorderSurface*> borders;
  
	std::vector<G4ThreeVector> pos;
	std::vector<G4RotationMatrix*> rot;
	
	size_t nDaughters;
	
	double size[3];
};

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

    void SetHexagonRadius(G4double val){ fHexagonRadius = val; }
        
    void SetMylarThickness(G4double val){ fMylarThickness = val; }
    
    void SetGreaseThickness(G4double val){ fGreaseThickness = val; }
    
	void SetNumColumns(const G4int &val){ fNumColumns = val; }
	
	void SetNumRows(const G4int &val){ fNumRows = val; }

	void SetNumPmtColumns(const G4int &val){ fNumColumnsPmt = val; }
	
	void SetNumPmtRows(const G4int &val){ fNumRowsPmt = val; }

    void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);

    bool setPmtSpectralResponse(const char *fname);

	bool setPmtGainMatrix(const char *fname);

	void SetGdmlFilename(const std::string &fname){ gdmlFilename = fname; }

	G4double GetDetectorLength() const { return fDetectorLength; }
	
	G4double GetDetectorWidth() const { return fDetectorWidth; }
	
	G4double GetDetectorThickness() const { return fDetectorThickness; }
	
	G4ThreeVector GetDetectorSize() const { return G4ThreeVector(fDetectorWidth, fDetectorThickness, fDetectorLength); }

	G4double GetDetectorPosX() const { return 0; }
	
	G4double GetDetectorPosY() const { return 0; }

	G4double GetDetectorPosZ() const { return 0; }
	
	G4ThreeVector GetDetectorPos() const { return G4ThreeVector(0, 0, 0); }

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
    
    G4LogicalVolume *LoadGDML(const G4String &fname, gdmlSolid &solid, G4OpticalSurface *surface=NULL);

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

    G4double greaseX;
    G4double greaseY;
    G4double greaseZ;

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
    G4double fDetectorLength;
    G4double fDetectorThickness;
    G4double fDetectorWidth;
    G4double fTrapezoidLength;
    G4double fHexagonRadius;
    G4double fLightYieldScale;

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
    G4LogicalVolume *mylar_logV;
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
    G4Material* fEJ299;
    G4Material* fGrease;
    G4Material* fSiO2;
    G4Material* fSil;
    G4Material* fMylar;
    G4Material* fAcrylic;

    //Material table properties
    G4MaterialPropertiesTable* fAirMPT;
    G4MaterialPropertiesTable* fTeflonMPT;
    G4MaterialPropertiesTable* fEJ200MPT;
    G4MaterialPropertiesTable* fEJ299MPT;
    G4MaterialPropertiesTable* fGreaseMPT;
    G4MaterialPropertiesTable* fSiO2MPT;
    G4MaterialPropertiesTable* fSilMPT;
    G4MaterialPropertiesTable* fMylarMPT;

    //Optical Surfaces
    G4OpticalSurface* fTeflonOpticalSurface;
    G4OpticalSurface* fSiliconPMOpticalSurface;
    G4OpticalSurface* fMylarOpticalSurface;

    //Logical Skins
    G4LogicalSkinSurface* fWrapSkinSurface;
    G4LogicalSkinSurface* fSiPMSkinSurface;
    G4LogicalBorderSurface* fMylarSurface;

	std::string gdmlFilename; // GDML model filename to load.

    // member functions
    void buildExpHall();

    void buildRectangle();
    
    void buildTestAssembly();
    
    void DefineMaterials();

    void buildSiPMs(const G4double &zOffset=0);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*nDetConstruction_h*/
