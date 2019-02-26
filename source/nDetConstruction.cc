////
// $Id: nDetConstruction.cc,v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "nDetConstruction.hh"
#include "nDetConstructionMessenger.hh"

#include "G4LogicalVolume.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

#include "G4Box.hh"
#include "G4Trd.hh"

#include "G4GeometryManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "G4PVPlacement.hh"

#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4ExtrudedSolid.hh"
#include "G4PhysicalConstants.hh"

static const G4double inch = 2.54*cm;

///////////////////////////////////////////////////////////////////////////////
// class nDetConstruction
///////////////////////////////////////////////////////////////////////////////

nDetConstruction::nDetConstruction(const G4double &scale/*=1*/){
    expHall_logV = NULL;
    expHall_physV = NULL;
    
    assembly_logV = NULL;
    assembly_physV = NULL;
    
    ej200_logV = NULL;
    ej200X = 1.5*mm;
    ej200Y = 50*mm;
    ej200Z = 1.5*mm;
    
    // 100 micron thick grease on both ends of EJ200 bar
    grease_logV = NULL;
    
    // 1 mm thick SiPM window
    qwSiPM_logV = NULL;
    qwSiPMx = 1.5*mm;
    qwSiPMy = 0.5*mm;
    qwSiPMz = 1.5*mm; 
    
    // note that two sensing parts on both ends of scintillator are the same
    psSiPM_logV = NULL;
    psSiPMx = 1.5*mm;
    psSiPMy = 0.05*mm;
    psSiPMz = 1.5*mm; 

	fNumColumns = 1;
	fNumRows = 1;

	fNumColumnsPmt = -1;
	fNumRowsPmt = -1;

    G4cout<<"nDetConstruction::nDetConstruction()->"<<this<<G4endl;

    fDetectorMessenger=new nDetConstructionMessenger(this);

    fGeometry="rectangle";
	wrappingMaterial="mylar";

    fCheckOverlaps = false;
    fTeflonThickness = 0.11*mm;
    fMylarThickness = 0;
    fGreaseThickness = 0.1*mm;
    fWindowThickness = 0.1*mm;
    fSensitiveThickness = 1*mm;
    fDetectorLength = 3.94*inch;
    fDetectorWidth = 6*mm;
    fTrapezoidLength = 0;
    fDiffuserLength = 0;
    fHexagonRadius = 5*cm;
    fDetectorThickness = 0.24*inch;
    SiPM_dimension=3*mm;    
    
    currentLayerSizeX = 0;
    currentLayerSizeY = 0;
    currentOffsetZ = 0;
    
    fLightYieldScale = scale;

    expHallX = 10*m;
    expHallY = 10*m;
    expHallZ = 10*m;

    //Build the materials
    DefineMaterials();

    assembly_VisAtt = new G4VisAttributes();
    //assembly_VisAtt->SetVisibility(false);
	
	sensitive_VisAtt = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75)); // grey
    sensitive_VisAtt->SetForceSolid(true);
	
	window_VisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0)); // cyan	
	
	grease_VisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); // red
	
	wrapping_VisAtt = new G4VisAttributes();
    wrapping_VisAtt->SetColor(1, 0, 1, 0.5); // Alpha=50%
    wrapping_VisAtt->SetForceSolid(true);
	
	scint_VisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); // blue
	
	shadow_VisAtt = new G4VisAttributes();
    shadow_VisAtt->SetColor(0, 1, 0, 0.5); // green, Alpha=50%
    shadow_VisAtt->SetForceSolid(true);	
	
	shadowBarMaterial = NULL;
} // end of construction function.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetConstruction::~nDetConstruction()
{} // end of deconstruction function.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* nDetConstruction::Construct() {

    G4cout << "nDetConstruction::Construct()-->" << G4endl;

    if (expHall_physV) {
        G4GeometryManager::GetInstance()->OpenGeometry();
        G4PhysicalVolumeStore::GetInstance()->Clean();
        G4LogicalVolumeStore::GetInstance()->Clean();
        G4SolidStore::GetInstance()->Clean();
        G4LogicalSkinSurface::CleanSurfaceTable();
        G4LogicalBorderSurface::CleanSurfaceTable();
    }

    return ConstructDetector();
}

G4VPhysicalVolume* nDetConstruction::ConstructDetector(){
	// Build experiment hall
	buildExpHall();

	// Build the detector.
	if(fGeometry == "next" || fGeometry == "module")
		buildModule();
	else if(fGeometry == "ellipse")
		buildEllipse();
	else if(fGeometry == "rectangle")
		buildRectangle();
	else if(fGeometry == "test")
		buildTestAssembly();

	// Build the shadow bar.
	if(shadowBarMaterial){
		G4Box *shadowBox = new G4Box("shadowBox", shadowBarSize.getX()/2, shadowBarSize.getY()/2, shadowBarSize.getZ()/2);
		G4LogicalVolume *shadowBox_logV = new G4LogicalVolume(shadowBox, shadowBarMaterial, "shadowBox_logV", 0, 0, 0);
		shadowBox_logV->SetVisAttributes(shadow_VisAtt);
		new G4PVPlacement(0, shadowBarPos, shadowBox_logV, "ShadowBar", expHall_logV, true, 0, fCheckOverlaps);
	}

	return expHall_physV;
}

void nDetConstruction::setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_){
  center[0].setSegmentedPmt(col_, row_, width_, height_);
  center[1].setSegmentedPmt(col_, row_, width_, height_);
  std::cout << " nDetConstruction: Setting segmented PMTs with WxH=(" << width_ << " x " << height_ << ") and " << col_ << " columns and " << row_ << " rows.\n";
}

bool nDetConstruction::setPmtSpectralResponse(const char *fname){
  bool retval = (center[0].loadSpectralResponse(fname) && center[1].loadSpectralResponse(fname));
  if(retval)
    std::cout << " nDetConstruction: Successfully loaded PMT spectral response function\n";
  else
    std::cout << " nDetConstruction: ERROR! Failed to load PMT spectral response from \"" << fname << "\"!\n";
  return retval;
}

bool nDetConstruction::setPmtGainMatrix(const char *fname){
  bool retval = (center[0].loadGainMatrix(fname) && center[1].loadGainMatrix(fname));
  if(retval)
    std::cout << " nDetConstruction: Successfully loaded PMT anode gain matrix\n";
  else
    std::cout << " nDetConstruction: ERROR! Failed to load PMT anode gain matrix from \"" << fname << "\"!\n";
  return retval;
}

void nDetConstruction::SetGdmlFilename(const std::string &fname){ 
	gdmlFilename = fname;
	solid.clear();
	
	// Load the light-guide from a GDML file.
	LoadGDML(gdmlFilename, solid);
	fTrapezoidLength = solid.getLength()*mm;
}

void nDetConstruction::SetPositionCylindrical(const G4ThreeVector &position){ 
	double x = position.getX()*std::cos(position.getY()*deg);
	double z = position.getX()*std::sin(position.getY()*deg);
	detectorPosition = G4ThreeVector(x*cm, position.getZ()*cm, z*cm);
}

void nDetConstruction::SetPositionSpherical(const G4ThreeVector &position){ 
	double x = position.getX()*std::sin(position.getY()*deg)*std::cos(position.getZ()*deg); 
	double y = position.getX()*std::sin(position.getY()*deg)*std::sin(position.getZ()*deg); 
	double z = position.getX()*std::cos(position.getY()*deg);
	detectorPosition = G4ThreeVector(x*cm, y*cm, z*cm);
}

void nDetConstruction::SetRotation(const G4ThreeVector &rotation){
	detectorRotation.rotateX(rotation.getX()*deg);
	detectorRotation.rotateY(rotation.getY()*deg); 
	detectorRotation.rotateZ(rotation.getZ()*deg);  
}

void nDetConstruction::SetShadowBarSize(const G4ThreeVector &size){
	shadowBarSize = size;
}

void nDetConstruction::SetShadowBarPosition(const G4ThreeVector &pos){
	shadowBarPos = pos;		
}

bool nDetConstruction::SetShadowBarMaterial(const G4String &material){
	shadowBarMaterial = nist.searchForMaterial(material);
	if(!shadowBarMaterial)
		return false;
	return true;
}

