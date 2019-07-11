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

#include "G4Tubs.hh"
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

#include "nDetConstruction.hh"
#include "nDetConstructionMessenger.hh"
#include "nDetThreadContainer.hh"
#include "nDetParticleSource.hh"
#include "termColors.hh"
#include "optionHandler.hh"

static const G4double inch = 2.54*cm;

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

	fNumColumns = 1;
	fNumRows = 1;
	fNumColumnsPmt = -1;
	fNumRowsPmt = -1;
	scintCopyNum = 1;

	fDetectorMessenger = new nDetConstructionMessenger(this);

	wrappingMaterial = "mylar";

	fCheckOverlaps = false;
	fPolishedInterface = true;
	fMylarThickness = 0;
	fGreaseThickness = 0.1*mm;
	fWindowThickness = 0.1*mm;
	fSensitiveThickness = 1*mm;
	fDetectorLength = 3.94*inch;
	fDetectorWidth = 6*mm;
	fTrapezoidLength = 0;
	fDiffuserLength = 0;
	fDetectorThickness = 0.24*inch;
	pmtWidth = 3*mm;
	pmtHeight = 3*mm;

	currentLayerSizeX = 0;
	currentLayerSizeY = 0;
	currentOffsetZ = 0;

	expHallX = 10*m;
	expHallY = 10*m;
	expHallZ = 10*m;

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
	for(std::vector<userAddDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		currentDetector = &(*iter);
		currentAssembly = currentDetector->getLogicalVolume();

		// Update the detector position offsets.
		currentDetector->getCurrentOffset(currentLayerSizeX, currentLayerSizeY, currentOffsetZ);

		// Generate all user-defined layers.
		iter->buildAllLayers(this);

		// Place the detector into the world.
		iter->placeDetector(expHall_logV);
	}

	// Build the shadow bar.
	if(shadowBarMaterial){
		G4Box *shadowBox = new G4Box("shadowBox", shadowBarSize.getX()/2, shadowBarSize.getY()/2, shadowBarSize.getZ()/2);
		G4LogicalVolume *shadowBox_logV = new G4LogicalVolume(shadowBox, shadowBarMaterial, "shadowBox_logV", 0, 0, 0);
		shadowBox_logV->SetVisAttributes(shadow_VisAtt);
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
	scintCopyNum = 1;
}

void nDetConstruction::UpdateGeometry(){
	// Define new one
	G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4RunManager::GetRunManager()->ReinitializeGeometry();

	if(PmtIsSegmented())
		setSegmentedPmt(fNumColumnsPmt, fNumRowsPmt, pmtWidth, pmtHeight);

	// Update the particle source
	nDetParticleSource::getInstance().SetDetector(this);
	
	for(std::vector<userAddDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
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

bool nDetConstruction::AddGeometry(const G4String &geom){
	// Build the detector.
	if(geom == "next" || geom == "module")
		buildModule();
	else if(geom == "ellipse")
		buildEllipse();
	else if(geom == "rectangle")
		buildRectangle();
	else if(geom == "cylinder")
		buildCylinder();
	else if(geom == "test")
		buildTestAssembly();
	else // Geometry name not recognized
		return false;
	
	// Set true isotropic source mode for multiple detectors
	if(userDetectors.size() > 1)
		nDetParticleSource::getInstance().SetRealIsotropicMode(true);
	
	// Attach PMTs.
	constructPSPmts();
	
	return true;
}

void nDetConstruction::SetPmtDimension(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "setPmtDimensions <sizeX> [sizeY]"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 1){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::SetPmtDimension(). Expected 1, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: setPmtDimensions <sizeX> [sizeY]\n";
		return;
	}
	G4double sizeX = strtod(args.at(0).c_str(), NULL);
	G4double sizeY = (Nargs >= 2 ? strtod(args.at(1).c_str(), NULL) : -1);
	SetPmtDimension(sizeX, sizeY);
}

void nDetConstruction::SetPmtDimension(const G4double &width, const G4double &height/*=-1*/){
	pmtWidth = width;
	pmtHeight = (height > 0 ? height : width);
}

void nDetConstruction::setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_){
	center[0].setSegmentedPmt(col_, row_, width_, height_);
	center[1].setSegmentedPmt(col_, row_, width_, height_);
	for(std::vector<userAddDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
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
	for(std::vector<userAddDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
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
	for(std::vector<userAddDetector>::iterator iter = userDetectors.begin(); iter != userDetectors.end(); iter++){
		iter->getCenterOfMassL()->copyGainMatrix(&center[0]);
		iter->getCenterOfMassR()->copyGainMatrix(&center[1]);
	}
	std::cout << " nDetConstruction: Successfully loaded PMT anode gain matrix\n";
	return true;
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
	detectorRotation = G4RotationMatrix();
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
} // end of buildExpHall function

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
		currentSolid->placeSolid(currentAssembly, fCheckOverlaps);
	
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
	loadLightGuide(args.at(0), rotation, args.at(4), materials.fMylarOpSurf);
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
		currentSolid->placeSolid(currentAssembly, fCheckOverlaps);
		
		// And on the negative z side.
		G4RotationMatrix *trapRot = new G4RotationMatrix();
		trapRot->rotateX(rotation.getX()-pi);
		currentSolid->placeSolid(trapRot, G4ThreeVector(0, 0, -trapezoidZ), currentAssembly, fCheckOverlaps);
	}

	currentLayerSizeX = currentSolid->getWidth();
	currentLayerSizeY = currentSolid->getThickness();
	currentOffsetZ += currentSolid->getLength();
	fTrapezoidLength = currentSolid->getLength()*mm;

	return currentSolid->getLogicalVolume();
}

