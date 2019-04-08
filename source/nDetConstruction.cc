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
#include "G4Sphere.hh"

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
    fPolishedInterface = true;
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

	// Load all user-defined layers.
	/*while(!userLayers.empty()){
		userLayers.front().execute(this);
		userLayers.pop_front();
	}*/
	// Load all user-defined layers.
	for(std::deque<userAddLayer>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		iter->execute(this);
	}	

	// Attach PMTs.
	constructPSPmts();

    // Full detector physical volume
    assembly_physV = new G4PVPlacement(&detectorRotation, detectorPosition, assembly_logV, "Assembly", expHall_logV, 0, 0, fCheckOverlaps);

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

    G4double photonEnergy_teflon[9] = {1.607*eV, 1.743*eV, 1.908*eV, 2.108*eV, 2.354*eV, 2.664*eV, 3.070*eV, 3.621*eV, 4.413*eV};
    G4double reflectivity_teflon[9] = {0.514, 0.583, 0.656, 0.727, 0.789, 0.836, 0.868, 0.887, 0.892}; // https://www.osti.gov/servlets/purl/1184400 (1 layer)
    G4double efficiency_teflon[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    G4double absorption_teflon[9] =  {0.333*cm, 0.333*cm, 0.333*cm, 0.333*cm, 0.333*cm, 0.333*cm, 0.333*cm, 0.333*cm, 0.333*cm};
    G4double refIndex_teflon[9] = {1.315, 1.315, 1.315, 1.315, 1.315, 1.315, 1.315, 1.315, 1.315};
    
    fTeflonMPT = new G4MaterialPropertiesTable();
    fTeflonMPT->AddProperty("REFLECTIVITY", photonEnergy_teflon, reflectivity_teflon, 9);
    fTeflonMPT->AddProperty("EFFICIENCY", photonEnergy_teflon, efficiency_teflon, 9);
    fTeflonMPT->AddProperty("RINDEX", photonEnergy_teflon, refIndex_teflon, 9);
    fTeflonMPT->AddProperty("ABSLENGTH", photonEnergy_teflon, absorption_teflon, 9);
    fTeflon->SetMaterialPropertiesTable(fTeflonMPT);

	/////////////////////////////////////////////////////////////////
	// EJ200 N(H)=52.4%, N(C)=47.6%
	/////////////////////////////////////////////////////////////////

    fEJ200 = new G4Material("EJ200", 1.023*g/cm3, 2);
    fEJ200->AddElement(fH, 0.08457);
    fEJ200->AddElement(fC, 0.91543);

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
	// EJ276 N(H)=48.1%, N(C)=51.9%
	/////////////////////////////////////////////////////////////////

    fEJ276 = new G4Material("EJ276", 1.096*g/cm3, 2);
    fEJ276->AddElement(fH, 0.07216);
    fEJ276->AddElement(fC, 0.92784);

	G4double photonEnergy_Ej276[36] = {3.131*eV, 3.087*eV, 3.060*eV, 3.044*eV, 3.029*eV, 3.017*eV, 3.010*eV, 3.001*eV, 2.993*eV, 2.984*eV, 
		                               2.976*eV, 2.967*eV, 2.959*eV, 2.950*eV, 2.941*eV, 2.910*eV, 2.857*eV, 2.838*eV, 2.821*eV, 2.802*eV, 
		                               2.784*eV, 2.764*eV, 2.739*eV, 2.705*eV, 2.671*eV, 2.646*eV, 2.625*eV, 2.599*eV, 2.567*eV, 2.533*eV, 
		                               2.500*eV, 2.468*eV, 2.437*eV, 2.406*eV, 2.377*eV, 2.350*eV};

	G4double ScintilFast_EJ276[36] = {0.000, 0.010, 0.088, 0.157, 0.225, 0.293, 0.354, 0.415, 0.492, 0.570, 
	                                  0.649, 0.730, 0.807, 0.882, 0.934, 1.000, 0.890, 0.826, 0.761, 0.692, 
	                                  0.629, 0.569, 0.509, 0.445, 0.388, 0.326, 0.263, 0.200, 0.144, 0.100, 
	                                  0.068, 0.038, 0.024, 0.012, 0.002, 0.000};

    fEJ276MPT = new G4MaterialPropertiesTable();
    fEJ276MPT->AddProperty("RINDEX", photonEnergy_Ej200_2, RefIndex_EJ200, 2);
    fEJ276MPT->AddProperty("ABSLENGTH", photonEnergy_Ej200_2, Absorption_EJ200, 2);
    fEJ276MPT->AddProperty("FASTCOMPONENT", photonEnergy_Ej276, ScintilFast_EJ276, 36);

    fEJ276MPT->AddConstProperty("SCINTILLATIONYIELD", 8600/MeV); // Scintillation efficiency as per Eljen specs
    fEJ276MPT->AddConstProperty("RESOLUTIONSCALE", 1.0); // Intrinsic resolution

    fEJ276MPT->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.9*ns);
    fEJ276MPT->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);
    fEJ276MPT->AddConstProperty("YIELDRATIO",1);// the strength of the fast component as a function of total scintillation yield

	G4double electronYield_EJ276[36];
	G4double protonYield_EJ276[36];
	G4double ionYield_EJ276[36];

	// Produce the scaled light-yield for EJ276 (scaled down from EJ200 by 14%).
	for(size_t i = 0; i < 36; i++){
		electronYield_EJ276[i] = 0.86 * electronYield[i];
		protonYield_EJ276[i] = 0.86 * protonYield[i];
		ionYield_EJ276[i] = 0.86 * ionYield[i];
	}

    fEJ276MPT->AddProperty("ELECTRONSCINTILLATIONYIELD", particleEnergy, electronYield_EJ276, 36)->SetSpline(true);
    fEJ276MPT->AddProperty("PROTONSCINTILLATIONYIELD", particleEnergy, protonYield_EJ276, 36)->SetSpline(true);
    fEJ276MPT->AddProperty("IONSCINTILLATIONYIELD", particleEnergy, ionYield_EJ276, 36)->SetSpline(true);

	fEJ276->SetMaterialPropertiesTable(fEJ276MPT);

	/////////////////////////////////////////////////////////////////
	// Silicone Optical Grease (C2H6OSi)n
	/////////////////////////////////////////////////////////////////
	
    density = 1.06*g/cm3;
    fGrease = new G4Material("Grease",density,ncomponents=4);

    fGrease->AddElement(fC, natoms=2);
    fGrease->AddElement(fH, natoms=6);
    fGrease->AddElement(fO, natoms=1);
    fGrease->AddElement(fSi, natoms=1);

    /*const G4int nEntries_Grease = 5;
    G4double Photon_Energy[nEntries_Grease] = { 2.757*eV, 3.102*eV, 3.312*eV, 3.545*eV, 4.136*eV };
    G4double RefractiveIndex_grease[nEntries_Grease] = {1.468, 1.473, 1.477, 1.482, 1.496};
    G4double Absorption_grease[nEntries_Grease] = { 195*mm,  195*mm, 195*mm, 195*mm, 195*mm };
    fGreaseMPT=new G4MaterialPropertiesTable();

    fGreaseMPT->AddProperty("RINDEX", Photon_Energy, RefractiveIndex_grease, nEntries_Grease);
    fGreaseMPT->AddProperty("ABSLENGTH", Photon_Energy, Absorption_grease, nEntries_Grease);*/

    G4double greasePhotonEnergy[3] = { 2*eV, 3*eV, 4*eV};
    G4double refIndexGrease[3] = {1.465, 1.465, 1.465};
    G4double absorptionGrease[3] = {195*mm, 195*mm, 195*mm};

    fGreaseMPT = new G4MaterialPropertiesTable();
    fGreaseMPT->AddProperty("RINDEX", greasePhotonEnergy, refIndexGrease, 3);
    fGreaseMPT->AddProperty("ABSLENGTH", greasePhotonEnergy, absorptionGrease, 3);
    fGrease->SetMaterialPropertiesTable(fGreaseMPT);

	/////////////////////////////////////////////////////////////////
	// Quartz (SiO2)
	/////////////////////////////////////////////////////////////////

    fSiO2 = nist.searchForMaterial("G4_SILICON_DIOXIDE");

    //optical properties of SiO2 - fused silica or fused quartz
    G4double PhotonEnergy[5] = { 2.484*eV, 2.615*eV, 2.760*eV, 2.922*eV, 3.105*eV };
    /*G4double RefractiveIndex_SiO2[5] = { 1.54, 1.54, 1.54, 1.54, 1.54 };
    G4double Absorption_SiO2[5] = {125.*cm, 123.5*cm, 122.*cm, 121.*cm, 120.*cm};

    fSiO2MPT = new G4MaterialPropertiesTable();
    fSiO2MPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex_SiO2, 5);
    fSiO2MPT->AddProperty("ABSLENGTH", PhotonEnergy, Absorption_SiO2, 5);*/
    
    G4double refIndexSiO2[3] = {1.458, 1.458, 1.458};
    G4double absorptionSiO2[3] = {125*cm, 125*cm, 125*cm};

    fSiO2MPT = new G4MaterialPropertiesTable();
    fSiO2MPT->AddProperty("RINDEX", greasePhotonEnergy, refIndexSiO2, 3);
    fSiO2MPT->AddProperty("ABSLENGTH", greasePhotonEnergy, absorptionSiO2, 3);   
    fSiO2->SetMaterialPropertiesTable(fSiO2MPT);

	/////////////////////////////////////////////////////////////////
	// Silicon (Si)
	/////////////////////////////////////////////////////////////////

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
	// Natural Aluminum
	/////////////////////////////////////////////////////////////////

	fAluminum = new G4Material("Aluminum", 2.7*g/cm3, 1);
	fAluminum->AddElement(fAl, 1);

	double AlEnergies[3] = {2.0*eV, 3.0*eV, 4.0*eV};	                       
	double AlRefIndex[3] = {0.86, 0.50, 0.28};
	double AlAbsLength[3] = {1*mm, 1*mm, 1*mm};

	// H. EHRENREICH et al. Phys. Rev. 132, 5 (1963)
	double aluminumEnergy[8] = {0.000*eV, 0.697*eV, 1.172*eV, 1.350*eV, 1.504*eV, 2.305*eV, 3.174*eV, 4.000*eV};
	double aluminumReflectivity[8] = {1.000, 0.977, 0.950, 0.911, 0.869, 0.914, 0.921, 0.922};

	fAluminumMPT = new G4MaterialPropertiesTable();
	fAluminumMPT->AddProperty("RINDEX", AlEnergies, AlRefIndex, 3);
	fAluminumMPT->AddProperty("ABSLENGTH", AlEnergies, AlAbsLength, 3);
	fAluminumMPT->AddProperty("REFLECTIVITY", aluminumEnergy, aluminumReflectivity, 8);
	fAluminum->SetMaterialPropertiesTable(fAluminumMPT);

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

    fMylar = new G4Material("AluminizedMylar",density=1.39*g/cm3,ncomponents=2);
    fMylar->AddMaterial(Mylar, 0.8);
    fMylar->AddMaterial(Al, 0.2);

    //G4double RefractiveReal_Mylar[5] = {0.81257,0.72122,0.63324,0.55571,0.48787};
    //G4double RefractiveImg_Mylar[5] = {6.0481,5.7556,5.4544,5.1464,4.8355};

	//fMylarMPT = new G4MaterialPropertiesTable();
	//fMylarMPT->AddProperty("REALRINDEX", PhotonEnergy, RefractiveReal_Mylar, 5);
	//fMylarMPT->AddProperty("IMAGINARYRINDEX", PhotonEnergy, RefractiveImg_Mylar, 5);

    fMylarOpticalSurface = new G4OpticalSurface("MylarSurface");
	fMylarOpticalSurface->SetType(dielectric_metal);
	fMylarOpticalSurface->SetFinish(polished); // dielectric_metal only allows polished or ground. Polished dielectric_metal uses only reflectivity or absorption.
	fMylarOpticalSurface->SetModel(glisur);
	fMylarOpticalSurface->SetMaterialPropertiesTable(fAluminumMPT);

	// 100% reflectivity
	double perfectEfficiency[3] = {0.0, 0.0, 0.0};
	double perfectReflectivity[3] = {1.0, 1.0, 1.0};
	double perfectSpecularSpike[3] = {1.0, 1.0, 1.0};
	
	// ESR (98% reflectivity)
	double esrReflectivity[3] = {0.98, 0.98, 0.98};
    
    fEsrMPT = new G4MaterialPropertiesTable();
	fEsrMPT->AddProperty("EFFICIENCY", AlEnergies, perfectEfficiency, 3);
	fEsrMPT->AddProperty("REFLECTIVITY", AlEnergies, esrReflectivity, 3);    
    
    // ESR film (built in look-up-table)
    fEsrOpticalSurface = new G4OpticalSurface("EsrSurface");
    fEsrOpticalSurface->SetType(dielectric_LUT);
    fEsrOpticalSurface->SetModel(LUT);    
    //fEsrOpticalSurface->SetFinish(polishedvm2000air);
    fEsrOpticalSurface->SetFinish(polishedvm2000glue);
    fEsrOpticalSurface->SetMaterialPropertiesTable(fEsrMPT);

	fPerfectMPT = new G4MaterialPropertiesTable();
	fPerfectMPT->AddProperty("EFFICIENCY", AlEnergies, perfectEfficiency, 3);
	fPerfectMPT->AddProperty("REFLECTIVITY", AlEnergies, perfectReflectivity, 3);
	fPerfectMPT->AddProperty("SPECULARSPIKECONSTANT", AlEnergies, perfectSpecularSpike, 3);

	fPerfectOpticalSurface = new G4OpticalSurface("PerfectReflector");
	fPerfectOpticalSurface->SetType(dielectric_metal);
	fPerfectOpticalSurface->SetFinish(polished);
	fPerfectOpticalSurface->SetModel(glisur);
	fPerfectOpticalSurface->SetMaterialPropertiesTable(fPerfectMPT);

	fGreaseOpticalSurface = new G4OpticalSurface("GreaseSurface");
	fGreaseOpticalSurface->SetType(dielectric_dielectric);
	fGreaseOpticalSurface->SetFinish(ground);
	fGreaseOpticalSurface->SetModel(unified); // Defaults to Lambertian reflection (i.e. rough surface) --CRT
	fGreaseOpticalSurface->SetMaterialPropertiesTable(fGreaseMPT);	

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
	// Clean up loaded solids.
	//solids.clear(); // Why are these here? CRT
	//userLayers.clear(); // Why are these here? CRT

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

