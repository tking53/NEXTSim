
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"

#include "nDetDetector.hh"
#include "nDetConstruction.hh"
#include "nDetDetectorMessenger.hh"
#include "optionHandler.hh" // split_str

void nDetDetectorParams::InitializeMessenger(){
	if(fMessenger) 
		return;
	fMessenger = new nDetDetectorMessenger(this);
}

void nDetDetectorParams::SetPmtDimension(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "setPmtDimensions <sizeX> [sizeY]"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 1){
		std::cout << " nDetDetectorParams: Invalid number of arguments given to ::SetPmtDimension(). Expected 1, received " << Nargs << ".\n";
		std::cout << " nDetDetectorParams:  SYNTAX: setPmtDimensions <sizeX> [sizeY]\n";
		return;
	}
	G4double sizeX = strtod(args.at(0).c_str(), NULL);
	G4double sizeY = (Nargs >= 2 ? strtod(args.at(1).c_str(), NULL) : -1);
	SetPmtDimension(sizeX, sizeY);
}

void nDetDetectorParams::SetPmtDimension(const G4double &width, const G4double &height/*=-1*/){
	pmtWidth = width;
	pmtHeight = (height > 0 ? height : width);
}

void nDetDetectorParams::SetPositionCylindrical(const G4ThreeVector &position){ 
	double x = position.getX()*std::cos(position.getY()*deg);
	double z = position.getX()*std::sin(position.getY()*deg);
	detectorPosition = G4ThreeVector(x*cm, position.getZ()*cm, z*cm);
}

void nDetDetectorParams::SetPositionSpherical(const G4ThreeVector &position){ 
	double x = position.getX()*std::sin(position.getY()*deg)*std::cos(position.getZ()*deg); 
	double y = position.getX()*std::sin(position.getY()*deg)*std::sin(position.getZ()*deg); 
	double z = position.getX()*std::cos(position.getY()*deg);
	detectorPosition = G4ThreeVector(x*cm, y*cm, z*cm);
}

void nDetDetectorParams::SetRotation(const G4ThreeVector &rotation){
	detectorRotation = G4RotationMatrix();
	detectorRotation.rotateX(rotation.getX()*deg);
	detectorRotation.rotateY(rotation.getY()*deg); 
	detectorRotation.rotateZ(rotation.getZ()*deg);  
}


nDetDetector::nDetDetector(nDetConstruction *detector, nDetMaterials *matptr) : nDetDetectorParams(detector->GetDetectorParameters()),
                                                                                assembly_logV(NULL), assembly_physV(NULL), layerSizeX(0), layerSizeY(0), offsetZ(0),
	                                                                            parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), 
	                                                                            checkOverlaps(false), geomType(GEOM_RECTANGLE), materials(matptr) 
{
	copyCenterOfMass(*detector->GetCenterOfMassL(), *detector->GetCenterOfMassR());
}

nDetDetector::~nDetDetector(){
	/*for(std::vector<userAddLayer*>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		delete (*iter); // This causes a seg-fault
	}
	userLayers.clear();*/
}

void nDetDetector::getCurrentOffset(G4double &x_, G4double &y_, G4double &z_){
	x_ = layerSizeX;
	y_ = layerSizeY;
	z_ = offsetZ;
}

void nDetDetector::setPositionAndRotation(const G4ThreeVector &pos, const G4RotationMatrix &rot){
	detectorPosition = pos;
	detectorRotation = rot;
}

void nDetDetector::setCurrentOffset(const G4double &x_, const G4double &y_, const G4double &z_){
	layerSizeX = x_;
	layerSizeY = y_;
	offsetZ = z_;
}

void nDetDetector::buildAllLayers(){
	for(std::vector<userAddLayer*>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		(*iter)->construct(this);
	}	
}

void nDetDetector::placeDetector(G4LogicalVolume *parent){
	assembly_physV = new G4PVPlacement(&detectorRotation, detectorPosition, assembly_logV, "Assembly", parent, 0, 0, false);
	assembly_physV->SetCopyNo(parentCopyNum);
}

void nDetDetector::clear(){
	cmL.clear();
	cmR.clear();
}

void nDetDetector::copyCenterOfMass(const centerOfMass &left, const centerOfMass &right){
	cmL = left;
	cmR = right;
}

bool nDetDetector::checkPmtCopyNumber(const G4int &num, bool &isLeft) const { 
	isLeft = (num % 2 == 0);
	return (num == 2*parentCopyNum || num == 2*parentCopyNum+1);
}

bool nDetDetector::getSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const {
	if(!this->checkCopyNumber(copyNum)) return false;
	col = (copyNum-firstSegmentCopyNum) / fNumRows;
	row = (copyNum-firstSegmentCopyNum) % fNumRows;
	return true;
}

void nDetDetector::addGDML(const G4String &input){
	addLayer(new gdmlLayer(input));
}

void nDetDetector::addLightGuideGDML(const G4String &input){
	addLayer(new gdmlLightGuideLayer(input));
}