void nDetConstruction::buildExpHall()
{
  // **************** expHalll **************
  //      ------------- Volumes --------------
  
  G4Box* expHall_solidV = new G4Box("expHall_solidV", expHallX, expHallY, expHallZ);

  expHall_logV  = new G4LogicalVolume(expHall_solidV, fAir, "expHall_logV", 0, 0, 0);
  expHall_logV->SetVisAttributes(G4VisAttributes::Invisible);

  expHall_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), expHall_logV, "expHall_physV",0,false,0);
 
  return;
} // end of buildExpHall function

//****************************************** Material Definitions  *********************************************//

void nDetConstruction::DefineMaterials() {
    G4cout<<"nDetConstruction::DefineMaterials()"<<G4endl;

	// Elements
	fH = nist.searchForElement("H");
	fC = nist.searchForElement("C");
	fO = nist.searchForElement("O");
	fF = nist.searchForElement("F");
	fSi = nist.searchForElement("Si");
	fAl = nist.searchForElement("Al");

	// Air
    fAir = nist.searchForMaterial("G4_AIR");

	// Lab vacuum
	fVacuum = nist.searchForMaterial("G4_Galactic");

    G4double density;
    int natoms;
    int ncomponents;

	/////////////////////////////////////////////////////////////////
	// Teflon (C2F4)n
	/////////////////////////////////////////////////////////////////

    fTeflon= new G4Material("Teflon", density=2.2*g/cm3,2);
    fTeflon->AddElement(fC,natoms=2);
    fTeflon->AddElement(fF,natoms=4);

    fTeflonMPT = new G4MaterialPropertiesTable();

    const G4int nEntries_Teflon = 2;

    G4double photonEnergy_teflon[nEntries_Teflon] = {2.*eV, 3.47*eV};
    G4double refl_teflon[nEntries_Teflon] = {0.99, 0.99};
    //G4double refl_teflon[nEntries_Teflon] = {0.5, 0.5};
    G4double effi_teflon[nEntries_Teflon] = {0, 0};
    G4double Absorption_Teflon[nEntries_Teflon] =  { 0.333 * cm ,0.333 * cm  };
    G4double refractiveIndex_teflon[nEntries_Teflon] = {1.315, 1.315};
    fTeflonMPT->AddProperty("REFLECTIVITY", photonEnergy_teflon, refl_teflon, nEntries_Teflon);
    fTeflonMPT->AddProperty("EFFICIENCY", photonEnergy_teflon, effi_teflon, nEntries_Teflon);
    fTeflonMPT->AddProperty("RINDEX", photonEnergy_teflon,refractiveIndex_teflon,nEntries_Teflon);
    fTeflonMPT->AddProperty("ABSLENGTH", photonEnergy_teflon,Absorption_Teflon,nEntries_Teflon);

    fTeflonMPT->DumpTable();

    fTeflon->SetMaterialPropertiesTable(fTeflonMPT);

	/////////////////////////////////////////////////////////////////
	// EJ200 (C9H10)n
	/////////////////////////////////////////////////////////////////

    fEJ200 = new G4Material("EJ200", 1.023*g/cm3, 2);
    fEJ200->AddElement(fH, 10);
    fEJ200->AddElement(fC, 9);

	G4double photonEnergy_Ej200[44] = {2.004*eV, 2.058*eV, 2.112*eV, 2.166*eV, 2.220*eV, 2.274*eV, 2.328*eV, 2.382*eV, 2.436*eV, 2.490*eV, 
		                               2.517*eV, 2.552*eV, 2.585*eV, 2.613*eV, 2.635*eV, 2.656*eV, 2.686*eV, 2.720*eV, 2.749*eV, 2.772*eV, 
		                               2.791*eV, 2.809*eV, 2.826*eV, 2.842*eV, 2.861*eV, 2.884*eV, 2.919*eV, 2.946*eV, 2.954*eV, 2.961*eV, 
		                               2.967*eV, 2.974*eV, 2.981*eV, 2.987*eV, 2.994*eV, 3.001*eV, 3.009*eV, 3.018*eV, 3.029*eV, 3.041*eV, 
		                               3.056*eV, 3.083*eV, 3.137*eV, 3.191*eV};

	G4double ScintilFast_EJ200[44] = {0.000, 0.001, 0.001, 0.002, 0.003, 0.006, 0.010, 0.018, 0.033, 0.060, 
		                              0.084, 0.122, 0.175, 0.234, 0.294, 0.356, 0.416, 0.473, 0.533, 0.594, 
		                              0.657, 0.720, 0.784, 0.846, 0.903, 0.962, 1.000, 0.917, 0.857, 0.798, 
		                              0.732, 0.669, 0.604, 0.542, 0.480, 0.422, 0.359, 0.297, 0.237, 0.170, 
		                              0.105, 0.028, 0.004, 0.000};
                                  
	G4double photonEnergy_Ej200_2[2] = {2.004*eV, 3.191*eV};
	G4double RefIndex_EJ200[2] = {1.580, 1.580};
	G4double Absorption_EJ200[2] = {400*cm, 400*cm};

    fEJ200MPT = new G4MaterialPropertiesTable();
    fEJ200MPT->AddProperty("RINDEX", photonEnergy_Ej200_2, RefIndex_EJ200, 2);
    fEJ200MPT->AddProperty("ABSLENGTH", photonEnergy_Ej200_2, Absorption_EJ200, 2);
    fEJ200MPT->AddProperty("FASTCOMPONENT", photonEnergy_Ej200, ScintilFast_EJ200, 44);

    //fEJ200MPT->AddConstProperty("SCINTILLATIONYIELD", 0.64*17400/MeV); // 64% of Anthracene
    fEJ200MPT->AddConstProperty("SCINTILLATIONYIELD", 10000/MeV); // Scintillation efficiency as per Eljen specs
    fEJ200MPT->AddConstProperty("RESOLUTIONSCALE", 1.0); // Intrinsic resolution

    //fEJ200MPT->AddConstProperty("RISETIMECONSTANT", 0.9*ns); Geant4 10.1 TODO
    fEJ200MPT->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.9*ns);
    fEJ200MPT->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);
    fEJ200MPT->AddConstProperty("YIELDRATIO",1);// the strength of the fast component as a function of total scintillation yield

    G4double pEF = fLightYieldScale; 
    G4double pSF = pEF * 1.35;

    //light yield - data taken form V.V. Verbinski et al, Nucl. Instrum. & Meth. 65 (1968) 8-25
	G4double particleEnergy[36] = {1E-3, 0.10*MeV, 0.13*MeV, 0.17*MeV, 0.20*MeV, 0.24*MeV, 0.30*MeV, 0.34*MeV, 0.40*MeV, 
		                           0.48*MeV, 0.60*MeV, 0.72*MeV, 0.84*MeV, 1.00*MeV, 1.30*MeV, 1.70*MeV, 2.00*MeV, 2.40*MeV, 
		                           3.00*MeV, 3.40*MeV, 4.00*MeV, 4.80*MeV, 6.00*MeV, 7.20*MeV, 8.40*MeV, 10.00*MeV, 11.00*MeV, 
		                           12.00*MeV, 13.00*MeV, 14.00*MeV, 15.00*MeV, 16.00*MeV, 17.00*MeV, 18.00*MeV, 19.00*MeV, 20.00*MeV};
		                           
	G4double electronYield[36] = {0.0E+00*pEF, 1.0E+03*pEF, 1.3E+03*pEF, 1.7E+03*pEF, 2.0E+03*pEF, 2.4E+03*pEF, 3.0E+03*pEF, 3.4E+03*pEF, 4.0E+03*pEF, 
		                          4.8E+03*pEF, 6.0E+03*pEF, 7.2E+03*pEF, 8.4E+03*pEF, 1.0E+04*pEF, 1.3E+04*pEF, 1.7E+04*pEF, 2.0E+04*pEF, 2.4E+04*pEF, 
		                          3.0E+04*pEF, 3.4E+04*pEF, 4.0E+04*pEF, 4.8E+04*pEF, 6.0E+04*pEF, 7.2E+04*pEF, 8.4E+04*pEF, 1.0E+05*pEF, 1.1E+05*pEF, 
		                          1.2E+05*pEF, 1.3E+05*pEF, 1.4E+05*pEF, 1.5E+05*pEF, 1.6E+05*pEF, 1.7E+05*pEF, 1.8E+05*pEF, 1.9E+05*pEF, 2.0E+05*pEF};

    fEJ200MPT->AddProperty("ELECTRONSCINTILLATIONYIELD", particleEnergy, electronYield, 36)->SetSpline(true);
	G4double protonYield[36] = {0.6*pSF, 67.1*pSF, 88.6*pSF, 120.7*pSF, 146.5*pSF, 183.8*pSF, 246.0*pSF, 290.0*pSF, 365.0*pSF, 
		                        483.0*pSF, 678.0*pSF, 910.0*pSF, 1175.0*pSF, 1562.0*pSF, 2385.0*pSF, 3660.0*pSF, 4725.0*pSF, 6250.0*pSF, 
		                        8660.0*pSF, 10420.0*pSF, 13270.0*pSF, 17180.0*pSF, 23100.0*pSF, 29500.0*pSF, 36200.0*pSF, 45500.0*pSF, 51826.7*pSF, 
		                        58313.7*pSF, 65047.2*pSF, 72027.4*pSF, 79254.2*pSF, 86727.6*pSF, 94447.6*pSF, 102414.2*pSF, 110627.4*pSF, 119087.2*pSF};

    fEJ200MPT->AddProperty("PROTONSCINTILLATIONYIELD", particleEnergy, protonYield, 36)->SetSpline(true);

	G4double ionYield[36] = {0.2*pEF, 10.4*pEF, 12.7*pEF, 15.7*pEF, 17.9*pEF, 20.8*pEF, 25.1*pEF, 27.9*pEF, 31.9*pEF, 
		                     36.8*pEF, 43.6*pEF, 50.2*pEF, 56.9*pEF, 65.7*pEF, 81.3*pEF, 101.6*pEF, 116.5*pEF, 136.3*pEF, 
		                     166.2*pEF, 187.1*pEF, 218.6*pEF, 260.5*pEF, 323.5*pEF, 387.5*pEF, 451.5*pEF, 539.9*pEF, 595.5*pEF, 
		                     651.8*pEF, 708.7*pEF, 766.2*pEF, 824.2*pEF, 882.9*pEF, 942.2*pEF, 1002.1*pEF, 1062.6*pEF, 1123.7*pEF}; 
                         
    fEJ200MPT->AddProperty("IONSCINTILLATIONYIELD", particleEnergy, ionYield, 36)->SetSpline(true);

    fEJ200->SetMaterialPropertiesTable(fEJ200MPT);

	/////////////////////////////////////////////////////////////////
	// Silicone Optical Grease (C2H6OSi)n
	/////////////////////////////////////////////////////////////////
	
    density = 1.06*g/cm3;
    fGrease = new G4Material("Grease",density,ncomponents=4);

    fGrease->AddElement(fC, natoms=2);
    fGrease->AddElement(fH, natoms=6);
    fGrease->AddElement(fO, natoms=1);
    fGrease->AddElement(fSi, natoms=1);

    const G4int nEntries_Grease = 5;
    G4double Photon_Energy[nEntries_Grease] = { 2.757*eV, 3.102*eV, 3.312*eV, 3.545*eV, 4.136*eV };
    G4double RefractiveIndex_grease[nEntries_Grease] = {1.468, 1.473, 1.477, 1.482, 1.496};
    G4double Absorption_grease[nEntries_Grease] = { 195*mm,  195*mm, 195*mm, 195*mm, 195*mm };
    fGreaseMPT=new G4MaterialPropertiesTable();

    fGreaseMPT->AddProperty("RINDEX", Photon_Energy, RefractiveIndex_grease, nEntries_Grease);
    fGreaseMPT->AddProperty("ABSLENGTH", Photon_Energy, Absorption_grease, nEntries_Grease);

    fGrease->SetMaterialPropertiesTable(fGreaseMPT);

    fSiO2 = nist.searchForMaterial("G4_SILICON_DIOXIDE");

    //optical properties of SiO2 - fused silica or fused quartz
    G4double PhotonEnergy[5] = { 2.484*eV, 2.615*eV, 2.760*eV, 2.922*eV, 3.105*eV };
    G4double RefractiveIndex_SiO2[5] = { 1.54, 1.54, 1.54, 1.54, 1.54 };
    G4double Absorption_SiO2[5] = {125.*cm, 123.5*cm, 122.*cm, 121.*cm, 120.*cm};

    fSiO2MPT = new G4MaterialPropertiesTable();
    fSiO2MPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex_SiO2, 5);
    fSiO2MPT->AddProperty("ABSLENGTH", PhotonEnergy, Absorption_SiO2, 5);
    fSiO2->SetMaterialPropertiesTable(fSiO2MPT);

    fSil = nist.searchForMaterial("G4_Si");

    // optical properties,    
    fSilMPT = new G4MaterialPropertiesTable();
    
    /*G4double RefractiveReal_Si[nEntries_Sil] = { 4.293, 4.453, 4.676, 5.008, 5.587 };
    G4double RefractiveImg_Si[nEntries_Sil] = { 0.045, 0.060, 0.091, 0.150, 0.303 };
    fSilMPT->AddProperty("REALRINDEX", PhotonEnergy, RefractiveReal_Si, nEntries_Sil);
    fSilMPT->AddProperty("IMAGINARYRINDEX", PhotonEnergy, RefractiveImg_Si, nEntries_Sil);*/
    
    // Ideal detector
    G4double EfficiencyIndex_Si[5] = { 1., 1., 1., 1., 1. };
    G4double Reflective_Si[5] = { 0., 0., 0., 0., 0.};
    
    // Non-ideal
    //G4double EfficiencyIndex_Si[nEntries_Sil] = { 0.37, 0.42, 0.39, 0.36, 0.32 };        
    //G4double Reflective_Si[nEntries_Sil] = { 0.49, 0.45, 0.42, 0.40, 0.39};

    fSilMPT->AddProperty("EFFICIENCY", PhotonEnergy, EfficiencyIndex_Si, 5);
    fSilMPT->AddProperty("REFLECTIVITY", PhotonEnergy, Reflective_Si, 5);

    fSil->SetMaterialPropertiesTable(fSilMPT);

	/////////////////////////////////////////////////////////////////
	// ACRYLIC (C5O2H8)n -- CRT
	/////////////////////////////////////////////////////////////////

	density = 1.19*g/cm3;
    fAcrylic = new G4Material("Acrylic", density, ncomponents=3);
    
    fAcrylic->AddElement(fC, natoms=5);
    fAcrylic->AddElement(fH, natoms=8);
    fAcrylic->AddElement(fO, natoms=2);

	// Photon energy (eV)
	G4double ENERGY_ACRYLIC[11] = {6.1992*eV, 4.95936*eV, 4.1328*eV, 3.5424*eV, 3.0996*eV, 2.7552*eV, 2.47968*eV, 2.25426*eV, 2.0664*eV, 1.90745*eV, 1.7712*eV};
	
	// Refractive index
	G4double RINDEX_ACRYLIC[11] = {1.57237, 1.54724, 1.5286, 1.51533, 1.50629, 1.50033, 1.49633, 1.49313, 1.4896, 1.48461, 1.47702};
	
	G4MaterialPropertiesTable *MPT_Acrylic = new G4MaterialPropertiesTable();
	MPT_Acrylic->AddProperty("RINDEX", ENERGY_ACRYLIC, RINDEX_ACRYLIC, 11);

	// Photon energy (eV)
	G4double energy2[25] = {12.3984*eV, 5.0292*eV, 4.75755*eV, 4.69897*eV, 4.66072*eV, 4.61377*eV, 4.56776*eV, 4.5316*eV, 4.48721*eV, 
	                        4.43507*eV, 4.13105*eV, 3.87258*eV, 3.64454*eV, 3.43671*eV, 3.25129*eV, 3.10158*eV, 2.94219*eV, 2.81212*eV, 
	                        2.69307*eV, 2.58078*eV, 2.47479*eV, 2.38212*eV, 2.29384*eV, 2.21614*eV, 1.7712*eV};
	                       
	// Acrylic absorption length (mm)
	G4double abslength[25] = {0, 0, 1.02102, 1.28345, 1.8604, 2.44271, 3.20801, 4.15751, 5.55214, 6.99127, 13.0334, 19.3961, 27.6547, 
	                          32.87, 34.1447, 35.5173, 34.1447, 32.87, 32.87, 34.1447, 34.1447, 35.5173, 35.5173, 34.1447, 33.7719};
	
	MPT_Acrylic->AddProperty("ABSLENGTH", energy2, abslength, 25);

	fAcrylic->SetMaterialPropertiesTable(MPT_Acrylic);

	/////////////////////////////////////////////////////////////////
	// Optical Surfaces
	/////////////////////////////////////////////////////////////////

	// Teflon	
    fTeflonOpticalSurface = new G4OpticalSurface("TeflonSurface", glisur, ground, dielectric_metal, 0.1); // polish level

    fTeflonOpticalSurface->SetFinish(ground);
    //fTeflonOpticalSurface->SetType(dielectric_metal);
    fTeflonOpticalSurface->SetType(dielectric_dielectric);
    fTeflonOpticalSurface->SetMaterialPropertiesTable(fTeflonMPT);

    // Silicon
    fSiliconPMOpticalSurface = new G4OpticalSurface("SiPMSurface");
    fSiliconPMOpticalSurface->SetType(dielectric_metal);
    fSiliconPMOpticalSurface->SetFinish(polished);
    fSiliconPMOpticalSurface->SetModel(glisur);
    fSiliconPMOpticalSurface->SetMaterialPropertiesTable(fSilMPT);

	// Aluminized mylar
    G4Material *Al = nist.searchForMaterial("G4_Al");
    G4Material *Mylar = nist.searchForMaterial("G4_MYLAR");

    fMylar=new G4Material("AluninizedMylar",density=1.39*g/cm3,ncomponents=2);
    fMylar->AddMaterial(Mylar,0.8);
    fMylar->AddMaterial(Al,0.2);

    G4double RefractiveReal_Mylar[5] = {0.81257,0.72122,0.63324,0.55571,0.48787};
    G4double RefractiveImg_Mylar[5] = {6.0481,5.7556,5.4544,5.1464,4.8355};

    fMylarMPT=new G4MaterialPropertiesTable();
    fMylarMPT->AddProperty("REALRINDEX", PhotonEnergy, RefractiveReal_Mylar, 5);
    fMylarMPT->AddProperty("IMAGINARYRINDEX", PhotonEnergy, RefractiveImg_Mylar, 5);

    fMylarOpticalSurface=new G4OpticalSurface("MylarSurface",glisur,polished,dielectric_metal,1.0);
    
    // ESR film (built in look-up-table)
    fEsrOpticalSurface = new G4OpticalSurface("EsrSurface");
    fEsrOpticalSurface->SetType(dielectric_LUT);
    fEsrOpticalSurface->SetModel(LUT);    
    //fEsrOpticalSurface->SetFinish(polishedvm2000air);
    fEsrOpticalSurface->SetFinish(polishedvm2000glue);

    return;
}