void nDetConstruction::loadGDML(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "filename posX(cm) posY(cm) posZ(cm) rotX(deg) rotY(deg) rotZ(deg) material"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 8){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::loadGDML(). Expected 8, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: loadGDML <filename> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString>\n";
		return;
	}
	G4ThreeVector position(strtod(args.at(1).c_str(), NULL)*cm, strtod(args.at(2).c_str(), NULL)*cm, strtod(args.at(3).c_str(), NULL)*cm);
	G4ThreeVector rotation(strtod(args.at(4).c_str(), NULL)*deg, strtod(args.at(5).c_str(), NULL)*deg, strtod(args.at(6).c_str(), NULL)*deg);
	loadGDML(args.at(0), position, rotation, args.at(7));
}

G4LogicalVolume *nDetConstruction::loadGDML(const G4String &fname, const G4ThreeVector &position, const G4ThreeVector &rotation, const G4String &material){
	solids.push_back(gdmlSolid());
	gdmlSolid *currentSolid = &solids.back();
	currentSolid->read(fname, material, fCheckOverlaps);
	currentSolid->setRotation(rotation);
	currentSolid->setPosition(position);
	std::cout << " nDetConstruction: Loaded GDML model (name=" << currentSolid->getName() << ") with size x=" << currentSolid->getWidth() << " mm, y=" << currentSolid->getThickness() << " mm, z=" << currentSolid->getLength() << " mm\n";
	
	// Place loaded model into the assembly.
	if(currentSolid->isLoaded())
		currentSolid->placeSolid(assembly_logV, fCheckOverlaps);
	
	return currentSolid->getLogicalVolume();
}

