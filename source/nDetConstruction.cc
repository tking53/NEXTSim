#include "G4LogicalVolume.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VPhysicalVolume.hh"

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"

#include "G4GeometryManager.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"

#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"

#include "nDetConstruction.hh"
#include "nDetConstructionMessenger.hh"
#include "nDetThreadContainer.hh"
#include "nDetParticleSource.hh"
#include "termColors.hh"
#include "optionHandler.hh" // split_str

///////////////////////////////////////////////////////////////////////////////
// class nDetConstruction
///////////////////////////////////////////////////////////////////////////////

nDetConstruction &nDetConstruction::getInstance(){
	// The only instance
	// Guaranteed to be lazy initialized
	// Guaranteed that it will be destroyed correctly
	static nDetConstruction instance;
	return instance;
}

nDetConstruction::nDetConstruction(){
	expHall_logV = NULL;
	expHall_physV = NULL;
	currentDetector = NULL;

	fDetectorMessenger = new nDetConstructionMessenger(this);

	fCheckOverlaps = false;

	// Set the default size of the experimental hall
	expHallSize = G4ThreeVector(10*m, 10*m, 10*m);

	expHallMaterial = "air";
	expHallFloorMaterial = "";

	expHallFloorThickness = 0;
	expHallFloorCenterY = 0;

	shadowBarMaterial = NULL;
	
	// Initialize the detector parameter messenger
	params.InitializeMessenger();
}

nDetConstruction::~nDetConstruction(){
}

G4VPhysicalVolume* nDetConstruction::Construct(){
	if(!expHall_physV)
		this->ConstructDetector();
	
	return expHall_physV;
}
	
G4VPhysicalVolume* nDetConstruction::ConstructDetector(){
	if(!materials.materialsAreDefined())
		materials.initialize();

	// Build experiment hall.
	buildExpHall();

	// Place all detectors.
	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		currentDetector = &(*iter);

		// Build the detector
		iter->construct();

		// Place the detector into the world.
		iter->placeDetector(expHall_logV);
	}

	// Place all external GDML solids
	for(std::vector<gdmlSolid>::iterator iter = solids.begin(); iter != solids.end(); iter++){
		iter->placeSolid(expHall_logV);
	}	

	// Build the shadow bar.
	if(shadowBarMaterial){
		G4Box *shadowBox = new G4Box("shadowBox", shadowBarSize.getX()/2, shadowBarSize.getY()/2, shadowBarSize.getZ()/2);
		G4LogicalVolume *shadowBox_logV = new G4LogicalVolume(shadowBox, shadowBarMaterial, "shadowBox_logV", 0, 0, 0);
		shadowBox_logV->SetVisAttributes(materials.visShadow);
		new G4PVPlacement(0, shadowBarPos, shadowBox_logV, "ShadowBar", expHall_logV, true, 0, fCheckOverlaps);
	}

	return expHall_physV;
}

void nDetConstruction::ClearGeometry(){
	// Clean-up previous geometry
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
	G4SolidStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	
	// Reset the world volume. Why is this needed? CRT
	expHall_physV = NULL;
	
	// Clear previous construction.
	userDetectors.clear();
	
	// Clear all loaded solids.
	solids.clear();
	
	// Reset the scintillator copy number.
	params.SetScintillatorCopyNumber(1);
}

void nDetConstruction::UpdateGeometry(){
	// Define new one
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4RunManager::GetRunManager()->ReinitializeGeometry();

	// Segment the PMT photo-sensitive surface
	if(params.PmtIsSegmented()){
		setSegmentedPmt();
		if(!gainMatrixFilename.empty())
			loadPmtGainMatrix();
	}
	
	// Load the anode quantum efficiency
	if(!spectralResponseFilename.empty())
		loadPmtSpectralResponse();
	
	// Update the particle source
	if(currentDetector)
		nDetParticleSource::getInstance().SetDetector(currentDetector);

	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		iter->copyCenterOfMass(center[0], center[1]);

		if(params.PmtIsSegmented()){
			iter->getCenterOfMassL()->setSegmentedPmt(&params);
			iter->getCenterOfMassR()->setSegmentedPmt(&params);	

			// Copy the PMT anode gain matrix
			iter->getCenterOfMassL()->copyGainMatrix(&center[0]);
			iter->getCenterOfMassR()->copyGainMatrix(&center[1]);
		}
	
		// Copy the PMT anode quantum efficiency curve
		if(iter->getCenterOfMassL()->getPmtResponse()->getSpectralResponseEnabled())
			iter->getCenterOfMassL()->copySpectralResponse(&center[0]);
		if(iter->getCenterOfMassR()->getPmtResponse()->getSpectralResponseEnabled())
			iter->getCenterOfMassR()->copySpectralResponse(&center[1]);
	}
	
	// Update the detector lists of all user run actions
	nDetThreadContainer *container = &nDetThreadContainer::getInstance();
	if(container->getMultithreadingMode())
		container->getMasterRunAction()->updateDetector(this);
	for(size_t index = 0; index < container->size(); index++){
		container->getActionManager(index)->getRunAction()->updateDetector(this);
	}
}