void nDetConstruction::ConstructSDandField(){
}

void nDetConstruction::GetSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const {
	if(copyNum > 0){ // Scintillator
		col = ((copyNum-1) / fNumRows) + 1;
		row = ((copyNum-1) % fNumRows) + 1;
	}
	else{ // Outer wrappings (mylar, teflon, etc) (id==0)
		col = 0;
		row = 0;
	}
}

bool nDetConstruction::AddDetectedPhoton(const G4Step *step, const double &mass/*=1*/){
	if(!step->GetPostStepPoint()){
		std::cout << " nDetConstruction: WARNING! INVALID POST POINT!\n";
		return false;
	}
	
	double energy = step->GetTrack()->GetTotalEnergy();
	double time = step->GetPostStepPoint()->GetGlobalTime();
	G4ThreeVector position = detectorRotation*(step->GetPostStepPoint()->GetPosition()-detectorPosition);
	
    if(position.z() > 0){
        if(center[0].addPoint(energy, time, position, mass))
            return true;
    }
    if(position.z() < 0){
        if(center[1].addPoint(energy, time, position, mass))
            return true;
    }
    return false;
}

void nDetConstruction::GetDetectedPhotons(size_t &numLeft, size_t &numRight){
	numLeft = center[0].getNumDetected();
	numRight = center[1].getNumDetected();
}