void nDetConstruction::AddGDML(const G4String &input){
	currentDetector->addLayer(userAddLayer(input, &nDetConstruction::loadGDML));
}

void nDetConstruction::AddLightGuideGDML(const G4String &input){
	currentDetector->addLayer(userAddLayer(input, &nDetConstruction::loadLightGuide));
}

void nDetConstruction::AddGrease(const G4String &input){
	currentDetector->addLayer(userAddLayer(input, &nDetConstruction::applyGreaseLayer));
}

void nDetConstruction::AddDiffuser(const G4String &input){
	currentDetector->addLayer(userAddLayer(input, &nDetConstruction::applyDiffuserLayer));
}

void nDetConstruction::AddLightGuide(const G4String &input){
	currentDetector->addLayer(userAddLayer(input, &nDetConstruction::applyLightGuide));
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
		this->SetPositionCylindrical(G4ThreeVector(r0, startTheta+dTheta*i, 0));
		this->SetRotation(G4ThreeVector(90, 0, startTheta+dTheta*i));
		this->AddGeometry(args.at(0));
	}
}

void nDetConstruction::SetLightYieldMultiplier(const G4double &yield){ 
	materials.setLightYield(yield);
}

void nDetConstruction::buildModule(){
	//const G4double cellWidth = (fDetectorWidth-2*fNumColumns*fMylarThickness)/fNumColumns;
	//const G4double cellHeight = (fDetectorThickness-2*fNumRows*fMylarThickness)/fNumRows;
	const G4double cellWidth = (fDetectorWidth-(fNumColumns-1)*fMylarThickness)/fNumColumns;
	const G4double cellHeight = (fDetectorThickness-(fNumRows-1)*fMylarThickness)/fNumRows;

	// Construct the assembly bounding box.
	G4ThreeVector assemblyBoundingBox;
	constructAssembly(assemblyBoundingBox);

	// Update the detector's number of rows and columns.
	currentDetector->setSegmentedDetector(fNumColumns, fNumRows, scintCopyNum);

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
		wrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), wrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);
		
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
			cellScint_physV[col][row] = new G4PVPlacement(0, cellCenter, cellScint_logV, stream.str().c_str(), currentAssembly, 0, scintCopyNum + col*fNumRows+row, fCheckOverlaps); 
			scintBody_physV.push_back(cellScint_physV[col][row]);
		
			// Place vertical and horizontal reflectors.
			if(fMylarThickness > 0){ 
				if(row == 0 && col != fNumColumns-1){ // New vertical reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col;
					mylarVertLayer_physV[col] = new G4PVPlacement(0, G4ThreeVector(cellCenter.getX()+cellWidth/2+fMylarThickness/2, 0, 0), mylarVertLayer_logV, stream2.str().c_str(), currentAssembly, 0, 0, fCheckOverlaps);
				}
				if(row != fNumRows-1){ // New horizontal reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col << "," << row;
					mylarHorizLayer_physV[col][row] = new G4PVPlacement(0, G4ThreeVector(cellCenter.getX(), cellCenter.getY()+cellHeight/2+fMylarThickness/2, 0), mylarHorizLayer_logV, stream2.str().c_str(), currentAssembly, 0, 0, fCheckOverlaps);
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
	
	// Update the scintillator copy number.
	scintCopyNum += fNumRows*fNumColumns;
}

void nDetConstruction::buildEllipse(){
	const G4double angle = 60*deg;

	// Width of the detector (defined by the trapezoid length and SiPM dimensions).
	fDetectorWidth = 2*(pmtWidth/2+(fTrapezoidLength/std::tan(angle)))*mm;
	G4double deltaWidth = fMylarThickness/std::sin(angle);

	// Construct the assembly bounding box.
	G4ThreeVector assemblyBoundingBox;
	constructAssembly(assemblyBoundingBox);

	// Create the geometry.
	G4Trd *innerTrapezoid = new G4Trd("innerTrapezoid", fDetectorWidth/2, pmtWidth/2, fDetectorThickness/2, pmtHeight/2, fTrapezoidLength/2);
	G4Trd *outerTrapezoid = new G4Trd("outerTrapezoid", fDetectorWidth/2+deltaWidth, pmtWidth/2+deltaWidth, fDetectorThickness/2+deltaWidth, pmtHeight/2+deltaWidth, fTrapezoidLength/2);
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
	G4PVPlacement *ellipseBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), scint_logV, "Scint", currentAssembly, true, 0, fCheckOverlaps);

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
		ellipseWrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), wrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);	
	}

	// Account for the trapezoids.
	currentOffsetZ = fDetectorLength/2 + fTrapezoidLength;
	
	// Update the detector position offsets.
	currentDetector->setCurrentOffsetZ(currentOffsetZ);	
	
	// Directly modify the size of the grease layer.
	currentLayerSizeX = pmtWidth;

    // Reflective wrapping.
    if(fMylarThickness > 0)
	    new G4LogicalBorderSurface("Wrapping", ellipseBody_physV, ellipseWrapping_physV, getUserOpticalSurface());
}