void nDetConstruction::loadLightGuide(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "filename rotX(deg) rotY(deg) rotZ(deg) material"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 5){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::loadGDML(). Expected 5, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: loadGDML <filename> <rotX> <rotY> <rotZ> <matString>\n";
		return;
	}
	G4ThreeVector rotation(strtod(args.at(1).c_str(), NULL)*deg, strtod(args.at(2).c_str(), NULL)*deg, strtod(args.at(3).c_str(), NULL)*deg);
	loadLightGuide(args.at(0), rotation, args.at(4), fMylarOpticalSurface);
}

G4LogicalVolume *nDetConstruction::loadLightGuide(const G4String &fname, const G4ThreeVector &rotation, const G4String &material, G4OpticalSurface *surface){
	solids.push_back(gdmlSolid());
	gdmlSolid *currentSolid = &solids.back();
	currentSolid->read(fname, material, fCheckOverlaps);
	currentSolid->setLogicalBorders("InnerWrapping", surface);
	currentSolid->setRotation(rotation);

	G4double trapezoidZ = currentOffsetZ + currentSolid->getLength()/2;
	std::cout << " nDetConstruction: Loaded GDML model (name=" << currentSolid->getName() << ") with size x=" << currentSolid->getWidth() << " mm, y=" << currentSolid->getThickness() << " mm, z=" << currentSolid->getLength() << " mm\n";

	// Place loaded model into the assembly.
	if(currentSolid->isLoaded()){
		// Place the light-guide on the positive z side.
		currentSolid->setPosition(G4ThreeVector(0, 0, trapezoidZ));
		currentSolid->placeSolid(assembly_logV, fCheckOverlaps);
		
		// And on the negative z side.
		G4RotationMatrix *trapRot = new G4RotationMatrix();
		trapRot->rotateX(rotation.getX()-pi);
		currentSolid->placeSolid(trapRot, G4ThreeVector(0, 0, -trapezoidZ), assembly_logV, fCheckOverlaps);
	}

	currentLayerSizeX = currentSolid->getWidth();
	currentLayerSizeY = currentSolid->getThickness();
	currentOffsetZ += currentSolid->getLength();
	fTrapezoidLength = currentSolid->getLength()*mm;

	return currentSolid->getLogicalVolume();
}