void nDetDetector::addGreaseLayer(const G4String &input){
	addLayer(new greaseLayer(input));
}

void nDetDetector::addDiffuserLayer(const G4String &input){
	addLayer(new diffuserLayer(input));
}

void nDetDetector::addLightGuideLayer(const G4String &input){
	addLayer(new lightGuideLayer(input));
}

bool nDetDetector::setGeometry(const G4String &geom){
	// Build the detector.
	if(geom == "next" || geom == "module")
		geomType = GEOM_MODULE;
	else if(geom == "ellipse")
		geomType = GEOM_ELLIPSE;
	else if(geom == "rectangle")
		geomType = GEOM_RECTANGLE;
	else if(geom == "cylinder")
		geomType = GEOM_CYLINDER;
	else if(geom == "test")
		geomType = GEOM_TEST;
	else // Geometry name not recognized
		return false;
	return true;
}

void nDetDetector::construct(){
	// Build the assembly volume
	constructAssembly();

	// Build the detector.
	if(geomType == GEOM_MODULE)
		buildModule();
	else if(geomType == GEOM_ELLIPSE)
		buildEllipse();
	else if(geomType == GEOM_RECTANGLE)
		buildRectangle();
	else if(geomType == GEOM_CYLINDER)
		buildCylinder();
	else if(geomType == GEOM_TEST)
		buildTestAssembly();
	else // Geometry not recognized. Do nothing
		return;
	
	// Generate all user-defined layers.
	buildAllLayers();

	// Attach PMTs.
	constructPSPmts();
}

G4LogicalVolume *nDetDetector::constructAssembly(){
	// Calculate the dimensions of the detector
	G4double assemblyLength = fDetectorLength + 2*(fGreaseThickness+fWindowThickness+fSensitiveThickness);
	G4double assemblyWidth = fDetectorWidth + 2*fWrappingThickness;
	G4double assemblyHeight = fDetectorHeight + 2*fWrappingThickness;

	// Account for the additional component layers
	for(std::vector<userAddLayer*>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		if(!(*iter)->decodeString()){
			std::cout << " nDetDetector: Invalid number of arguments given to ::decodeString(). Expected " << (*iter)->getNumRequiredArgs() << " but received " << (*iter)->getNumSuppliedArgs() << ".\n";
			std::cout << " nDetDetector:  SYNTAX: " << (*iter)->syntaxStr() << std::endl;
			continue;
		}
		std::max(assemblyWidth, (*iter)->getSizeX());
		std::max(assemblyHeight, (*iter)->getSizeY());
		assemblyLength += 2*(*iter)->getSizeZ();
	}

	// Build the assembly box
	G4Box *assembly = new G4Box("assembly", assemblyWidth/2, assemblyHeight/2, assemblyLength/2);
	assembly_logV = new G4LogicalVolume(assembly, materials->fAir, "assembly_logV");
	assembly_logV->SetVisAttributes(materials->visAssembly);

	return assembly_logV;
}

G4PVPlacement *nDetDetector::addToDetectorBody(G4LogicalVolume *volume, const G4String &name/*=""*/, const G4ThreeVector &pos/*=G4ThreeVector(0,0,0)*/, G4RotationMatrix *rot/*=NULL*/){
	return (new G4PVPlacement(rot, pos, volume, (name.empty() ? volume->GetName() : name), assembly_logV, false, 0, checkOverlaps));
}

G4PVPlacement *nDetDetector::addSegmentToBody(G4LogicalVolume *volume, const G4String &name/*=""*/, const G4ThreeVector &pos/*=G4ThreeVector(0,0,0)*/, G4RotationMatrix *rot/*=NULL*/){
	return (new G4PVPlacement(rot, pos, volume, (name.empty() ? volume->GetName() : name), assembly_logV, false, lastSegmentCopyNum++, checkOverlaps));
}

G4PVPlacement *nDetDetector::addLeftComponent(G4LogicalVolume *volume, const G4double &offset, const G4String &name/*=""*/, G4RotationMatrix *rot/*=NULL*/){
	return (new G4PVPlacement(rot, G4ThreeVector(0, 0, offset), volume, (name.empty() ? volume->GetName() : name), assembly_logV, false, 2*parentCopyNum, checkOverlaps));
}

G4PVPlacement *nDetDetector::addRightComponent(G4LogicalVolume *volume, const G4double &offset, const G4String &name/*=""*/, G4RotationMatrix *rot/*=NULL*/){
	return (new G4PVPlacement(rot, G4ThreeVector(0, 0, -offset), volume, (name.empty() ? volume->GetName() : name), assembly_logV, false, 2*parentCopyNum+1, checkOverlaps));
}

void nDetDetector::addMirroredComponents(G4LogicalVolume *volume, const G4double &offset, const G4String &name/*=""*/, G4RotationMatrix *rot/*=NULL*/){
	addLeftComponent(volume, offset, name, rot);
	addRightComponent(volume, offset, name, rot);
}
	