void nDetConstruction::Clear(){
    center[0].clear();
    center[1].clear();
}

void nDetConstruction::UpdateGeometry(){
    // clean-up previous geometry
    G4SolidStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    
    //define new one
    G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    G4RunManager::GetRunManager()->ReinitializeGeometry();

    if(PmtIsSegmented())
      setSegmentedPmt(fNumColumnsPmt, fNumRowsPmt, SiPM_dimension*2, SiPM_dimension*2);
}

G4LogicalVolume *nDetConstruction::LoadGDML(const G4String &fname, gdmlSolid &solid_, G4OpticalSurface *surface/*=NULL*/){
	G4LogicalVolume *retval = solid_.read(fname, surface);
	std::cout << " nDetConstruction: Loaded GDML model with size x=" << solid_.getWidth() << " mm, y=" << solid_.getThickness() << " mm, z=" << solid_.getLength() << " mm\n";
	return retval;
}

void nDetConstruction::buildModule(){
	//const G4double cellWidth = (fDetectorWidth-2*fNumColumns*fMylarThickness)/fNumColumns;
	//const G4double cellHeight = (fDetectorThickness-2*fNumRows*fMylarThickness)/fNumRows;
	const G4double cellWidth = (fDetectorWidth-(fNumColumns-1)*fMylarThickness)/fNumColumns;
	const G4double cellHeight = (fDetectorThickness-(fNumRows-1)*fMylarThickness)/fNumRows;

	// Construct the assembly bounding box.
	G4ThreeVector assemblyBoundingBox = constructAssembly();

    // Construct the scintillator cell
    G4Box *cellScint = new G4Box("scintillator", cellWidth/2, cellHeight/2, fDetectorLength/2);
    G4LogicalVolume *cellScint_logV = new G4LogicalVolume(cellScint, fEJ200, "scint_log");
    cellScint_logV->SetVisAttributes(scint_VisAtt);

	G4Box *mylarVertLayer = NULL;
	G4Box *mylarHorizLayer = NULL;
	
	G4LogicalVolume *mylarVertLayer_logV = NULL;
	G4LogicalVolume *mylarHorizLayer_logV = NULL;

	// Build the wrapping.
	G4PVPlacement *wrapping_physV = NULL;
	if(fMylarThickness > 0){
		G4double scintBoxWidth = fNumColumns*cellWidth+(fNumColumns-1)*fMylarThickness;
		G4double scintBoxHeight = fNumRows*cellHeight+(fNumRows-1)*fMylarThickness;
	
		// Construct the outer wrapping.
		G4Box *wrappingBox = new G4Box("wrappingBox", assemblyBoundingBox.getX()/2, assemblyBoundingBox.getY()/2, fDetectorLength/2);
		G4Box *scintBox = new G4Box("scintBox", scintBoxWidth/2, scintBoxHeight/2, assemblyBoundingBox.getZ()/2);
		
		G4SubtractionSolid *wrappingBody = new G4SubtractionSolid("wrapping", wrappingBox, scintBox);
		G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrappingBody, getUserSurfaceMaterial(), "wrapping_logV");
		wrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		// Place the outer wrapping into the assembly.
		wrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), wrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);
		
		// Construct vertical and horizontal reflector layers for later use.
		mylarVertLayer = new G4Box("mylarVertLayer", fMylarThickness/2, scintBoxHeight/2, fDetectorLength/2);
		mylarHorizLayer = new G4Box("mylarHorizLayer", cellWidth/2, fMylarThickness/2, fDetectorLength/2);

		mylarVertLayer_logV = new G4LogicalVolume(mylarVertLayer, getUserSurfaceMaterial(), "mylarVertLayer_logV");
		mylarHorizLayer_logV = new G4LogicalVolume(mylarHorizLayer, getUserSurfaceMaterial(), "mylarHorizLayer_logV");
		
		mylarVertLayer_logV->SetVisAttributes(wrapping_VisAtt);
		mylarHorizLayer_logV->SetVisAttributes(wrapping_VisAtt);
	}

	// Place the scintillator segments into the assembly.
	std::vector<G4PVPlacement*> mylarVertLayer_physV(fNumColumns, NULL);
	std::vector<std::vector<G4PVPlacement*> > mylarHorizLayer_physV(fNumColumns, std::vector<G4PVPlacement*>(fNumRows, NULL));
	std::vector<std::vector<G4PVPlacement*> > cellScint_physV(fNumColumns, std::vector<G4PVPlacement*>(fNumRows, NULL));	
	for(int col = 0; col < fNumColumns; col++){
		for(int row = 0; row < fNumRows; row++){
			//G4ThreeVector cellCenter(-fDetectorWidth/2 + (2*col+1)*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + (2*row+1)*fMylarThickness + (row+0.5)*cellHeight, 0);
			G4ThreeVector cellCenter(-fDetectorWidth/2 + col*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + row*fMylarThickness + (row+0.5)*cellHeight, 0);

			// Copy numbers (segment IDs), indexed from 1
			std::stringstream stream; stream << "Scint-" << col << "," << row;
			cellScint_physV[col][row] = new G4PVPlacement(0, cellCenter, cellScint_logV, stream.str().c_str(), assembly_logV, 0, col*fNumRows+row+1, fCheckOverlaps); 
		
			// Place vertical and horizontal reflectors.
			if(fMylarThickness > 0){ 
				if(row == 0 && col != fNumColumns-1){ // New vertical reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col;
					mylarVertLayer_physV[col] = new G4PVPlacement(0, G4ThreeVector(cellCenter.getX()+cellWidth/2+fMylarThickness/2, 0, 0), mylarVertLayer_logV, stream2.str().c_str(), assembly_logV, 0, 0, fCheckOverlaps);
				}
				if(row != fNumRows-1){ // New horizontal reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col << "," << row;
					mylarHorizLayer_physV[col][row] = new G4PVPlacement(0, G4ThreeVector(cellCenter.getX(), cellCenter.getY()+cellHeight/2+fMylarThickness/2, 0), mylarHorizLayer_logV, stream2.str().c_str(), assembly_logV, 0, 0, fCheckOverlaps);
				}
			}
		}
	}

	// Define logical reflector surfaces.
	if(fMylarThickness > 0){ 
		for(int col = 0; col < fNumColumns; col++){
			for(int row = 0; row < fNumRows; row++){
				G4PVPlacement *cellPhysical = cellScint_physV[col][row];
				
				int leftCol = col-1;
				int rightCol = col+1;
				int downRow = row-1;
				int upRow = row+1;
					
				// Border with the outer wrapping.
				if((col == 0 || row == 0) || (col == fNumColumns-1 || row == fNumRows-1)) 
					new G4LogicalBorderSurface("Wrapping", cellPhysical, wrapping_physV, getUserOpticalSurface());
				
				// Internal reflector layers.
				if(leftCol >= 0 && leftCol < fNumColumns) // Left side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarVertLayer_physV.at(col-1), getUserOpticalSurface());
				if(rightCol >= 0 && rightCol < fNumColumns) // Right side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarVertLayer_physV.at(col), getUserOpticalSurface());
				if(downRow >= 0 && downRow < fNumRows) // Bottom side horizontal layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarHorizLayer_physV.at(col).at(row-1), getUserOpticalSurface());
				if(upRow >= 0 && upRow < fNumRows) // Top side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarHorizLayer_physV.at(col).at(row), getUserOpticalSurface());
			}
		}
	}

	// Place light diffusers (BEFORE LIGHT-GUIDE).
	/*if(fDiffuserLength > 0){
		applyGreaseLayer();
		applyDiffuserLayer();
	}*/

	// Apply trapezoidal light-guides.
	if(fTrapezoidLength > 0){
		applyGreaseLayer();
		applyLightGuide();
	}

	// Place light diffusers (AFTER LIGHT-GUIDE).
	if(fDiffuserLength > 0){
		applyGreaseLayer();
		applyDiffuserLayer();
	}

	// Build the two PSPmts
	constructPSPmts();
	
    // Full detector physical volume
    assembly_physV = new G4PVPlacement(&detectorRotation, detectorPosition, assembly_logV, "Assembly", expHall_logV, 0, 0, true);//fCheckOverlaps);
}