void nDetConstruction::AddGDML(const G4String &input){
	userLayers.push_back(userAddLayer(input, &nDetConstruction::loadGDML));
}

void nDetConstruction::AddLightGuideGDML(const G4String &input){
	userLayers.push_back(userAddLayer(input, &nDetConstruction::loadLightGuide));
}

void nDetConstruction::AddGrease(const G4String &input){
	userLayers.push_back(userAddLayer(input, &nDetConstruction::applyGreaseLayer));
}

void nDetConstruction::AddDiffuser(const G4String &input){
	userLayers.push_back(userAddLayer(input, &nDetConstruction::applyDiffuserLayer));
}

void nDetConstruction::AddLightGuide(const G4String &input){
	userLayers.push_back(userAddLayer(input, &nDetConstruction::applyLightGuide));
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
    G4LogicalVolume *cellScint_logV = new G4LogicalVolume(cellScint, getUserDetectorMaterial(), "scint_log");
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
			scintBody_physV.push_back(cellScint_physV[col][row]);
		
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
	G4LogicalVolume *scint_logV = new G4LogicalVolume(scintBody2, getUserDetectorMaterial(), "scint_logV");	
    scint_logV->SetVisAttributes(scint_VisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *ellipseBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), scint_logV, "Scint", assembly_logV, true, 0, fCheckOverlaps);

	scintBody_physV.push_back(ellipseBody_physV);

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

    // Reflective wrapping.
    if(fMylarThickness > 0)
	    new G4LogicalBorderSurface("Wrapping", ellipseBody_physV, ellipseWrapping_physV, getUserOpticalSurface());
}