void nDetDetector::addMirroredComponents(G4PVPlacement* &phys1, G4PVPlacement* &phys2, G4LogicalVolume *volume, const G4double &offset, const G4String &name/*=""*/, G4RotationMatrix *rot/*=NULL*/){
	phys1 = addLeftComponent(volume, offset, name, rot);
	phys2 = addRightComponent(volume, offset, name, rot);
}

void nDetDetector::buildModule(){
	const G4double cellWidth = GetSegmentWidth();
	const G4double cellHeight = GetSegmentHeight();

	// Get the number of rows and columns for this segmented detector
	int Ncol = fNumColumns;
	int Nrow = fNumRows;

	// Update the detector's copy numbers
	firstSegmentCopyNum = scintCopyNum; 
	lastSegmentCopyNum = firstSegmentCopyNum; //startCopyNum+col*row;

    // Construct the scintillator cell
    G4Box *cellScint = new G4Box("scintillator", cellWidth/2, cellHeight/2, fDetectorLength/2);
    G4LogicalVolume *cellScint_logV = new G4LogicalVolume(cellScint, getUserDetectorMaterial(), "scint_log");
    cellScint_logV->SetVisAttributes(materials->visScint);

	G4Box *mylarVertLayer = NULL;
	G4Box *mylarHorizLayer = NULL;
	
	G4LogicalVolume *mylarVertLayer_logV = NULL;
	G4LogicalVolume *mylarHorizLayer_logV = NULL;

	// Build the wrapping.
	G4PVPlacement *wrapping_physV = NULL;
	if(WrappingEnabled()){
		// Construct the outer wrapping.
		G4Box *wrappingBox = new G4Box("wrappingBox", fDetectorWidth/2+fWrappingThickness, fDetectorHeight/2+fWrappingThickness, fDetectorLength/2);
		G4Box *scintBox = new G4Box("scintBox", fDetectorWidth/2, fDetectorHeight/2, fDetectorLength/2);
		
		G4SubtractionSolid *wrappingBody = new G4SubtractionSolid("wrapping", wrappingBox, scintBox);
		G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrappingBody, getUserSurfaceMaterial(), "wrapping_logV");
		wrapping_logV->SetVisAttributes(materials->visWrapping);
		
		// Place the outer wrapping into the assembly.
		wrapping_physV = addToDetectorBody(wrapping_logV, "Wrapping");
		
		// Construct vertical and horizontal reflector layers for later use.
		mylarVertLayer = new G4Box("mylarVertLayer", fWrappingThickness/2, fDetectorHeight/2, fDetectorLength/2);
		mylarHorizLayer = new G4Box("mylarHorizLayer", cellWidth/2, fWrappingThickness/2, fDetectorLength/2);

		mylarVertLayer_logV = new G4LogicalVolume(mylarVertLayer, getUserSurfaceMaterial(), "mylarVertLayer_logV");
		mylarHorizLayer_logV = new G4LogicalVolume(mylarHorizLayer, getUserSurfaceMaterial(), "mylarHorizLayer_logV");
		
		mylarVertLayer_logV->SetVisAttributes(materials->visWrapping);
		mylarHorizLayer_logV->SetVisAttributes(materials->visWrapping);
	}

	// Place the scintillator segments into the assembly.
	std::vector<G4PVPlacement*> mylarVertLayer_physV(Ncol, NULL);
	std::vector<std::vector<G4PVPlacement*> > mylarHorizLayer_physV(Ncol, std::vector<G4PVPlacement*>(Nrow, NULL));
	std::vector<std::vector<G4PVPlacement*> > cellScint_physV(Ncol, std::vector<G4PVPlacement*>(Nrow, NULL));	
	for(int col = 0; col < Ncol; col++){
		for(int row = 0; row < Nrow; row++){
			G4ThreeVector cellCenter(-fDetectorWidth/2 + col*fWrappingThickness + (col+0.5)*cellWidth, -fDetectorHeight/2 + row*fWrappingThickness + (row+0.5)*cellHeight, 0);

			// Copy numbers (segment IDs), indexed from 1
			std::stringstream stream; stream << "Scint-" << col << "," << row;
			cellScint_physV[col][row] = addSegmentToBody(cellScint_logV, stream.str(), cellCenter);
			scintBody_physV.push_back(cellScint_physV[col][row]);
		
			// Place vertical and horizontal reflectors.
			if(WrappingEnabled()){ 
				if(row == 0 && col != Ncol-1){ // New vertical reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col;
					mylarVertLayer_physV[col] = addToDetectorBody(mylarVertLayer_logV, stream2.str().c_str(), G4ThreeVector(cellCenter.getX()+cellWidth/2+fWrappingThickness/2, 0, 0));
				}
				if(row != Nrow-1){ // New horizontal reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col << "," << row;
					mylarHorizLayer_physV[col][row] = addToDetectorBody(mylarHorizLayer_logV, stream2.str().c_str(), G4ThreeVector(cellCenter.getX(), cellCenter.getY()+cellHeight/2+fWrappingThickness/2, 0));
				}
			}
		}
	}

	// Define logical reflector surfaces.
	if(WrappingEnabled()){ 
		for(int col = 0; col < Ncol; col++){
			for(int row = 0; row < Nrow; row++){
				G4PVPlacement *cellPhysical = cellScint_physV[col][row];
				
				int leftCol = col-1;
				int rightCol = col+1;
				int downRow = row-1;
				int upRow = row+1;
					
				// Border with the outer wrapping.
				if((col == 0 || row == 0) || (col == Ncol-1 || row == Nrow-1)) 
					new G4LogicalBorderSurface("Wrapping", cellPhysical, wrapping_physV, getUserOpticalSurface());
				
				// Internal reflector layers.
				if(leftCol >= 0 && leftCol < Ncol) // Left side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarVertLayer_physV.at(col-1), getUserOpticalSurface());
				if(rightCol >= 0 && rightCol < Ncol) // Right side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarVertLayer_physV.at(col), getUserOpticalSurface());
				if(downRow >= 0 && downRow < Nrow) // Bottom side horizontal layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarHorizLayer_physV.at(col).at(row-1), getUserOpticalSurface());
				if(upRow >= 0 && upRow < Nrow) // Top side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarHorizLayer_physV.at(col).at(row), getUserOpticalSurface());
			}
		}
	}
	
	// Update the scintillator copy number.
	scintCopyNum += Nrow*Ncol;
	
	// Update the Z offset and layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
}