void nDetConstruction::buildRectangle(){
	// Construct the assembly bounding box.
	G4ThreeVector assemblyBoundingBox;
	constructAssembly(assemblyBoundingBox);

    G4Box *plateBody = new G4Box("", fDetectorWidth/2, fDetectorThickness/2, fDetectorLength/2);
    G4LogicalVolume *plateBody_logV = new G4LogicalVolume(plateBody, getUserDetectorMaterial(), "plateBody_logV");
    plateBody_logV->SetVisAttributes(scint_VisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *plateBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), plateBody_logV, "Scint", currentAssembly, true, 0, fCheckOverlaps);

	scintBody_physV.push_back(plateBody_physV);

	// Build the wrapping.
	if(fMylarThickness > 0){
		G4Box *plateWrappingBox = new G4Box("", fDetectorWidth/2 + fMylarThickness, fDetectorThickness/2 + fMylarThickness, fDetectorLength/2);
		G4SubtractionSolid *plateWrapping = new G4SubtractionSolid("", plateWrappingBox, plateBody);
		G4LogicalVolume *plateWrapping_logV = new G4LogicalVolume(plateWrapping, getUserSurfaceMaterial(), "plateWrapping_logV");
		plateWrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *plateWrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), plateWrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);	
		
		// Reflective wrapping.
		new G4LogicalBorderSurface("Wrapping", plateBody_physV, plateWrapping_physV, getUserOpticalSurface());
	}
}