void nDetConstruction::buildRectangle(){
	// Construct the assembly bounding box.
	G4ThreeVector assemblyBoundingBox = constructAssembly();

    G4Box *plateBody = new G4Box("", fDetectorWidth/2, fDetectorThickness/2, fDetectorLength/2);
    G4LogicalVolume *plateBody_logV = new G4LogicalVolume(plateBody, getUserDetectorMaterial(), "plateBody_logV");
    plateBody_logV->SetVisAttributes(scint_VisAtt);

	/*// Add some bubbles (for testing).
	G4Sphere *bubble = new G4Sphere("", 0, 1.5*mm, 0, 360*deg, 0, 360*deg);
	
	//G4SubtractionSolid *bubblePlate = new G4SubtractionSolid("", plateBody, bubble);
	G4SubtractionSolid *bubblePlate;
	for(size_t i = 0; i < 10; i++){
		G4double x = (fDetectorWidth/2)*(2*G4UniformRand()-1);
		G4double y = (fDetectorThickness/2)*(2*G4UniformRand()-1);
		G4double z = (fDetectorLength/2)*(2*G4UniformRand()-1);
		if(i != 0)
			bubblePlate = new G4SubtractionSolid("", bubblePlate, bubble, 0, G4ThreeVector(x, y, z));
		else
			bubblePlate = new G4SubtractionSolid("", plateBody, bubble, 0, G4ThreeVector(x, y, z));
	}
	G4LogicalVolume *bubble_logV = new G4LogicalVolume(bubblePlate, getUserDetectorMaterial(), "bubble_logV");*/

	// Place the scintillator inside the assembly.
	G4PVPlacement *plateBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), plateBody_logV, "Scint", assembly_logV, true, 0, fCheckOverlaps);
	//G4PVPlacement *plateBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), bubble_logV, "Scint", assembly_logV, true, 0, fCheckOverlaps);

	scintBody_physV.push_back(plateBody_physV);

	// Build the wrapping.
	G4PVPlacement *plateWrapping_physV = NULL;
	if(fMylarThickness > 0){
		G4Box *plateBoundingBox = new G4Box("", fDetectorWidth/2, fDetectorThickness/2, fDetectorLength/2);
		G4Box *plateWrappingBox = new G4Box("", fDetectorWidth/2 + fMylarThickness, fDetectorThickness/2 + fMylarThickness, fDetectorLength/2);

		G4SubtractionSolid *plateWrapping = new G4SubtractionSolid("", plateWrappingBox, plateBoundingBox);
		G4LogicalVolume *plateWrapping_logV = new G4LogicalVolume(plateWrapping, getUserSurfaceMaterial(), "plateWrapping_logV");
		plateWrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		// Place the wrapping around the scintillator.
		plateWrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), plateWrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);		
	}

    // Reflective wrapping.
    if(fMylarThickness > 0)
	    new G4LogicalBorderSurface("Wrapping", plateBody_physV, plateWrapping_physV, getUserOpticalSurface());
}