void nDetDetector::buildEllipse(){
	const G4double angle = 60*deg;

	// Width of the detector (defined by the trapezoid length and SiPM dimensions).
	fDetectorWidth = 2*(pmtWidth/2+(fTrapezoidLength/std::tan(angle)))*mm;
	G4double deltaWidth = fWrappingThickness/std::sin(angle);

	// Create the geometry.
	G4Trd *innerTrapezoid = new G4Trd("innerTrapezoid", fDetectorWidth/2, pmtWidth/2, fDetectorHeight/2, pmtHeight/2, fTrapezoidLength/2);
	G4Trd *outerTrapezoid = new G4Trd("outerTrapezoid", fDetectorWidth/2+deltaWidth, pmtWidth/2+deltaWidth, fDetectorHeight/2+deltaWidth, pmtHeight/2+deltaWidth, fTrapezoidLength/2);
	G4Box *innerBody = new G4Box("innerBody", fDetectorWidth/2, fDetectorHeight/2, fDetectorLength/2);
	G4Box *outerBody = new G4Box("outerBody", fDetectorWidth/2+deltaWidth, fDetectorHeight/2+deltaWidth, fDetectorLength/2);

	// Build the detector body using unions.
	G4RotationMatrix *trapRot = new G4RotationMatrix;
	trapRot->rotateY(180*deg);
	G4UnionSolid *scintBody1 = new G4UnionSolid("", innerBody, innerTrapezoid, 0, G4ThreeVector(0, 0, +fDetectorLength/2+fTrapezoidLength/2));
	G4UnionSolid *scintBody2 = new G4UnionSolid("scint", scintBody1, innerTrapezoid, trapRot, G4ThreeVector(0, 0, -fDetectorLength/2-fTrapezoidLength/2));
	G4LogicalVolume *scint_logV = new G4LogicalVolume(scintBody2, getUserDetectorMaterial(), "scint_logV");	
    scint_logV->SetVisAttributes(materials->visScint);

	// Place the scintillator inside the assembly.
	G4PVPlacement *ellipseBody_physV = addToDetectorBody(scint_logV, "Scint");

	scintBody_physV.push_back(ellipseBody_physV);

	// Build the wrapping.
	G4PVPlacement *ellipseWrapping_physV = NULL;
	if(WrappingEnabled()){
		G4UnionSolid *wrappingBody1 = new G4UnionSolid("", outerBody, outerTrapezoid, 0, G4ThreeVector(0, 0, +fDetectorLength/2+fTrapezoidLength/2));
		G4UnionSolid *wrappingBody2 = new G4UnionSolid("", wrappingBody1, outerTrapezoid, trapRot, G4ThreeVector(0, 0, -fDetectorLength/2-fTrapezoidLength/2));
		G4SubtractionSolid *wrappingBody3 = new G4SubtractionSolid("wrapping", wrappingBody2, scintBody2);
		G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrappingBody3, getUserSurfaceMaterial(), "wrapping_logV");		
		wrapping_logV->SetVisAttributes(materials->visWrapping);

		// Place the wrapping around the scintillator.
		ellipseWrapping_physV = addToDetectorBody(wrapping_logV, "Wrapping");
	}

	// Account for the trapezoids and update the layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2 + fTrapezoidLength;
	
	// Directly modify the size of the grease layer.
	layerSizeX = pmtWidth;

    // Reflective wrapping.
    if(WrappingEnabled())
	    new G4LogicalBorderSurface("Wrapping", ellipseBody_physV, ellipseWrapping_physV, getUserOpticalSurface());
}