void nDetConstruction::buildCylinder(){
	// Make sure the height and width match
	fDetectorThickness = fDetectorWidth;

	// Construct the assembly bounding box
	G4ThreeVector assemblyBoundingBox;
	constructAssembly(assemblyBoundingBox);

    G4Tubs *cylinderBody = new G4Tubs("", 0, fDetectorWidth/2, fDetectorLength/2, 0, 2*pi);
    G4LogicalVolume *cylinderBody_logV = new G4LogicalVolume(cylinderBody, getUserDetectorMaterial(), "cylinderBody_logV");
    cylinderBody_logV->SetVisAttributes(scint_VisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *cylinderBody_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), cylinderBody_logV, "Scint", currentAssembly, true, 0, fCheckOverlaps);

	scintBody_physV.push_back(cylinderBody_physV);

	// Build the wrapping.
	if(fMylarThickness > 0){
		G4Tubs *cylinderWrappingBox = new G4Tubs("", 0, fDetectorWidth/2 + fMylarThickness, fDetectorLength/2, 0, 2*pi);
		G4SubtractionSolid *cylinderWrapping = new G4SubtractionSolid("", cylinderWrappingBox, cylinderBody);
		G4LogicalVolume *cylinderWrapping_logV = new G4LogicalVolume(cylinderWrapping, getUserSurfaceMaterial(), "cylinderWrapping_logV");
		cylinderWrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *cylinderWrapping_physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), cylinderWrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);	
		
		// Reflective wrapping.
		new G4LogicalBorderSurface("Wrapping", cylinderBody_physV, cylinderWrapping_physV, getUserOpticalSurface());
	}
}

void nDetConstruction::buildTestAssembly(){
}

G4LogicalVolume *nDetConstruction::constructAssembly(G4ThreeVector &boundingBox){
	G4ThreeVector pmtBoundingBox = getPSPmtBoundingBox();
	G4double assemblyLength = fDetectorLength + 2*(pmtBoundingBox.getZ());
	G4double assemblyWidth = fDetectorWidth + 2*fMylarThickness;
	G4double assemblyThickness = fDetectorThickness + 2*fMylarThickness;

	assemblyWidth = std::max(assemblyWidth, pmtBoundingBox.getX());
	assemblyThickness = std::max(assemblyThickness, pmtBoundingBox.getY());

	G4Box *assembly = new G4Box("assembly", assemblyWidth/2, assemblyThickness/2, assemblyLength/2);
	currentAssembly = new G4LogicalVolume(assembly, materials.fAir, "assembly_logV");
	currentAssembly->SetVisAttributes(assembly_VisAtt);

	// Add the assembly to the vector of detectors.
	userDetectors.push_back(userAddDetector(this));
	currentDetector = &userDetectors.back();

	currentLayerSizeX = fDetectorWidth;
	currentLayerSizeY = fDetectorThickness;
	currentOffsetZ = fDetectorLength/2;

	// Update the position and rotation of the detector.
	currentDetector->setPositionAndRotation(detectorPosition, detectorRotation);
	currentDetector->setCurrentOffset(currentLayerSizeX, currentLayerSizeY, currentOffsetZ);

	// Update the detector's copy numbers.
	currentDetector->setParentCopyNumber(userDetectors.size()-1);

	boundingBox = G4ThreeVector(assemblyWidth, assemblyThickness, assemblyLength);

	return currentAssembly;
}