void nDetConstruction::buildTestAssembly(){
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

	G4double sensitiveZ = currentOffsetZ + fGreaseThickness + fWindowThickness + fSensitiveThickness/2;
	G4double wrappingThickness = fGreaseThickness + fWindowThickness;
	
	// The optical grease layer.
	G4PVPlacement *grease_physV[2] = {NULL, NULL};
	if(fGreaseThickness > 0){
		G4double greaseZ = currentOffsetZ + fGreaseThickness/2;
	
		G4Box* grease_solidV = new G4Box("window_solidV", SiPM_dimension, SiPM_dimension, fGreaseThickness/2);
		G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, fGrease, "grease_logV");
		
		grease_logV->SetVisAttributes(grease_VisAtt);
	
		grease_physV[0] = new G4PVPlacement(0, G4ThreeVector(0, 0, greaseZ), grease_logV, "Grease", assembly_logV, true, 0, fCheckOverlaps);
		grease_physV[1] = new G4PVPlacement(0, G4ThreeVector(0, 0, -greaseZ), grease_logV, "Grease", assembly_logV, true, 0, fCheckOverlaps);
		
		if(!fPolishedInterface){
			for(std::vector<G4PVPlacement*>::iterator iter = scintBody_physV.begin(); iter != scintBody_physV.end(); iter++){
				new G4LogicalBorderSurface("GreaseInterface", (*iter), grease_physV[0], fGreaseOpticalSurface);
				new G4LogicalBorderSurface("GreaseInterface", (*iter), grease_physV[1], fGreaseOpticalSurface);
			}
		}
		
		// Clear all scintillator placements.
		scintBody_physV.clear();
	}

	G4PVPlacement *window_physV[2] = {NULL, NULL};
	if(fWindowThickness > 0){ // The quartz window
		G4double windowZ = currentOffsetZ + fGreaseThickness + fWindowThickness/2;
	
		G4Box* window_solidV = new G4Box("window_solidV", SiPM_dimension, SiPM_dimension, fWindowThickness/2);
		G4LogicalVolume *window_logV = new G4LogicalVolume(window_solidV, fSiO2, "window_logV");
		
		window_logV->SetVisAttributes(window_VisAtt);
	
		window_physV[0] = new G4PVPlacement(0, G4ThreeVector(0, 0, windowZ), window_logV, "Quartz", assembly_logV, true, 0, fCheckOverlaps);
		window_physV[1] = new G4PVPlacement(0, G4ThreeVector(0, 0, -windowZ), window_logV, "Quartz", assembly_logV, true, 0, fCheckOverlaps);
	}

	// Build the wrapping.
	if(fMylarThickness > 0 && wrappingThickness > 0){
		G4Box *boundingBox = new G4Box("", SiPM_dimension, SiPM_dimension, wrappingThickness/2);
		G4Box *wrappingBox = new G4Box("", SiPM_dimension + fMylarThickness, SiPM_dimension + fMylarThickness, wrappingThickness/2);
		
		G4SubtractionSolid *greaseWrapping = new G4SubtractionSolid("", wrappingBox, boundingBox);
		G4LogicalVolume *greaseWrapping_logV = new G4LogicalVolume(greaseWrapping, getUserSurfaceMaterial(), "greaseWrapping_logV");
		greaseWrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		G4double wrappingZ = currentOffsetZ + fGreaseThickness;
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *greaseWrapping_physV[2];
		greaseWrapping_physV[0] = new G4PVPlacement(0, G4ThreeVector(0, 0, wrappingZ), greaseWrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);		
		greaseWrapping_physV[1] = new G4PVPlacement(0, G4ThreeVector(0, 0, -wrappingZ), greaseWrapping_logV, "Wrapping", assembly_logV, true, 0, fCheckOverlaps);
		
		if(grease_physV[0] && grease_physV[1]){
			new G4LogicalBorderSurface("Wrapping", grease_physV[0], greaseWrapping_physV[0], getUserOpticalSurface());
			new G4LogicalBorderSurface("Wrapping", grease_physV[1], greaseWrapping_physV[1], getUserOpticalSurface());
		}
		if(window_physV[0] && window_physV[1]){
			new G4LogicalBorderSurface("Wrapping", window_physV[0], greaseWrapping_physV[0], getUserOpticalSurface());
			new G4LogicalBorderSurface("Wrapping", window_physV[1], greaseWrapping_physV[1], getUserOpticalSurface());
		}
	}
	
    // The photon sensitive surface
    G4Box *sensitive_solidV = new G4Box(name+"_solidV", SiPM_dimension, SiPM_dimension, fSensitiveThickness/2);
    G4LogicalVolume *sensitive_logV = new G4LogicalVolume(sensitive_solidV, fSil, name+"_logV");
    sensitive_logV->SetVisAttributes(sensitive_VisAtt);
    
    // Logical skin surface.
    new G4LogicalSkinSurface(name, sensitive_logV, fSiliconPMOpticalSurface);    
    
	new G4PVPlacement(0, G4ThreeVector(0, 0, sensitiveZ), sensitive_logV, name, assembly_logV, true, 0, fCheckOverlaps);
	new G4PVPlacement(0, G4ThreeVector(0, 0, -sensitiveZ), sensitive_logV, name, assembly_logV, true, 0, fCheckOverlaps);

	/*// Physically segmented PMT.
 	for(int col = 0; col < fNumColumns; col++){
		for(int row = 0; row < fNumRows; row++){
			//G4ThreeVector cellCenter(-fDetectorWidth/2 + (2*col+1)*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + (2*row+1)*fMylarThickness + (row+0.5)*cellHeight, 0);
			G4ThreeVector cellCenterL(-fDetectorWidth/2 + col*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + row*fMylarThickness + (row+0.5)*cellHeight, sensitiveZ);
			G4ThreeVector cellCenterR(-fDetectorWidth/2 + col*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + row*fMylarThickness + (row+0.5)*cellHeight, -sensitiveZ);

			// Copy numbers (segment IDs), indexed from 1
			std::stringstream stream; stream << name << "-" << col << "," << row;
			new G4PVPlacement(0, cellCenterL, sensitive_logV, stream.str().c_str(), assembly_logV, 0, col*fNumRows+row+1, fCheckOverlaps); 
			new G4PVPlacement(0, cellCenterR, sensitive_logV, stream.str().c_str(), assembly_logV, 0, col*fNumRows+row+1, fCheckOverlaps);
		}
	}*/
    
    // Move the current offset past the PMT
	currentLayerSizeX = 2*SiPM_dimension;
	currentLayerSizeY = 2*SiPM_dimension;
    currentOffsetZ += fGreaseThickness + fWindowThickness + fSensitiveThickness;
}