bool nDetConstruction::SetWorldFloor(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "centerY(cm) thickness(cm) [material=G4_CONCRETE]"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 2){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::SetWorldFloor(). Expected 2, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: <centerY> <thickness> [material=G4_CONCRETE]\n";
		return false;
	}
	expHallFloorCenterY = strtod(args.at(0).c_str(), NULL)*cm;
	expHallFloorThickness = strtod(args.at(1).c_str(), NULL)*cm;
	if(Nargs < 3) // Defaults to concrete
		expHallFloorMaterial = "G4_CONCRETE";
	else
		expHallFloorMaterial = args.at(2);
	return true;
}

void nDetConstruction::LoadGDML(const G4String &input){
	loadGDML(input);
}

gdmlSolid *nDetConstruction::loadGDML(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "filename posX(cm) posY(cm) posZ(cm) rotX(deg) rotY(deg) rotZ(deg) material"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 8){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::loadGDML(). Expected 8, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: loadGDML <filename> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString>\n";
		return NULL;
	}
	G4ThreeVector position(strtod(args.at(1).c_str(), NULL)*cm, strtod(args.at(2).c_str(), NULL)*cm, strtod(args.at(3).c_str(), NULL)*cm);
	G4ThreeVector rotation(strtod(args.at(4).c_str(), NULL)*deg, strtod(args.at(5).c_str(), NULL)*deg, strtod(args.at(6).c_str(), NULL)*deg);
	return loadGDML(args.at(0), position, rotation, args.at(7));
}

gdmlSolid *nDetConstruction::loadGDML(const G4String &fname, const G4ThreeVector &position, const G4ThreeVector &rotation, const G4String &material){
	solids.push_back(gdmlSolid());
	gdmlSolid *currentSolid = &solids.back();
	currentSolid->read(fname, material, fCheckOverlaps);
	currentSolid->setRotation(rotation);
	currentSolid->setPosition(position);
	std::cout << " nDetConstruction: Loaded GDML model (name=" << currentSolid->getName() << ") with size x=" << currentSolid->getWidth() << " mm, y=" << currentSolid->getThickness() << " mm, z=" << currentSolid->getLength() << " mm\n";
	return currentSolid;
}

bool nDetConstruction::AddGeometry(const G4String &geom){
	// Add a new detector assembly to the vector of detectors
	// Copy the detector attributes to the new detector
	userDetectors.push_back(nDetDetector(this, &materials));
	currentDetector = &userDetectors.back();

	// Enable/disable overlap checking
	currentDetector->setCheckOverlaps(fCheckOverlaps);

	if(!currentDetector->setGeometry(geom)){ // Set the detector type
		Display::ErrorPrint("User specified un-recognized detector type!", "nDetConstruction");
		return false;
	}

	// Update the detector's copy numbers.
	currentDetector->setParentCopyNumber(userDetectors.size()-1);

	// Set true isotropic source mode for multiple detectors
	if(userDetectors.size() > 1)
		nDetParticleSource::getInstance().SetRealIsotropicMode(true);
	
	return true;
}

void nDetConstruction::setSegmentedPmt(){
	center[0].setSegmentedPmt(&params);
	center[1].setSegmentedPmt(&params);
	std::cout << " nDetConstruction: Set segmented PMTs with WxH=(" << params.GetPmtWidth() << " x " << params.GetPmtHeight() << ") and " << params.GetNumPmtColumns() << " columns and " << params.GetNumPmtRows() << " rows.\n";
}

bool nDetConstruction::loadPmtSpectralResponse(){
	if(!(center[0].loadSpectralResponse(spectralResponseFilename.c_str()) && center[1].loadSpectralResponse(spectralResponseFilename.c_str()))){
		Display::ErrorPrint("Failed to load PMT spectral response from file!", "nDetConstruction");
		return false;
	}
	std::cout << " nDetConstruction: Successfully loaded PMT spectral response function\n";
	spectralResponseFilename = ""; // Unset the filename so that it will not be loaded again later
	return true;
}

bool nDetConstruction::loadPmtGainMatrix(){
	if(!(center[0].loadGainMatrix(gainMatrixFilename.c_str()) && center[1].loadGainMatrix(gainMatrixFilename.c_str()))){
		Display::ErrorPrint("Failed to load PMT anode gain matrix from file!", "nDetConstruction");
		return false;
	}
	std::cout << " nDetConstruction: Successfully loaded PMT anode gain matrix\n";
	gainMatrixFilename = ""; // Unset the filename so that it will not be loaded again later
	return true;
}

void nDetConstruction::SetShadowBarSize(const G4ThreeVector &size){
	shadowBarSize = size;
}

void nDetConstruction::SetShadowBarPosition(const G4ThreeVector &pos){
	shadowBarPos = pos;		
}