void nDetConstruction::constructPSPmts(){
	// Build the sensitive PMT surfaces.
	const G4String name = "psSiPM";

	G4double sensitiveZ = currentOffsetZ + fGreaseThickness + fWindowThickness + fSensitiveThickness/2;
	G4double wrappingThickness = fGreaseThickness + fWindowThickness;

	G4int leftSideCopyNum = currentDetector->getLeftPmtCopyNumber();
	G4int rightSideCopyNum = currentDetector->getRightPmtCopyNumber();

	// The optical grease layer.
	G4PVPlacement *grease_physV[2] = {NULL, NULL};
	if(fGreaseThickness > 0){
		G4double greaseZ = currentOffsetZ + fGreaseThickness/2;
	
		G4Box* grease_solidV = new G4Box("window_solidV", pmtWidth/2, pmtHeight/2, fGreaseThickness/2);
		G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, materials.fGrease, "grease_logV");
		
		grease_logV->SetVisAttributes(grease_VisAtt);

		grease_physV[0] = new G4PVPlacement(0, G4ThreeVector(0, 0, greaseZ), grease_logV, "Grease", currentAssembly, true, leftSideCopyNum, fCheckOverlaps);
		grease_physV[1] = new G4PVPlacement(0, G4ThreeVector(0, 0, -greaseZ), grease_logV, "Grease", currentAssembly, true, rightSideCopyNum, fCheckOverlaps);
		
		if(!fPolishedInterface){
			for(std::vector<G4PVPlacement*>::iterator iter = scintBody_physV.begin(); iter != scintBody_physV.end(); iter++){
				new G4LogicalBorderSurface("GreaseInterface", (*iter), grease_physV[0], materials.fGreaseOpSurf);
				new G4LogicalBorderSurface("GreaseInterface", (*iter), grease_physV[1], materials.fGreaseOpSurf);
			}
		}
		
		// Clear all scintillator placements.
		scintBody_physV.clear();
	}

	G4PVPlacement *window_physV[2] = {NULL, NULL};
	if(fWindowThickness > 0){ // The quartz window
		G4double windowZ = currentOffsetZ + fGreaseThickness + fWindowThickness/2;
	
		G4Box* window_solidV = new G4Box("window_solidV", pmtWidth/2, pmtHeight/2, fWindowThickness/2);
		G4LogicalVolume *window_logV = new G4LogicalVolume(window_solidV, materials.fSiO2, "window_logV");
		
		window_logV->SetVisAttributes(window_VisAtt);

		window_physV[0] = new G4PVPlacement(0, G4ThreeVector(0, 0, windowZ), window_logV, "Quartz", currentAssembly, true, leftSideCopyNum, fCheckOverlaps);
		window_physV[1] = new G4PVPlacement(0, G4ThreeVector(0, 0, -windowZ), window_logV, "Quartz", currentAssembly, true, rightSideCopyNum, fCheckOverlaps);
	}

	// Build the wrapping.
	if(fMylarThickness > 0 && wrappingThickness > 0){
		G4Box *boundingBox = new G4Box("", pmtWidth/2, pmtHeight/2, wrappingThickness/2);
		G4Box *wrappingBox = new G4Box("", pmtWidth/2 + fMylarThickness, pmtHeight/2 + fMylarThickness, wrappingThickness/2);
		
		G4SubtractionSolid *greaseWrapping = new G4SubtractionSolid("", wrappingBox, boundingBox);
		G4LogicalVolume *greaseWrapping_logV = new G4LogicalVolume(greaseWrapping, getUserSurfaceMaterial(), "greaseWrapping_logV");
		greaseWrapping_logV->SetVisAttributes(wrapping_VisAtt);
		
		G4double wrappingZ = currentOffsetZ + fGreaseThickness/2 + fWindowThickness/2;

		// Place the wrapping around the scintillator.
		G4PVPlacement *greaseWrapping_physV[2];
		greaseWrapping_physV[0] = new G4PVPlacement(0, G4ThreeVector(0, 0, wrappingZ), greaseWrapping_logV, "Wrapping", currentAssembly, true, leftSideCopyNum, fCheckOverlaps);		
		greaseWrapping_physV[1] = new G4PVPlacement(0, G4ThreeVector(0, 0, -wrappingZ), greaseWrapping_logV, "Wrapping", currentAssembly, true, rightSideCopyNum, fCheckOverlaps);
		
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
    G4Box *sensitive_solidV = new G4Box(name+"_solidV", pmtWidth/2, pmtHeight/2, fSensitiveThickness/2);
    G4LogicalVolume *sensitive_logV = new G4LogicalVolume(sensitive_solidV, materials.fSilicon, name+"_logV");
    sensitive_logV->SetVisAttributes(sensitive_VisAtt);
    
    // Logical skin surface.
    new G4LogicalSkinSurface(name, sensitive_logV, materials.fSiliconOpSurf);    

	new G4PVPlacement(0, G4ThreeVector(0, 0, sensitiveZ), sensitive_logV, name, currentAssembly, true, leftSideCopyNum, fCheckOverlaps);
	new G4PVPlacement(0, G4ThreeVector(0, 0, -sensitiveZ), sensitive_logV, name, currentAssembly, true, rightSideCopyNum, fCheckOverlaps);

	/*// Physically segmented PMT.
 	for(int col = 0; col < fNumColumns; col++){
		for(int row = 0; row < fNumRows; row++){
			//G4ThreeVector cellCenter(-fDetectorWidth/2 + (2*col+1)*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + (2*row+1)*fMylarThickness + (row+0.5)*cellHeight, 0);
			G4ThreeVector cellCenterL(-fDetectorWidth/2 + col*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + row*fMylarThickness + (row+0.5)*cellHeight, sensitiveZ);
			G4ThreeVector cellCenterR(-fDetectorWidth/2 + col*fMylarThickness + (col+0.5)*cellWidth, -fDetectorThickness/2 + row*fMylarThickness + (row+0.5)*cellHeight, -sensitiveZ);

			// Copy numbers (segment IDs), indexed from 1
			std::stringstream stream; stream << name << "-" << col << "," << row;
			new G4PVPlacement(0, cellCenterL, sensitive_logV, stream.str().c_str(), currentAssembly, 0, col*fNumRows+row+1, fCheckOverlaps); 
			new G4PVPlacement(0, cellCenterR, sensitive_logV, stream.str().c_str(), currentAssembly, 0, col*fNumRows+row+1, fCheckOverlaps);
		}
	}*/
    
    // Move the current offset past the PMT
	currentLayerSizeX = pmtWidth;
	currentLayerSizeY = pmtHeight;
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
	    G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, materials.fGrease, "grease_logV");
	    grease_logV->SetVisAttributes(grease_VisAtt);
	
	    new G4PVPlacement(0, G4ThreeVector(0, 0, currentOffsetZ+thickness/2), grease_logV, "Grease", currentAssembly, true, 0, fCheckOverlaps);
	    new G4PVPlacement(0, G4ThreeVector(0, 0, -currentOffsetZ-thickness/2), grease_logV, "Grease", currentAssembly, true, 0, fCheckOverlaps);
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
        G4LogicalVolume *lightDiffuserLog = new G4LogicalVolume(lightDiffuser, materials.fSiO2, "lightDiffuser_logV");

        new G4PVPlacement(0, G4ThreeVector(0, 0, currentOffsetZ+thickness/2), lightDiffuserLog, "Diffuser", currentAssembly, true, 0, fCheckOverlaps);
        new G4PVPlacement(0, G4ThreeVector(0, 0, -currentOffsetZ-thickness/2), lightDiffuserLog, "Diffuser", currentAssembly, true, 0, fCheckOverlaps);
        
    	// Offset the other layers to account for the light-diffuser and another layer of grease.
		currentLayerSizeX = x;
		currentLayerSizeY = y;
    	currentOffsetZ += thickness;
    }
}