void nDetConstruction::buildEllipse(){
	const G4double angle = 60*deg;

	// Width of the detector (defined by the trapezoid length and SiPM dimensions).
	fDetectorWidth = 2*(SiPM_dimension+(fTrapezoidLength/std::tan(angle)))*mm;
	G4double deltaWidth = fMylarThickness/std::sin(angle);

	// Construct the assembly bounding box.
	constructAssembly();

	// Create the geometry.
	G4Trd *innerTrapezoid = new G4Trd("innerTrapezoid", fDetectorWidth/2, SiPM_dimension, fDetectorThickness/2, SiPM_dimension, fTrapezoidLength/2);
	G4Trd *outerTrapezoid = new G4Trd("outerTrapezoid", fDetectorWidth/2+deltaWidth, SiPM_dimension+deltaWidth, fDetectorThickness/2+deltaWidth, SiPM_dimension+deltaWidth, fTrapezoidLength/2);
	G4Box *innerBody = new G4Box("innerBody", fDetectorWidth/2, fDetectorThickness/2, fDetectorLength/2);
	G4Box *outerBody = new G4Box("outerBody", fDetectorWidth/2+deltaWidth, fDetectorThickness/2+deltaWidth, fDetectorLength/2);

	// Build the detector body using unions.
	G4RotationMatrix *trapRot = new G4RotationMatrix;
	trapRot->rotateY(180*deg);
	G4UnionSolid *scintBody1 = new G4UnionSolid("", innerBody, innerTrapezoid, 0, G4ThreeVector(0, 0, +fDetectorLength/2+fTrapezoidLength/2));
	G4UnionSolid *scintBody2 = new G4UnionSolid("scint", scintBody1, innerTrapezoid, trapRot, G4ThreeVector(0, 0, -fDetectorLength/2-fTrapezoidLength/2));
	G4LogicalVolume *scint_logV = new G4LogicalVolume(scintBody2, fEJ200, "scint_logV");	
    scint_logV->SetVisAttributes(scint_VisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *ellipseBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), scint_logV, "Scint", assembly_logV, true, 0, fCheckOverlaps);

	// Build the wrapping.
	G4PVPlacement *ellipseWrapping_physV = NULL;
	if(fMylarThickness > 0){
		G4UnionSolid *wrappingBody1 = new G4UnionSolid("", outerBody, outerTrapezoid, 0, G4ThreeVector(0, 0, +fDetectorLength/2+fTrapezoidLength/2));
		G4UnionSolid *wrappingBody2 = new G4UnionSolid("", wrappingBody1, outerTrapezoid, trapRot, G4ThreeVector(0, 0, -fDetectorLength/2-fTrapezoidLength/2));
		G4SubtractionSolid *wrappingBody3 = new G4SubtractionSolid("wrapping", wrappingBody2, scintBody2);
		G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrappingBody3, getUserSurfaceMaterial(), "wrapping_logV");		
		wrapping_logV->SetVisAttributes(wrapping_VisAtt);

		// Place the wrapping around the scintillator.
		ellipseWrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), wrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);	
	}

	// Account for the trapezoids.
	currentOffsetZ += fTrapezoidLength;
	
	// Directly modify the size of the grease layer.
	currentLayerSizeX = 2*SiPM_dimension;

	// Attach PMTs.
	constructPSPmts();
	
    // Full detector physical volume
    assembly_physV = new G4PVPlacement(&detectorRotation, detectorPosition, assembly_logV, "Assembly", expHall_logV, 0, 0, true);//fCheckOverlaps);

    // Reflective wrapping.
    if(fMylarThickness > 0)
	    new G4LogicalBorderSurface("Wrapping", ellipseBody_physV, ellipseWrapping_physV, getUserOpticalSurface());
}