bool nDetConstruction::SetShadowBarMaterial(const G4String &material){
	shadowBarMaterial = materials.searchForMaterial(material);
	if(!shadowBarMaterial)
		return false;
	return true;
}

void nDetConstruction::buildExpHall()
{
	G4Box* expHall_solidV = new G4Box("expHall_solidV", expHallSize.getX()/2, expHallSize.getY()/2, expHallSize.getZ()/2);

	G4Material *expHallFill = materials.searchForMaterial(expHallMaterial);
	if(!expHallFill){ // Use the default material, if
		Display::WarningPrint("Failed to find user-specified world material!", "nDetConstruction");
		Display::WarningPrint(" Defaulting to filling world volume with air", "nDetConstruction");
		expHallFill = materials.fAir;
	}

	expHall_logV  = new G4LogicalVolume(expHall_solidV, expHallFill, "expHall_logV", 0, 0, 0);
	expHall_logV->SetVisAttributes(G4VisAttributes::Invisible);

	expHall_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), expHall_logV, "expHall_physV",0,false,0);

	// Add a floor to the experimental hall (disabled by default)
	if(!expHallFloorMaterial.empty() && expHallFloorThickness > 0){
		G4Material *floorMaterial = materials.searchForMaterial(expHallFloorMaterial);
		if(expHallFloorMaterial){
			G4Box *floorBox = new G4Box("floor", expHallSize.getX()/2, expHallFloorThickness/2, expHallSize.getZ()/2);
			G4LogicalVolume *floorBox_logV = new G4LogicalVolume(floorBox, floorMaterial, "floorBox_logV");
			floorBox_logV->SetVisAttributes(materials.visShadow);
			expHall_logV->SetVisAttributes(materials.visAssembly);
			new G4PVPlacement(NULL, G4ThreeVector(0, -expHallFloorCenterY, 0), floorBox_logV, "floorBox_physV", expHall_logV, 0, 0, false);
		}
		else{
			Display::WarningPrint("Failed to find user-specified floor material!", "nDetConstruction");
			Display::WarningPrint(" Disabling the use of a floor", "nDetConstruction");
			expHallFloorMaterial = "";
			expHallFloorThickness = 0;
		}
	}

	return;
}

void nDetConstruction::AddGDML(const G4String &input){
	if(currentDetector)
		currentDetector->addGDML(input);
	else
		Display::ErrorPrint("Cannot add GDML component before a detector is defined!", "nDetConstruction");
}

void nDetConstruction::AddLightGuideGDML(const G4String &input){
	if(currentDetector)
		currentDetector->addLightGuideGDML(input);
	else
		Display::ErrorPrint("Cannot add GDML light-guide before a detector is defined!", "nDetConstruction");
}

void nDetConstruction::AddGrease(const G4String &input){
	if(currentDetector)
		currentDetector->addGreaseLayer(input);
	else
		Display::ErrorPrint("Cannot add grease layer before a detector is defined!", "nDetConstruction");
}

void nDetConstruction::AddDiffuser(const G4String &input){
	if(currentDetector)
		currentDetector->addDiffuserLayer(input);
	else
		Display::ErrorPrint("Cannot add diffuser layer before a detector is defined!", "nDetConstruction");
}

void nDetConstruction::AddLightGuide(const G4String &input){
	if(currentDetector)
		currentDetector->addLightGuideLayer(input);
	else
		Display::ErrorPrint("Cannot add light-guide before a detector is defined!", "nDetConstruction");
}

void nDetConstruction::AddDetectorArray(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "addDiffuserLayer width(mm) height(mm) thickness(mm) material"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 3){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::AddDetectorArray(). Expected 5, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: addArray <geom> <r0> <startTheta> <stopTheta> <Ndet>\n";
		return;
	}
	double r0 = strtod(args.at(1).c_str(), NULL);
	double startTheta = strtod(args.at(2).c_str(), NULL);
	double stopTheta = strtod(args.at(3).c_str(), NULL);
	int Ndet = strtol(args.at(4).c_str(), NULL, 10);
	
	double dTheta = 0;
	if(Ndet > 1)
		dTheta = (stopTheta-startTheta)/(Ndet-1);
	for(int i = 0; i < Ndet; i++){
		//std::cout << " nDetConstruction: Adding detector (type=" << args.at(0) << ", r=" << r0 << ", theta=" << startTheta+dTheta*i << ")\n";
		params.SetPositionCylindrical(G4ThreeVector(r0, startTheta+dTheta*i, 0));
		params.SetRotation(G4ThreeVector(90, 0, startTheta+dTheta*i));
		AddGeometry(args.at(0));
	}
}

void nDetConstruction::SetLightYieldMultiplier(const G4double &yield){ 
	materials.setLightYield(yield);
}

void nDetConstruction::PrintAllDetectors() const {
	int detCount = 0;
	for(std::vector<nDetDetector>::const_iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		std::cout << "***********************************************************\n";
		std::cout << " Detector ID          = " << detCount++ << std::endl;
		iter->Print();
	}
	std::cout << "***********************************************************\n";
}