void nDetDetector::buildRectangle(){
    G4Box *plateBody = new G4Box("", fDetectorWidth/2, fDetectorHeight/2, fDetectorLength/2);
    G4LogicalVolume *plateBody_logV = new G4LogicalVolume(plateBody, getUserDetectorMaterial(), "plateBody_logV");
    plateBody_logV->SetVisAttributes(materials->visScint);

	// Place the scintillator inside the assembly.
	G4PVPlacement *plateBody_physV = addToDetectorBody(plateBody_logV, "Scint");

	scintBody_physV.push_back(plateBody_physV);

	// Build the wrapping.
	if(WrappingEnabled()){
		G4Box *plateWrappingBox = new G4Box("", fDetectorWidth/2 + fWrappingThickness, fDetectorHeight/2 + fWrappingThickness, fDetectorLength/2);
		G4SubtractionSolid *plateWrapping = new G4SubtractionSolid("", plateWrappingBox, plateBody);
		G4LogicalVolume *plateWrapping_logV = new G4LogicalVolume(plateWrapping, getUserSurfaceMaterial(), "plateWrapping_logV");
		plateWrapping_logV->SetVisAttributes(materials->visWrapping);
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *plateWrapping_physV = addToDetectorBody(plateWrapping_logV, "Wrapping");
		
		// Reflective wrapping.
		new G4LogicalBorderSurface("Wrapping", plateBody_physV, plateWrapping_physV, getUserOpticalSurface());
	}
	
	// Update the Z offset and layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
}

void nDetDetector::buildCylinder(){
	// Make sure the height and width match
	fDetectorHeight = fDetectorWidth;

    G4Tubs *cylinderBody = new G4Tubs("", 0, fDetectorWidth/2, fDetectorLength/2, 0, 2*CLHEP::pi);
    G4LogicalVolume *cylinderBody_logV = new G4LogicalVolume(cylinderBody, getUserDetectorMaterial(), "cylinderBody_logV");
    cylinderBody_logV->SetVisAttributes(materials->visScint);

	// Place the scintillator inside the assembly.
	G4PVPlacement *cylinderBody_physV = addToDetectorBody(cylinderBody_logV, "Scint");

	scintBody_physV.push_back(cylinderBody_physV);

	// Build the wrapping.
	if(WrappingEnabled()){
		G4Tubs *cylinderWrappingBox = new G4Tubs("", 0, fDetectorWidth/2 + fWrappingThickness, fDetectorLength/2, 0, 2*CLHEP::pi);
		G4SubtractionSolid *cylinderWrapping = new G4SubtractionSolid("", cylinderWrappingBox, cylinderBody);
		G4LogicalVolume *cylinderWrapping_logV = new G4LogicalVolume(cylinderWrapping, getUserSurfaceMaterial(), "cylinderWrapping_logV");
		cylinderWrapping_logV->SetVisAttributes(materials->visWrapping);
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *cylinderWrapping_physV = addToDetectorBody(cylinderWrapping_logV, "Wrapping");	
		
		// Reflective wrapping.
		new G4LogicalBorderSurface("Wrapping", cylinderBody_physV, cylinderWrapping_physV, getUserOpticalSurface());
	}
	
	// Update the Z offset and layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
}

void nDetDetector::buildTestAssembly(){
}