void nDetConstruction::buildRectangle(){
	// Construct the assembly bounding box.
	G4ThreeVector assemblyBoundingBox = constructAssembly();

    G4Box *plateBody = new G4Box("", fDetectorWidth/2, fDetectorThickness/2, fDetectorLength/2);
    G4LogicalVolume *plateBody_logV = new G4LogicalVolume(plateBody, fEJ200, "plateBody_logV");
    plateBody_logV->SetVisAttributes(scint_VisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *plateBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), plateBody_logV, "Scint", assembly_logV, true, 0, fCheckOverlaps);

	// Build the wrapping.
	G4PVPlacement *plateWrapping_physV = NULL;
	if(fMylarThickness > 0){
		G4ThreeVector pmtSize = getPSPmtBoundingBox();
		G4Box *pmtBoundingBox = new G4Box("", pmtSize.getX()/2, pmtSize.getY()/2, pmtSize.getZ()/2);
		G4Box *plateWrappingBox = new G4Box("", fDetectorWidth/2 + fMylarThickness, fDetectorThickness/2 + fMylarThickness, fDetectorLength/2 + fMylarThickness);
		G4UnionSolid *cookieCutter = new G4UnionSolid("", plateBody, pmtBoundingBox, 0, G4ThreeVector(0, 0, fDetectorLength/2+pmtSize.getZ()/2));
		cookieCutter = new G4UnionSolid("", cookieCutter, pmtBoundingBox, 0, G4ThreeVector(0, 0, -fDetectorLength/2-pmtSize.getZ()/2));
		
		G4SubtractionSolid *plateWrapping = new G4SubtractionSolid("", plateWrappingBox, cookieCutter);
		G4LogicalVolume *plateWrapping_logV = new G4LogicalVolume(plateWrapping, getUserSurfaceMaterial(), "plateWrapping_logV");
		plateWrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		// Place the wrapping around the scintillator.
		plateWrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), plateWrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);		
	}

	// Attach PMTs.
	constructPSPmts();

    // Full detector physical volume
    assembly_physV = new G4PVPlacement(&detectorRotation, detectorPosition, assembly_logV, "Assembly", expHall_logV, 0, 0, true);//fCheckOverlaps);
    
    // Reflective wrapping.
    if(fMylarThickness > 0)
	    new G4LogicalBorderSurface("Wrapping", plateBody_physV, plateWrapping_physV, getUserOpticalSurface());
}

void nDetConstruction::buildTestAssembly(){
	if(gdmlFilename.empty()) return;

	// Load the testSolid.
    gdmlSolid testSolid;
    LoadGDML(gdmlFilename, testSolid, fEsrOpticalSurface);
    
	G4RotationMatrix *trapRot = new G4RotationMatrix;
    trapRot->rotateX(gdmlRotation.getX()*deg);
    trapRot->rotateY(gdmlRotation.getY()*deg);
    trapRot->rotateZ(gdmlRotation.getZ()*deg);
    
    // Place the model at the origin.
	std::vector<G4PVPlacement*> trapPhysical;
    testSolid.placeSolid(trapRot, G4ThreeVector(0, 0, 0), expHall_logV, trapPhysical);

    testSolid.setLogicalBorders("ESR", fEsrOpticalSurface, trapPhysical); // This works as expected.
}

G4ThreeVector nDetConstruction::constructAssembly(){
	G4ThreeVector pmtBoundingBox = getPSPmtBoundingBox();
	G4double assemblyLength = fDetectorLength + 2*(pmtBoundingBox.getZ());
	G4double assemblyWidth = fDetectorWidth + 2*fMylarThickness;
	G4double assemblyThickness = fDetectorThickness + 2*fMylarThickness;

	assemblyWidth = std::max(assemblyWidth, pmtBoundingBox.getX());
	assemblyThickness = std::max(assemblyThickness, pmtBoundingBox.getY());

	G4Box *assembly = new G4Box("assembly", assemblyWidth/2, assemblyThickness/2, assemblyLength/2);
    assembly_logV = new G4LogicalVolume(assembly, fAir, "assembly_logV");
    assembly_logV->SetVisAttributes(assembly_VisAtt);

	currentLayerSizeX = fDetectorWidth;
	currentLayerSizeY = fDetectorThickness;
	currentOffsetZ = fDetectorLength/2;

	return G4ThreeVector(assemblyWidth, assemblyThickness, assemblyLength);
}

void nDetConstruction::constructPSPmts(){
	// Build the sensitive PMT surfaces.
	const G4String name = "psSiPM";

	G4double windowZ = currentOffsetZ + fGreaseThickness + fWindowThickness/2;
	G4double sensitiveZ = currentOffsetZ + fGreaseThickness + fWindowThickness + fSensitiveThickness/2;
	
	// The optical grease layers.
	applyGreaseLayer(); 

	if(fWindowThickness > 0){ // The quartz window
		G4Box* window_solidV = new G4Box("window_solidV", SiPM_dimension, SiPM_dimension, fWindowThickness/2);
		G4LogicalVolume *window_logV = new G4LogicalVolume(window_solidV, fSiO2, "window_logV");
		
		window_logV->SetVisAttributes(window_VisAtt);
	
		new G4PVPlacement(0, G4ThreeVector(0, 0, windowZ), window_logV, "Quartz", assembly_logV, true, 0, fCheckOverlaps);
		new G4PVPlacement(0, G4ThreeVector(0, 0, -windowZ), window_logV, "Quartz", assembly_logV, true, 0, fCheckOverlaps);
	}
	
    // The photon sensitive surface
    G4Box *sensitive_solidV = new G4Box(name+"_solidV", SiPM_dimension, SiPM_dimension, fSensitiveThickness/2);
    G4LogicalVolume *sensitive_logV = new G4LogicalVolume(sensitive_solidV, fSil, name+"_logV");
    sensitive_logV->SetVisAttributes(sensitive_VisAtt);
    
    // Logical skin surface.
    new G4LogicalSkinSurface(name, sensitive_logV, fSiliconPMOpticalSurface);    
    
    new G4PVPlacement(0, G4ThreeVector(0, 0, sensitiveZ), sensitive_logV, name, assembly_logV, true, 0, fCheckOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0, 0, -sensitiveZ), sensitive_logV, name, assembly_logV, true, 0, fCheckOverlaps);
    
    // Move the current offset past the PMT
	currentLayerSizeX = 2*SiPM_dimension;
	currentLayerSizeY = 2*SiPM_dimension;
    currentOffsetZ += fGreaseThickness + fWindowThickness + fSensitiveThickness;
}

void nDetConstruction::applyGreaseLayer(){
	this->applyGreaseLayer(currentLayerSizeX, currentLayerSizeY);
}

void nDetConstruction::applyGreaseLayer(const G4double &x, const G4double &y){
    if(fGreaseThickness > 0){
	    G4Box *grease_solidV = new G4Box("grease", x/2, y/2, fGreaseThickness/2);
	    grease_logV = new G4LogicalVolume(grease_solidV, fGrease, "grease_logV");
	    grease_logV->SetVisAttributes(grease_VisAtt);
	
	    new G4PVPlacement(0, G4ThreeVector(0, 0, currentOffsetZ+fGreaseThickness/2), grease_logV, "Grease", assembly_logV, true, 0, fCheckOverlaps);
	    new G4PVPlacement(0, G4ThreeVector(0, 0, -currentOffsetZ-fGreaseThickness/2), grease_logV, "Grease", assembly_logV, true, 0, fCheckOverlaps);
		currentLayerSizeX = x;
		currentLayerSizeY = y;
		currentOffsetZ += fGreaseThickness;
	}
}

void nDetConstruction::applyDiffuserLayer(){
	this->applyDiffuserLayer(currentLayerSizeX, currentLayerSizeY);
}