void nDetConstruction::applyGreaseLayer(){
	this->applyGreaseLayer(currentLayerSizeX, currentLayerSizeY);
}

void nDetConstruction::applyGreaseLayer(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "addGreaseLayer width(mm) height(mm) thickness(mm)"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 2){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::applyGreaseLayer(). Expected 2, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: addGreaseLayer <width> <height> [thickness]\n";
		return;
	}
	double width = strtod(args.at(0).c_str(), NULL);
	double height = strtod(args.at(1).c_str(), NULL);
	if(Nargs >= 3)
		this->applyGreaseLayer(width, height, strtod(args.at(2).c_str(), NULL));
	else
		this->applyGreaseLayer(width, height);
}

void nDetConstruction::applyGreaseLayer(const G4double &x, const G4double &y, double thickness/*=0*/){
	if(thickness <= 0)
		thickness = fGreaseThickness;
    if(thickness > 0){
	    G4Box *grease_solidV = new G4Box("grease", x/2, y/2, thickness/2);
	    G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, fGrease, "grease_logV");
	    grease_logV->SetVisAttributes(grease_VisAtt);
	
	    new G4PVPlacement(0, G4ThreeVector(0, 0, currentOffsetZ+thickness/2), grease_logV, "Grease", assembly_logV, true, 0, fCheckOverlaps);
	    new G4PVPlacement(0, G4ThreeVector(0, 0, -currentOffsetZ-thickness/2), grease_logV, "Grease", assembly_logV, true, 0, fCheckOverlaps);
		currentLayerSizeX = x;
		currentLayerSizeY = y;
		currentOffsetZ += thickness;
	}
}

void nDetConstruction::applyDiffuserLayer(){
	this->applyDiffuserLayer(currentLayerSizeX, currentLayerSizeY, fDiffuserLength);
}

void nDetConstruction::applyDiffuserLayer(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "addDiffuserLayer width(mm) height(mm) thickness(mm) material"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 3){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::applyDiffuserLayer(). Expected 3, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: addDiffuserLayer <width> <height> <thickness> [material=G4_SILICON_DIOXIDE]\n";
		return;
	}
	double width = strtod(args.at(0).c_str(), NULL);
	double height = strtod(args.at(1).c_str(), NULL);
	double thickness = strtod(args.at(2).c_str(), NULL);
	this->applyDiffuserLayer(width, height, thickness);
}

void nDetConstruction::applyDiffuserLayer(const G4double &x, const G4double &y, const double &thickness){
    if(thickness > 0){ // Build the light diffusers (if needed)
        G4Box *lightDiffuser = new G4Box("lightDiffuser", x/2, y/2, thickness/2);
        G4LogicalVolume *lightDiffuserLog = new G4LogicalVolume(lightDiffuser, fSiO2, "lightDiffuser_logV");

        new G4PVPlacement(0, G4ThreeVector(0, 0, currentOffsetZ+thickness/2), lightDiffuserLog, "Diffuser", assembly_logV, true, 0, fCheckOverlaps);
        new G4PVPlacement(0, G4ThreeVector(0, 0, -currentOffsetZ-thickness/2), lightDiffuserLog, "Diffuser", assembly_logV, true, 0, fCheckOverlaps);
        
    	// Offset the other layers to account for the light-diffuser and another layer of grease.
		currentLayerSizeX = x;
		currentLayerSizeY = y;
    	currentOffsetZ += thickness;
    }
}

void nDetConstruction::applyLightGuide(){
	this->applyLightGuide(currentLayerSizeX, 2*SiPM_dimension, currentLayerSizeY, 2*SiPM_dimension, fTrapezoidLength);
}

void nDetConstruction::applyLightGuide(const G4double &x2, const G4double &y2){
	this->applyLightGuide(currentLayerSizeX, x2, currentLayerSizeY, y2, fTrapezoidLength);
}