void nDetConstruction::applyLightGuide(){
	this->applyLightGuide(currentLayerSizeX, pmtWidth, currentLayerSizeY, pmtHeight, fTrapezoidLength);
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
	    G4LogicalVolume *lightGuideLog = new G4LogicalVolume(lightGuide, materials.fSiO2, "lightGuide_logV");	

		trapRot[1]->rotateY(180*deg);

		// Place the light guides.
		G4PVPlacement *trapPhysicalL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), lightGuideLog, trapName, currentAssembly, true, 0, fCheckOverlaps);
		G4PVPlacement *trapPhysicalR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), lightGuideLog, trapName, currentAssembly, true, 0, fCheckOverlaps);

		// Build the wrapping.
		if(fMylarThickness > 0){
			G4Trd *wrappingSolid = new G4Trd("wrapping", x1/2+deltaX, x2/2+deltaY, y1/2+deltaX, y2/2+deltaY, thickness/2);
			G4SubtractionSolid *wrapping = new G4SubtractionSolid("wrapping", wrappingSolid, lightGuide);
			G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrapping, getUserSurfaceMaterial(), "wrapping_logV");		
			wrapping_logV->SetVisAttributes(wrapping_VisAtt);

			// Place the wrapping around the light guides.
			G4PVPlacement *trapWrappingL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), wrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);
			G4PVPlacement *trapWrappingR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), wrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);
		
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
	    	currentSolid->placeSolid(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), currentAssembly, trapPhysicalL);
	    	currentSolid->placeSolid(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), currentAssembly, trapPhysicalR);
			currentSolid->setLogicalBorders("Wrapping", getUserOpticalSurface(), trapPhysicalL);
			currentSolid->setLogicalBorders("Wrapping", getUserOpticalSurface(), trapPhysicalR);	 

			// Build the wrapping.
			if(fMylarThickness > 0){
				G4Trd *wrappingSolid = new G4Trd("wrapping", currentSolid->getWidth()/2+deltaX, x2/2+deltaY, currentSolid->getThickness()/2+deltaX, y2/2+deltaY, thickness/2);
				G4SubtractionSolid *wrapping = new G4SubtractionSolid("wrapping", wrappingSolid, lightGuide);
				G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrapping, getUserSurfaceMaterial(), "wrapping_logV");		
				wrapping_logV->SetVisAttributes(wrapping_VisAtt);

				// Place the wrapping around the light guides.
				G4PVPlacement *trapWrappingL = new G4PVPlacement(trapRot[0], G4ThreeVector(0, 0, trapezoidZ), wrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);
				G4PVPlacement *trapWrappingR = new G4PVPlacement(trapRot[1], G4ThreeVector(0, 0, -trapezoidZ), wrapping_logV, "Wrapping", currentAssembly, true, 0, fCheckOverlaps);
			
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
	G4double boundingX = std::max(pmtWidth, fDetectorWidth);
	G4double boundingY = std::max(pmtHeight, fDetectorThickness);
	G4double boundingZ = fGreaseThickness + fWindowThickness + fSensitiveThickness;

	if(fDiffuserLength > 0) // Account for light diffusers
	    boundingZ += fDiffuserLength + fGreaseThickness;
	if(fTrapezoidLength > 0) // Account for light guides
	    boundingZ += fTrapezoidLength + fGreaseThickness;

	return G4ThreeVector(boundingX, boundingY, boundingZ);
}