void nDetConstruction::applyDiffuserLayer(const G4double &x, const G4double &y){
    if(fDiffuserLength > 0){ // Build the light diffusers (if needed)
        G4Box *lightDiffuser = new G4Box("lightDiffuser", x/2, y/2, fDiffuserLength/2);
        G4LogicalVolume *lightDiffuserLog = new G4LogicalVolume(lightDiffuser, fSiO2, "lightDiffuser_logV");

        new G4PVPlacement(0, G4ThreeVector(0, 0, currentOffsetZ+fDiffuserLength/2), lightDiffuserLog, "Diffuser", assembly_logV, true, 0, fCheckOverlaps);
        new G4PVPlacement(0, G4ThreeVector(0, 0, -currentOffsetZ-fDiffuserLength/2), lightDiffuserLog, "Diffuser", assembly_logV, true, 0, fCheckOverlaps);
        
    	// Offset the other layers to account for the light-diffuser and another layer of grease.
		currentLayerSizeX = x;
		currentLayerSizeY = y;
    	currentOffsetZ += fDiffuserLength;
    }
}

void nDetConstruction::applyLightGuide(){
	this->applyLightGuide(currentLayerSizeX, 2*SiPM_dimension, currentLayerSizeY, 2*SiPM_dimension);
}

void nDetConstruction::applyLightGuide(const G4double &x2, const G4double &y2){
	this->applyLightGuide(currentLayerSizeX, x2, currentLayerSizeY, y2);
}

void nDetConstruction::applyLightGuide(const G4double &x1, const G4double &x2, const G4double &y1, const G4double &y2){
    if(fTrapezoidLength > 0 || solid.isLoaded()){ // Build the light guides (if needed)
        const G4double trapAngleXZ = std::atan2(2*fTrapezoidLength, x1-x2);
        const G4double trapAngleYZ = std::atan2(2*fTrapezoidLength, y1-y2);
        
        const G4double deltaX = fMylarThickness/std::sin(trapAngleXZ);
        const G4double deltaY = fMylarThickness/std::sin(trapAngleYZ);
        
        G4double trapezoidZ = currentOffsetZ + fTrapezoidLength/2;
        
        std::string trapName = "Acrylic";
        
        G4RotationMatrix *trapRot[2] = {new G4RotationMatrix, new G4RotationMatrix};
        
        // Build the light-guide.
        if(!solid.isLoaded()){ // Use a standard G4Trd as the light-guide (default).
		    G4Trd *lightGuide = new G4Trd("lightGuide", x1/2, x2/2, y1/2, y2/2, fTrapezoidLength/2);
		    G4LogicalVolume *lightGuideLog = new G4LogicalVolume(lightGuide, fSiO2, "lightGuide_logV");	

			trapRot[1]->rotateY(180*deg);

			// Place the light guides.
			G4PVPlacement *trapPhysicalL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), lightGuideLog, trapName, assembly_logV, true, 0, fCheckOverlaps);
			G4PVPlacement *trapPhysicalR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), lightGuideLog, trapName, assembly_logV, true, 0, fCheckOverlaps);

			// Build the wrapping.
			if(fMylarThickness > 0){
				G4Trd *wrappingSolid = new G4Trd("wrapping", x1/2+deltaX, x2/2+deltaY, y1/2+deltaX, y2/2+deltaY, fTrapezoidLength/2);
				G4SubtractionSolid *wrapping = new G4SubtractionSolid("wrapping", wrappingSolid, lightGuide);
				G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrapping, getUserSurfaceMaterial(), "wrapping_logV");		
				wrapping_logV->SetVisAttributes(wrapping_VisAtt);

				// Place the wrapping around the light guides.
				G4PVPlacement *trapWrappingL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), wrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);
				G4PVPlacement *trapWrappingR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), wrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);
			
				// Reflective wrapping.
				new G4LogicalBorderSurface("Wrapping", trapPhysicalL, trapWrappingL, getUserOpticalSurface());
				new G4LogicalBorderSurface("Wrapping", trapPhysicalR, trapWrappingR, getUserOpticalSurface());
			}
        }
        else{ // Load the light-guide from a GDML file.
	    	//G4LogicalVolume *lightGuideLog = solid.getLogicalVolume();
			trapName = solid.getName();

			// Set the dimensions of the light-guide.
		    G4Trd *lightGuide = new G4Trd("lightGuide", solid.getWidth()/2, x2/2, solid.getThickness()/2, y2/2, fTrapezoidLength/2);

			trapRot[0]->rotateX(gdmlRotation.getX()*deg);
			trapRot[0]->rotateY(gdmlRotation.getY()*deg);
			trapRot[0]->rotateZ(gdmlRotation.getZ()*deg);

			trapRot[1]->rotateX(gdmlRotation.getX()*deg);
			trapRot[1]->rotateY(gdmlRotation.getY()*deg);
			trapRot[1]->rotateZ(gdmlRotation.getZ()*deg);

		    std::vector<G4PVPlacement*> trapPhysicalL;
			std::vector<G4PVPlacement*> trapPhysicalR;
	    	solid.placeSolid(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), assembly_logV, trapPhysicalL);
	    	solid.placeSolid(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), assembly_logV, trapPhysicalR);
			solid.setLogicalBorders("Wrapping", getUserOpticalSurface(), trapPhysicalL);
			solid.setLogicalBorders("Wrapping", getUserOpticalSurface(), trapPhysicalR);	 

			// Build the wrapping.
			if(fMylarThickness > 0){
				G4Trd *wrappingSolid = new G4Trd("wrapping", solid.getWidth()/2+deltaX, x2/2+deltaY, solid.getThickness()/2+deltaX, y2/2+deltaY, fTrapezoidLength/2);
				G4SubtractionSolid *wrapping = new G4SubtractionSolid("wrapping", wrappingSolid, lightGuide);
				G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrapping, getUserSurfaceMaterial(), "wrapping_logV");		
				wrapping_logV->SetVisAttributes(wrapping_VisAtt);

				// Place the wrapping around the light guides.
				G4PVPlacement *trapWrappingL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), wrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);
				G4PVPlacement *trapWrappingR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), wrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);
			
				// Reflective wrapping.
    	    	for(std::vector<G4PVPlacement*>::iterator iter = trapPhysicalL.begin(); iter != trapPhysicalL.end(); iter++){
    	    		new G4LogicalBorderSurface("Wrapping", (*iter), trapWrappingL, getUserOpticalSurface());
    	    	}
				for(std::vector<G4PVPlacement*>::iterator iter = trapPhysicalR.begin(); iter != trapPhysicalR.end(); iter++){
	    			new G4LogicalBorderSurface("Wrapping", (*iter), trapWrappingR, getUserOpticalSurface());
	    		}
			}
	    }

        /*G4double trapezoidZ = fDetectorLength/2 + fGreaseThickness + fTrapezoidLength/2;
        if(fDiffuserLength > 0)
			trapezoidZ += fGreaseThickness + fDiffuserLength;
        
        std::vector<G4PVPlacement*> trapPhysicalL;
		std::vector<G4PVPlacement*> trapPhysicalR;
        
        if(!solid.isLoaded()){
    		trapPhysicalL.push_back(new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), lightGuideLog, trapName, assembly_logV, true, 0, fCheckOverlaps));
    		trapPhysicalR.push_back(new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), lightGuideLog, trapName, assembly_logV, true, 0, fCheckOverlaps));
    	}
    	else{
	    	solid.placeSolid(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), assembly_logV, trapPhysicalL);
	    	solid.placeSolid(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), assembly_logV, trapPhysicalR);
			solid.setLogicalBorders("ESR", fEsrOpticalSurface, trapPhysicalL);
			solid.setLogicalBorders("ESR", fEsrOpticalSurface, trapPhysicalR);	    	
	    }*/
    	
    	/*if(fMylarThickness > 0){ // Construct the mylar cover flaps
    		
    	
    	    G4double topFlapOffset = trapezoidW2/2-SiPM_dimension;
    	    G4double sideFlapOffset = trapezoidW1/2-SiPM_dimension;
    	
    	    // Top and bottom
    	    G4double zprime = std::sqrt(std::pow(topFlapOffset, 2.0) + std::pow(fTrapezoidLength, 2.0));
    	    std::vector<G4TwoVector> trapCoverPoints;
    	    trapCoverPoints.push_back(G4TwoVector(-trapezoidW2/2, zprime/2));
    	    trapCoverPoints.push_back(G4TwoVector(trapezoidW2/2, zprime/2));
    	    trapCoverPoints.push_back(G4TwoVector(SiPM_dimension, -zprime/2));
    	    trapCoverPoints.push_back(G4TwoVector(-SiPM_dimension, -zprime/2));

    	    G4ExtrudedSolid *trapMylarCover = new G4ExtrudedSolid("", trapCoverPoints, fMylarThickness/2, G4TwoVector(0, 0), 1, G4TwoVector(0, 0), 1);
    	    G4LogicalVolume *trapMylarCover_logV = new G4LogicalVolume(trapMylarCover, fMylar, "trapMylarCover_logV");
    	    trapMylarCover_logV->SetVisAttributes(wrapping_VisAtt);

    	    // Sides
	        zprime = std::sqrt(std::pow(sideFlapOffset, 2.0) + std::pow(fTrapezoidLength, 2.0));
    	    std::vector<G4TwoVector> sideTrapCoverPoints;
    	    sideTrapCoverPoints.push_back(G4TwoVector(-trapezoidW2/2, zprime/2));
    	    sideTrapCoverPoints.push_back(G4TwoVector(trapezoidW2/2, zprime/2));
    	    sideTrapCoverPoints.push_back(G4TwoVector(SiPM_dimension, -zprime/2));
    	    sideTrapCoverPoints.push_back(G4TwoVector(-SiPM_dimension, -zprime/2));
    	    
    	    G4ExtrudedSolid *sideTrapMylarCover = new G4ExtrudedSolid("", sideTrapCoverPoints, fMylarThickness/2, G4TwoVector(0, 0), 1, G4TwoVector(0, 0), 1);
    	    G4LogicalVolume *sideTrapMylarCover_logV = new G4LogicalVolume(sideTrapMylarCover, fMylar, "sideTrapMylarCover_logV");
    	    sideTrapMylarCover_logV->SetVisAttributes(wrapping_VisAtt);
    	    
    	    // Handle polygon rotation
    	    G4RotationMatrix *trapCoverRotL[4] = {new G4RotationMatrix(), new G4RotationMatrix(), new G4RotationMatrix(), new G4RotationMatrix()};
            G4RotationMatrix *trapCoverRotR[4] = {new G4RotationMatrix(), new G4RotationMatrix(), new G4RotationMatrix(), new G4RotationMatrix()};	       	    
    	    
    	    G4double trapAngle = std::atan2(fTrapezoidLength, topFlapOffset);

            trapCoverRotL[0]->rotateX(trapAngle);
            
            trapCoverRotR[0]->rotateY(pi);
            trapCoverRotR[0]->rotateX(trapAngle);
            
            trapCoverRotL[1]->rotateX(pi-trapAngle);
            
            trapCoverRotR[1]->rotateY(pi);
            trapCoverRotR[1]->rotateX(pi-trapAngle);
            
            trapAngle = std::atan2(fTrapezoidLength, sideFlapOffset);
            
            trapCoverRotL[2]->rotateY(trapAngle);
            trapCoverRotL[2]->rotateZ(270*deg);
            
            trapCoverRotR[2]->rotateX(pi);
            trapCoverRotR[2]->rotateY(trapAngle);
            trapCoverRotR[2]->rotateZ(270*deg);
            
            trapCoverRotL[3]->rotateY(pi-trapAngle);
            trapCoverRotL[3]->rotateZ(270*deg);

            trapCoverRotR[3]->rotateX(pi);
            trapCoverRotR[3]->rotateY(pi-trapAngle);
            trapCoverRotR[3]->rotateZ(270*deg);
    	    
    	    // Place the mylar covers
    	    zprime = 0.5*fMylarThickness/std::sqrt(1+std::pow(std::tan(trapAngle), 2.0));
    	    G4double hprime = zprime*std::tan(trapAngle);
    	    
    	    G4double verticalOffset = 0.5*(topFlapOffset) + SiPM_dimension + hprime;
    	    G4double horizontalOffset = 0.5*(sideFlapOffset) + SiPM_dimension + hprime;
    	    trapezoidZ += zprime;
    	    
    	    G4PVPlacement *cellMylarWrap[4];
    	    
    	    cellMylarWrap[0] = new G4PVPlacement(trapCoverRotL[0], G4ThreeVector(0, verticalOffset, trapezoidZ), trapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    cellMylarWrap[1] = new G4PVPlacement(trapCoverRotL[1], G4ThreeVector(0, -verticalOffset, trapezoidZ), trapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    cellMylarWrap[2] = new G4PVPlacement(trapCoverRotL[2], G4ThreeVector(-horizontalOffset, 0, trapezoidZ), sideTrapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    cellMylarWrap[3] = new G4PVPlacement(trapCoverRotL[3], G4ThreeVector(horizontalOffset, 0, trapezoidZ), sideTrapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    for(size_t i = 0; i < 4; i++){
    	    	for(std::vector<G4PVPlacement*>::iterator iter = trapPhysicalL.begin(); iter != trapPhysicalL.end(); iter++){
    	    		new G4LogicalBorderSurface("Mylar", (*iter), cellMylarWrap[i], fMylarOpticalSurface);
    	    	}
    	    }

            // Now for the other side
    	    cellMylarWrap[0] = new G4PVPlacement(trapCoverRotR[0], G4ThreeVector(0, verticalOffset, -trapezoidZ), trapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    cellMylarWrap[1] = new G4PVPlacement(trapCoverRotR[1], G4ThreeVector(0, -verticalOffset, -trapezoidZ), trapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    cellMylarWrap[2] = new G4PVPlacement(trapCoverRotR[2], G4ThreeVector(-horizontalOffset, 0, -trapezoidZ), sideTrapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    cellMylarWrap[3] = new G4PVPlacement(trapCoverRotR[3], G4ThreeVector(horizontalOffset, 0, -trapezoidZ), sideTrapMylarCover_logV, "Mylar", assembly_logV, true, 0, fCheckOverlaps);
    	    for(size_t i = 0; i < 4; i++){
    	    	for(std::vector<G4PVPlacement*>::iterator iter = trapPhysicalR.begin(); iter != trapPhysicalR.end(); iter++){
    	    		new G4LogicalBorderSurface("Mylar", (*iter), cellMylarWrap[i], fMylarOpticalSurface);
    	    	}
    	    }
    	}*/
    	
		currentLayerSizeX = x2;
		currentLayerSizeY = y2;
    	currentOffsetZ += fTrapezoidLength;
	}
}