void nDetDetector::constructPSPmts(){
	// Build the sensitive PMT surfaces.
	const G4String name = "psSiPM";

	G4double sensitiveZ = offsetZ + fGreaseThickness + fWindowThickness + fSensitiveThickness/2;
	G4double wrappingThickness = fGreaseThickness + fWindowThickness;

	// The optical grease layer.
	G4PVPlacement *grease_physV[2] = {NULL, NULL};
	if(fGreaseThickness > 0){
		G4double greaseZ = offsetZ + fGreaseThickness/2;
	
		G4CSGSolid *grease_solidV;
		if(fSquarePMTs)
			grease_solidV = new G4Box("window_solidV", pmtWidth/2, pmtHeight/2, fGreaseThickness/2);
		else
			grease_solidV = new G4Tubs("window_solidV", 0, pmtWidth/2, fGreaseThickness/2, 0, 2*CLHEP::pi);
		G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, materials->fGrease, "grease_logV");
		
		grease_logV->SetVisAttributes(materials->visGrease);

		addMirroredComponents(grease_physV[0], grease_physV[1], grease_logV, greaseZ, "Grease");
		
		if(!fPolishedInterface){
			for(std::vector<G4PVPlacement*>::iterator iter = scintBody_physV.begin(); iter != scintBody_physV.end(); iter++){
				new G4LogicalBorderSurface("GreaseInterface", (*iter), grease_physV[0], materials->fGreaseOpSurf);
				new G4LogicalBorderSurface("GreaseInterface", (*iter), grease_physV[1], materials->fGreaseOpSurf);
			}
		}
		
		// Clear all scintillator placements.
		scintBody_physV.clear();
	}

	G4PVPlacement *window_physV[2] = {NULL, NULL};
	if(fWindowThickness > 0){ // The quartz window
		G4double windowZ = offsetZ + fGreaseThickness + fWindowThickness/2;
	
		G4CSGSolid *window_solidV;
		if(fSquarePMTs)
			window_solidV = new G4Box("window_solidV", pmtWidth/2, pmtHeight/2, fWindowThickness/2);
		else
			window_solidV = new G4Tubs("window_solidV", 0, pmtWidth/2, fWindowThickness/2, 0, 2*CLHEP::pi);
		G4LogicalVolume *window_logV = new G4LogicalVolume(window_solidV, materials->fSiO2, "window_logV");
		
		window_logV->SetVisAttributes(materials->visWindow);

		addMirroredComponents(window_physV[0], window_physV[1], window_logV, windowZ, "Quartz");
	}

	// Build the wrapping.
	if(WrappingEnabled() && wrappingThickness > 0){
		G4CSGSolid *boundingBox;
		G4CSGSolid *wrappingBox;
		if(fSquarePMTs){
			boundingBox = new G4Box("", pmtWidth/2, pmtHeight/2, wrappingThickness/2);
			wrappingBox = new G4Box("", pmtWidth/2 + fWrappingThickness, pmtHeight/2 + fWrappingThickness, wrappingThickness/2);
		}
		else{
			boundingBox = new G4Tubs("", 0, pmtWidth/2, wrappingThickness/2, 0, 2*CLHEP::pi);
			wrappingBox = new G4Tubs("", 0, pmtWidth/2 + fWrappingThickness, wrappingThickness/2, 0, 2*CLHEP::pi);
		}
		
		G4SubtractionSolid *greaseWrapping = new G4SubtractionSolid("", wrappingBox, boundingBox);
		G4LogicalVolume *greaseWrapping_logV = new G4LogicalVolume(greaseWrapping, getUserSurfaceMaterial(), "greaseWrapping_logV");
		greaseWrapping_logV->SetVisAttributes(materials->visWrapping);
		
		G4double wrappingZ = offsetZ + fGreaseThickness/2 + fWindowThickness/2;

		// Place the wrapping around the scintillator.
		G4PVPlacement *greaseWrapping_physV[2];
		
		addMirroredComponents(greaseWrapping_physV[0], greaseWrapping_physV[1], greaseWrapping_logV, wrappingZ, "Wrapping");
		
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
    G4CSGSolid *sensitive_solidV;
    if(fSquarePMTs)
		sensitive_solidV = new G4Box(name+"_solidV", pmtWidth/2, pmtHeight/2, fSensitiveThickness/2);
	else
		sensitive_solidV = new G4Tubs(name+"_solidV", 0, pmtWidth/2, fSensitiveThickness/2, 0, 2*CLHEP::pi);
    G4LogicalVolume *sensitive_logV = new G4LogicalVolume(sensitive_solidV, materials->fSilicon, name+"_logV");
    sensitive_logV->SetVisAttributes(materials->visSensitive);
    
    // Logical skin surface.
    new G4LogicalSkinSurface(name, sensitive_logV, materials->fSiliconOpSurf);    

	addMirroredComponents(sensitive_logV, sensitiveZ, name);

    // Move the current offset past the PMT
	layerSizeX = pmtWidth;
	layerSizeY = pmtHeight;
    offsetZ += fGreaseThickness + fWindowThickness + fSensitiveThickness;
}

void nDetDetector::applyGreaseLayer(){
	this->applyGreaseLayer(layerSizeX, layerSizeY);
}

void nDetDetector::applyGreaseLayer(const G4double &x, const G4double &y, double thickness/*=0*/){
	if(thickness <= 0)
		thickness = fGreaseThickness;
    if(thickness > 0){
	    G4Box *grease_solidV = new G4Box("grease", x/2, y/2, thickness/2);
	    G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, materials->fGrease, "grease_logV");
	    grease_logV->SetVisAttributes(materials->visGrease);

		addMirroredComponents(grease_logV, offsetZ+thickness/2, "Grease");

		layerSizeX = x;
		layerSizeY = y;
		offsetZ += thickness;
	}
}

void nDetDetector::applyDiffuserLayer(){
	this->applyDiffuserLayer(layerSizeX, layerSizeY, fDiffuserLength);
}

void nDetDetector::applyDiffuserLayer(const G4double &x, const G4double &y, const double &thickness){
    if(thickness > 0){ // Build the light diffusers (if needed)
        G4Box *lightDiffuser = new G4Box("lightDiffuser", x/2, y/2, thickness/2);
        G4LogicalVolume *lightDiffuserLog = new G4LogicalVolume(lightDiffuser, materials->fSiO2, "lightDiffuser_logV");

		addMirroredComponents(lightDiffuserLog, offsetZ+thickness/2, "Diffuser");
        
    	// Offset the other layers to account for the light-diffuser and another layer of grease.
		layerSizeX = x;
		layerSizeY = y;
    	offsetZ += thickness;
    }
}

