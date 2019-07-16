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

	expHallX = 10*m;
	expHallY = 10*m;
	expHallZ = 10*m;

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
	
	// Reset the scintillator copy number.
	params.scintCopyNum = 1;
}

void nDetConstruction::UpdateGeometry(){
	// Define new one
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4RunManager::GetRunManager()->ReinitializeGeometry();

	if(params.PmtIsSegmented())
		setSegmentedPmt(params.fNumColumnsPmt, params.fNumRowsPmt, params.pmtWidth, params.pmtHeight);

	// Update the particle source
	nDetParticleSource::getInstance().SetDetector(currentDetector);
	
	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		iter->copyCenterOfMass(center[0], center[1]);
	}
	
	// Update the detector lists of all user run actions
	nDetThreadContainer *container = &nDetThreadContainer::getInstance();
	if(container->getMultithreadingMode())
		container->getMasterRunAction()->updateDetector(this);
	for(size_t index = 0; index < container->size(); index++){
		container->getActionManager(index)->getRunAction()->updateDetector(this);
	}
}

void nDetConstruction::LoadGDML(const G4String &input){
	gdmlSolid *model = loadGDML(input);
	if(model->isLoaded()) // Check that the model was loaded correctly
		new G4PVPlacement(model->getRotation(), *model->getPosition(), model->getLogicalVolume(), model->getName(), expHall_logV, true, 0, fCheckOverlaps);
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
	
	// Place loaded model into the assembly.
	if(currentSolid->isLoaded())
		currentSolid->placeSolid(expHall_logV, fCheckOverlaps);
	
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

	/*currentLayerSizeX = params.fDetectorWidth;
	currentLayerSizeY = params.fDetectorThickness;
	currentOffsetZ = params.fDetectorLength/2;

	// Update the position and rotation of the detector.
	currentDetector->setPositionAndRotation(detectorPosition, detectorRotation);
	currentDetector->setCurrentOffset(currentLayerSizeX, currentLayerSizeY, currentOffsetZ);*/
		
	// Update the detector attributes
	//currentDetector->setDetectorAttributes(params);

	// Update the detector's copy numbers.
	currentDetector->setParentCopyNumber(userDetectors.size()-1);

	// Set true isotropic source mode for multiple detectors
	if(userDetectors.size() > 1)
		nDetParticleSource::getInstance().SetRealIsotropicMode(true);
	
	return true;
}

void nDetConstruction::setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_){
	center[0].setSegmentedPmt(col_, row_, width_, height_);
	center[1].setSegmentedPmt(col_, row_, width_, height_);
	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		iter->getCenterOfMassL()->setSegmentedPmt(col_, row_, width_, height_);
		iter->getCenterOfMassR()->setSegmentedPmt(col_, row_, width_, height_);
	}
	std::cout << " nDetConstruction: Set segmented PMTs with WxH=(" << width_ << " x " << height_ << ") and " << col_ << " columns and " << row_ << " rows.\n";
}

bool nDetConstruction::setPmtSpectralResponse(const char *fname){
	if(!(center[0].loadSpectralResponse(fname) && center[1].loadSpectralResponse(fname))){
		Display::ErrorPrint("Failed to load PMT spectral response from file!", "nDetConstruction");
		return false;
	}
	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		iter->getCenterOfMassL()->copySpectralResponse(&center[0]);
		iter->getCenterOfMassR()->copySpectralResponse(&center[1]);
	}
	std::cout << " nDetConstruction: Successfully loaded PMT spectral response function\n";
	return true;
}

bool nDetConstruction::setPmtGainMatrix(const char *fname){
	if(!(center[0].loadGainMatrix(fname) && center[1].loadGainMatrix(fname))){
		Display::ErrorPrint("Failed to load PMT anode gain matrix from file!", "nDetConstruction");
		return false;
	}
	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		iter->getCenterOfMassL()->copyGainMatrix(&center[0]);
		iter->getCenterOfMassR()->copyGainMatrix(&center[1]);
	}
	std::cout << " nDetConstruction: Successfully loaded PMT anode gain matrix\n";
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
  G4Box* expHall_solidV = new G4Box("expHall_solidV", expHallX, expHallY, expHallZ);

  expHall_logV  = new G4LogicalVolume(expHall_solidV, materials.fAir, "expHall_logV", 0, 0, 0);
  expHall_logV->SetVisAttributes(G4VisAttributes::Invisible);

  expHall_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), expHall_logV, "expHall_physV",0,false,0);
 
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