G4ThreeVector nDetConstruction::getPSPmtBoundingBox(){
	G4double boundingX = std::max(2*SiPM_dimension, fDetectorWidth);
	G4double boundingY = std::max(2*SiPM_dimension, fDetectorThickness);
	G4double boundingZ = fGreaseThickness + fWindowThickness + fSensitiveThickness;

	if(fDiffuserLength > 0) // Account for light diffusers
	    boundingZ += fDiffuserLength + fGreaseThickness;
	if(fTrapezoidLength > 0) // Account for light guides
	    boundingZ += fTrapezoidLength + fGreaseThickness;

	if(solid.isLoaded()){
		boundingX = std::max(boundingX, solid.getWidth());
		boundingY = std::max(boundingY, solid.getThickness());
	}

	return G4ThreeVector(boundingX, boundingY, boundingZ);
}

G4Material *nDetConstruction::getUserSurfaceMaterial(){
    if(wrappingMaterial == "mylar")
    	return fMylar;
    else if(wrappingMaterial == "teflon")
    	return fTeflon;
    else if(wrappingMaterial == "esr")
    	return fMylar; //fEsr; CRT!!!
    else if(wrappingMaterial == "silicon")
    	return fSil;
    
    return fMylar; // default
}

G4OpticalSurface *nDetConstruction::getUserOpticalSurface(){
    if(wrappingMaterial == "mylar")
    	return fMylarOpticalSurface;
    else if(wrappingMaterial == "teflon")
    	return fTeflonOpticalSurface;
    else if(wrappingMaterial == "esr")
    	return fEsrOpticalSurface;
    else if(wrappingMaterial == "silicon")
    	return fSiliconPMOpticalSurface;
    
    return fMylarOpticalSurface; // default
}
