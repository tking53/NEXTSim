#include "G4LogicalVolume.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VPhysicalVolume.hh"

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SubtractionSolid.hh"
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
#include "nDetWorld.hh"
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
	currentDetector = NULL;

	fDetectorMessenger = new nDetConstructionMessenger(this);

	fCheckOverlaps = false;

	shadowBarMaterial = NULL;
	
	// Initialize the detector parameter messenger
	params.InitializeMessenger();
	
	expHall = new nDetWorld();
}

nDetConstruction::~nDetConstruction(){
}

G4VPhysicalVolume* nDetConstruction::Construct(){
	if(!expHall->getPhysicalVolume())
		this->ConstructDetector();
	
	return expHall->getPhysicalVolume();
}
	
G4VPhysicalVolume* nDetConstruction::ConstructDetector(){
	if(!materials.materialsAreDefined())
		materials.initialize();

	// Build experiment hall.
	expHall->buildExpHall(&materials);

	// Place all detectors.
	for(std::vector<nDetDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		currentDetector = &(*iter);

		// Build the detector
		iter->construct();

		// Place the detector into the world.
		iter->placeDetector(expHall->getLogicalVolume());
	}

	// Place all external GDML solids
	for(std::vector<gdmlSolid>::iterator iter = solids.begin(); iter != solids.end(); iter++){
		iter->placeSolid(expHall->getLogicalVolume());
	}	

	// Build the shadow bar.
	if(shadowBarMaterial){
		G4Box *shadowBox = new G4Box("shadowBox", shadowBarSize.getX()/2, shadowBarSize.getY()/2, shadowBarSize.getZ()/2);
		G4LogicalVolume *shadowBox_logV = new G4LogicalVolume(shadowBox, shadowBarMaterial, "shadowBox_logV", 0, 0, 0);
		shadowBox_logV->SetVisAttributes(materials.visShadow);
		new G4PVPlacement(0, shadowBarPos, shadowBox_logV, "ShadowBar", expHall->getLogicalVolume(), true, 0, fCheckOverlaps);
	}

	return expHall->getPhysicalVolume();
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
	expHall->reset();
	
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