void nDetDetector::applyLightGuide(){
	this->applyLightGuide(layerSizeX, pmtWidth, layerSizeY, pmtHeight, fTrapezoidLength);
}

void nDetDetector::applyLightGuide(const G4double &x2, const G4double &y2){
	this->applyLightGuide(layerSizeX, x2, layerSizeY, y2, fTrapezoidLength);
}

void nDetDetector::applyLightGuide(const G4double &x1, const G4double &x2, const G4double &y1, const G4double &y2, const double &thickness){
    if(thickness > 0){ // Build the light guides (if needed)
        const G4double trapAngleXZ = std::atan2(2*thickness, x1-x2);
        const G4double trapAngleYZ = std::atan2(2*thickness, y1-y2);
        
        const G4double deltaX = fWrappingThickness/std::sin(trapAngleXZ);
        const G4double deltaY = fWrappingThickness/std::sin(trapAngleYZ);
        
        G4double trapezoidZ = offsetZ + thickness/2;
        
        std::string trapName = "Acrylic";
        
        // Build the light-guide.
	    G4Trd *lightGuide = new G4Trd("lightGuide", x1/2, x2/2, y1/2, y2/2, thickness/2);
	    G4LogicalVolume *lightGuideLog = new G4LogicalVolume(lightGuide, materials->fSiO2, "lightGuide_logV");	

		// Place the light guides.
		G4PVPlacement *trapPhysicalL, *trapPhysicalR;
		addMirroredComponents(trapPhysicalL, trapPhysicalR, lightGuideLog, trapezoidZ, trapName);

		// Build the wrapping.
		if(WrappingEnabled()){
			G4Trd *wrappingSolid = new G4Trd("wrapping", x1/2+deltaX, x2/2+deltaY, y1/2+deltaX, y2/2+deltaY, thickness/2);
			G4SubtractionSolid *wrapping = new G4SubtractionSolid("wrapping", wrappingSolid, lightGuide);
			G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrapping, getUserSurfaceMaterial(), "wrapping_logV");		
			wrapping_logV->SetVisAttributes(materials->visWrapping);

			// Place the wrapping around the light guides.
			G4PVPlacement *trapWrappingR, *trapWrappingL;
			addMirroredComponents(trapWrappingL, trapWrappingR, wrapping_logV, trapezoidZ, trapName);
		
			// Reflective wrapping.
			new G4LogicalBorderSurface("Wrapping", trapPhysicalL, trapWrappingL, getUserOpticalSurface());
			new G4LogicalBorderSurface("Wrapping", trapPhysicalR, trapWrappingR, getUserOpticalSurface());
		}
		
		layerSizeX = x2;
		layerSizeY = y2;
    	offsetZ += thickness;
	}
}

void nDetDetector::loadGDML(gdmlSolid *solid){
	if(!solid || !solid->isLoaded()) 
		return;

	std::cout << " nDetDetector: Loaded GDML model (name=" << solid->getName() << ") with size x=" << solid->getWidth() << " mm, y=" << solid->getThickness() << " mm, z=" << solid->getLength() << " mm\n";
	
	// Place loaded model into the assembly.
	solid->placeSolid(assembly_logV, checkOverlaps);
}

void nDetDetector::loadLightGuide(gdmlSolid *solid, const G4ThreeVector &rotation){
	if(!solid || !solid->isLoaded()) 
		return;

	// Set internal reflectors
	solid->setLogicalBorders("InnerWrapping", materials->fEsrOpSurf);

	G4double trapezoidZ = offsetZ + solid->getLength()/2;
	std::cout << " nDetConstruction: Loaded GDML model (name=" << solid->getName() << ") with size x=" << solid->getWidth() << " mm, y=" << solid->getThickness() << " mm, z=" << solid->getLength() << " mm\n";

	// Place loaded model into the assembly.
	// Place the light-guide on the positive z side.
	solid->setPosition(G4ThreeVector(0, 0, trapezoidZ));
	solid->placeSolid(assembly_logV, checkOverlaps);
	
	// And on the negative z side.
	G4RotationMatrix *trapRot = new G4RotationMatrix();
	trapRot->rotateX(rotation.getX()-CLHEP::pi);
	solid->placeSolid(trapRot, G4ThreeVector(0, 0, -trapezoidZ), assembly_logV, checkOverlaps);

	layerSizeX = solid->getWidth();
	layerSizeY = solid->getThickness();
	offsetZ += solid->getLength();
	fTrapezoidLength = solid->getLength()*mm;
}

G4Material* nDetDetector::getUserDetectorMaterial(){
	return materials->getUserDetectorMaterial(detectorMaterial);
}

G4Material* nDetDetector::getUserSurfaceMaterial(){
	return materials->getUserSurfaceMaterial(wrappingMaterial);
}

G4OpticalSurface* nDetDetector::getUserOpticalSurface(){
	return materials->getUserOpticalSurface(wrappingMaterial);
}