G4Material* nDetConstruction::getUserDetectorMaterial(){
	return materials.getUserDetectorMaterial(detectorMaterial);
}

G4Material* nDetConstruction::getUserSurfaceMaterial(){
	return materials.getUserSurfaceMaterial(wrappingMaterial);
}

G4OpticalSurface* nDetConstruction::getUserOpticalSurface(){
	return materials.getUserOpticalSurface(wrappingMaterial);
}

///////////////////////////////////////////////////////////////////////////////
// class userAddDetector
///////////////////////////////////////////////////////////////////////////////

userAddDetector::userAddDetector(nDetConstruction *detector) : assembly_logV(NULL), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0), parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), numColumns(1), numRows(1), isStart(false) {
	assembly_logV = detector->getCurrentAssembly();
	copyCenterOfMass(*detector->GetCenterOfMassL(), *detector->GetCenterOfMassR());
}

void userAddDetector::getCurrentOffset(G4double &x_, G4double &y_, G4double &z_){
	x_ = layerSizeX;
	y_ = layerSizeY;
	z_ = offsetZ;
}

void userAddDetector::setPositionAndRotation(const G4ThreeVector &pos, const G4RotationMatrix &rot){
	position = pos;
	rotation = rot;
}

void userAddDetector::setCurrentOffset(const G4double &x_, const G4double &y_, const G4double &z_){
	layerSizeX = x_;
	layerSizeY = y_;
	offsetZ = z_;
}

void userAddDetector::buildAllLayers(nDetConstruction *detector){
	for(std::vector<userAddLayer>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		iter->execute(detector);
	}		
}

void userAddDetector::placeDetector(G4LogicalVolume *parent){
	assembly_physV = new G4PVPlacement(&rotation, position, assembly_logV, "Assembly", parent, 0, 0, false);
	assembly_physV->SetCopyNo(parentCopyNum);
}

void userAddDetector::clear(){
	cmL.clear();
	cmR.clear();
}

void userAddDetector::copyCenterOfMass(const centerOfMass &left, const centerOfMass &right){
	cmL = left;
	cmR = right;
}

bool userAddDetector::checkPmtCopyNumber(const G4int &num, bool &isLeft) const { 
	isLeft = (num % 2 == 0);
	return (num == 2*parentCopyNum || num == 2*parentCopyNum+1);
}

bool userAddDetector::getSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const {
	if(!this->checkCopyNumber(copyNum)) return false;
	col = (copyNum-firstSegmentCopyNum) / numRows;
	row = (copyNum-firstSegmentCopyNum) % numRows;
	return true;
}

void userAddDetector::setSegmentedDetector(const G4int &col, const G4int &row, const G4int &startCopyNum/*=0*/){
	firstSegmentCopyNum = startCopyNum; 
	lastSegmentCopyNum = startCopyNum+col*row;
	numColumns = col;
	numRows = row;
}