void nDetConstruction::applyLightGuide(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "addLightGuide width1(mm) width2(mm) height1(mm) height2(mm) thickness(mm) material"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 5){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::applyLightGuide(). Expected 5, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: addLightGuide <width1> <width2> <height1> <height2> <thickness> [material=G4_SILICON_DIOXIDE]\n";
		return;
	}
	double width1 = strtod(args.at(0).c_str(), NULL);
	double width2 = strtod(args.at(1).c_str(), NULL);
	double height1 = strtod(args.at(2).c_str(), NULL);
	double height2 = strtod(args.at(3).c_str(), NULL);
	double thickness = strtod(args.at(4).c_str(), NULL);
	this->applyLightGuide(width1, width2, height1, height2, thickness);
}

void nDetConstruction::applyLightGuide(const G4double &x1, const G4double &x2, const G4double &y1, const G4double &y2, const double &thickness){
    if(thickness > 0){ // Build the light guides (if needed)
        const G4double trapAngleXZ = std::atan2(2*thickness, x1-x2);
        const G4double trapAngleYZ = std::atan2(2*thickness, y1-y2);
        
        const G4double deltaX = fMylarThickness/std::sin(trapAngleXZ);
        const G4double deltaY = fMylarThickness/std::sin(trapAngleYZ);
        
        G4double trapezoidZ = currentOffsetZ + thickness/2;
        
        std::string trapName = "Acrylic";
        
        G4RotationMatrix *trapRot[2] = {new G4RotationMatrix, new G4RotationMatrix};
        
        // Build the light-guide.
	    G4Trd *lightGuide = new G4Trd("lightGuide", x1/2, x2/2, y1/2, y2/2, thickness/2);
	    G4LogicalVolume *lightGuideLog = new G4LogicalVolume(lightGuide, fSiO2, "lightGuide_logV");	

		trapRot[1]->rotateY(180*deg);

		// Place the light guides.
		G4PVPlacement *trapPhysicalL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), lightGuideLog, trapName, assembly_logV, true, 0, fCheckOverlaps);
		G4PVPlacement *trapPhysicalR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), lightGuideLog, trapName, assembly_logV, true, 0, fCheckOverlaps);

		// Build the wrapping.
		if(fMylarThickness > 0){
			G4Trd *wrappingSolid = new G4Trd("wrapping", x1/2+deltaX, x2/2+deltaY, y1/2+deltaX, y2/2+deltaY, thickness/2);
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
		
        /*else{ // Load the light-guide from a GDML file.
        	gdmlSolid *currentSolid = &solids.back();
        
	    	//G4LogicalVolume *lightGuideLog = currentSolid->getLogicalVolume();
			trapName = currentSolid->getName();

			// Set the dimensions of the light-guide.
		    G4Trd *lightGuide = new G4Trd("lightGuide", currentSolid->getWidth()/2, x2/2, currentSolid->getThickness()/2, y2/2, thickness/2);

			trapRot[0]->rotateX(gdmlRotation.getX()*deg);
			trapRot[0]->rotateY(gdmlRotation.getY()*deg);
			trapRot[0]->rotateZ(gdmlRotation.getZ()*deg);

			trapRot[1]->rotateX(gdmlRotation.getX()*deg);
			trapRot[1]->rotateY(gdmlRotation.getY()*deg);
			trapRot[1]->rotateZ(gdmlRotation.getZ()*deg);

		    std::vector<G4PVPlacement*> trapPhysicalL;
			std::vector<G4PVPlacement*> trapPhysicalR;
	    	currentSolid->placeSolid(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), assembly_logV, trapPhysicalL);
	    	currentSolid->placeSolid(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), assembly_logV, trapPhysicalR);
			currentSolid->setLogicalBorders("Wrapping", getUserOpticalSurface(), trapPhysicalL);
			currentSolid->setLogicalBorders("Wrapping", getUserOpticalSurface(), trapPhysicalR);	 

			// Build the wrapping.
			if(fMylarThickness > 0){
				G4Trd *wrappingSolid = new G4Trd("wrapping", currentSolid->getWidth()/2+deltaX, x2/2+deltaY, currentSolid->getThickness()/2+deltaX, y2/2+deltaY, thickness/2);
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
	    }*/

		currentLayerSizeX = x2;
		currentLayerSizeY = y2;
    	currentOffsetZ += thickness;
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

	return G4ThreeVector(boundingX, boundingY, boundingZ);
}

G4Material *nDetConstruction::getUserDetectorMaterial(){
	if(detectorMaterial == "ej200")
		return fEJ200;
	else if(detectorMaterial == "ej276")
		return fEJ276;
	
	return fEJ200; // default
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
    else if(wrappingMaterial == "perfect")
    	return fMylar;
    
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
    else if(wrappingMaterial == "perfect")
    	return fPerfectOpticalSurface;
    
    return fMylarOpticalSurface; // default
}