bool greaseLayer::decodeString(){
	// Expects a space-delimited string of the form:
	//  "addGreaseLayer width(mm) height(mm) thickness(mm)"
	std::vector<std::string> args;
	nUserArgs = split_str(argStr, args);
	if(nUserArgs < nReqArgs)
		return false;
	x = strtod(args.at(0).c_str(), NULL);
	y = strtod(args.at(1).c_str(), NULL);
	if(nUserArgs >= 3)
		thickness = strtod(args.at(2).c_str(), NULL);
	size = G4ThreeVector(x, y, thickness);
	return true;
}

void greaseLayer::construct(nDetDetector *obj){
	obj->applyGreaseLayer(x, y, thickness);
}

std::string greaseLayer::syntaxStr() const {
	return std::string("addGreaseLayer <width> <height> [thickness]");
}

bool diffuserLayer::decodeString(){
	// Expects a space-delimited string of the form:
	//  "addDiffuserLayer width(mm) height(mm) thickness(mm) material"
	std::vector<std::string> args;
	nUserArgs = split_str(argStr, args);
	if(nUserArgs < nReqArgs)
		return false;
	x = strtod(args.at(0).c_str(), NULL);
	y = strtod(args.at(1).c_str(), NULL);
	thickness = strtod(args.at(2).c_str(), NULL);
	size = G4ThreeVector(x, y, thickness);
	return true;
}

void diffuserLayer::construct(nDetDetector *obj){
	obj->applyDiffuserLayer(x, y, thickness);
}

std::string diffuserLayer::syntaxStr() const {
	return std::string("addDiffuserLayer <width> <height> <thickness> [material=G4_SILICON_DIOXIDE]");
}

bool lightGuideLayer::decodeString(){
	// Expects a space-delimited string of the form:
	//  "addLightGuide width1(mm) width2(mm) height1(mm) height2(mm) thickness(mm) material"
	std::vector<std::string> args;
	nUserArgs = split_str(argStr, args);
	if(nUserArgs < nReqArgs)
		return false;
	x1 = strtod(args.at(0).c_str(), NULL);
	x2 = strtod(args.at(1).c_str(), NULL);
	y1 = strtod(args.at(2).c_str(), NULL);
	y2 = strtod(args.at(3).c_str(), NULL);
	thickness = strtod(args.at(4).c_str(), NULL);
	size = G4ThreeVector(std::max(x1, x2), std::max(y1, y2), thickness);
	return true;
}

void lightGuideLayer::construct(nDetDetector *obj){
	obj->applyLightGuide(x1, x2, y1, y2, thickness);
}

std::string lightGuideLayer::syntaxStr() const {
	return std::string("addLightGuide <width1> <width2> <height1> <height2> <thickness> [material=G4_SILICON_DIOXIDE]");
}

bool gdmlLayer::decodeString(){
	// Expects a space-delimited string of the form:
	//  "filename posX(cm) posY(cm) posZ(cm) rotX(deg) rotY(deg) rotZ(deg) material"
	std::vector<std::string> args;
	nUserArgs = split_str(argStr, args);
	if(nUserArgs < nReqArgs)
		return false;
		
	filename = args.at(0);
	position = G4ThreeVector(strtod(args.at(1).c_str(), NULL)*cm, strtod(args.at(2).c_str(), NULL)*cm, strtod(args.at(3).c_str(), NULL)*cm);
	rotation = G4ThreeVector(strtod(args.at(4).c_str(), NULL)*deg, strtod(args.at(5).c_str(), NULL)*deg, strtod(args.at(6).c_str(), NULL)*deg);
	material = args.at(7);

	// Load the model
	solid.read(filename, material, false);
	solid.setRotation(rotation);
	solid.setPosition(position);

	// Get the size of the model
	size = G4ThreeVector(solid.getWidth(), solid.getThickness(), solid.getLength());
	
	return true;
}

void gdmlLayer::construct(nDetDetector *obj){
	obj->loadGDML(&solid);
}

std::string gdmlLayer::syntaxStr() const {
	return std::string("loadGDML <filename> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString>");
}

bool gdmlLightGuideLayer::decodeString(){
	// Expects a space-delimited string of the form:
	//  "filename rotX(deg) rotY(deg) rotZ(deg) material"
	std::vector<std::string> args;
	nUserArgs = split_str(argStr, args);
	if(nUserArgs < nReqArgs)
		return false;

	filename = args.at(0);
	rotation = G4ThreeVector(strtod(args.at(1).c_str(), NULL)*deg, strtod(args.at(2).c_str(), NULL)*deg, strtod(args.at(3).c_str(), NULL)*deg);
	material = args.at(4);

	// Load the model
	solid.read(filename, material, false);
	solid.setRotation(rotation);
	
	// Get the size of the model
	size = G4ThreeVector(solid.getWidth(), solid.getThickness(), solid.getLength());
	
	return true;
}

void gdmlLightGuideLayer::construct(nDetDetector *obj){
	obj->loadLightGuide(&solid, rotation);
}

std::string gdmlLightGuideLayer::syntaxStr() const {
	return std::string("loadLightGuide <filename> <rotX> <rotY> <rotZ> <matString>");
}
