
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
#include "G4Cons.hh"
#include "G4Sphere.hh"

#include "nDetDetector.hh"
#include "nDetDetectorLayer.hh"
#include "nDetConstruction.hh"
#include "nDetDetectorMessenger.hh"
#include "optionHandler.hh" // split_str

///////////////////////////////////////////////////////////////////////////////
// class nDetDetectorParams
///////////////////////////////////////////////////////////////////////////////

void nDetDetectorParams::InitializeMessenger(){
	if(fMessenger) 
		return;
	fMessenger = new nDetDetectorMessenger(this);
}

void nDetDetectorParams::SetDetectorWidth(const G4double &val){ 
	fDetectorWidth = val; 
	constantWidth = true;
}

void nDetDetectorParams::SetDetectorHeight(const G4double &val){ 
	fDetectorHeight = val; 
	constantHeight = true;
}

void nDetDetectorParams::SetSegmentWidth(const G4double &val){
	fSegmentWidth = val;
	constantWidth = false;
}

void nDetDetectorParams::SetSegmentHeight(const G4double &val){
	fSegmentHeight = val;
	constantHeight = false;
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
	detectorPosition = G4ThreeVector(x*cm, z*cm, 0);
	//detectorPosition = G4ThreeVector(x*cm, position.getZ()*cm, z*cm);

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

void nDetDetectorParams::SetDetectorMaterial(const G4String &material){ 
	detectorMaterialName = material;
}

void nDetDetectorParams::SetWrappingMaterial(const G4String &material){ 
	wrappingMaterialName = material; 
}

void nDetDetectorParams::SetUnsegmented(){
	fSegmentWidth = 0;
	fSegmentHeight = 0;
	fNumColumns = 1;
	fNumRows = 1;
	constantWidth = true;
	constantHeight = true;
}

void nDetDetectorParams::Print() const {
	G4ThreeVector rowX = detectorRotation.rowX();
	G4ThreeVector rowY = detectorRotation.rowY();
	G4ThreeVector rowZ = detectorRotation.rowZ();
	std::cout << " Address              = " << this << std::endl;
	std::cout << " Detector Length (Z)  = " << fDetectorLength << " mm\n";
	std::cout << " Detector Size (X, Y) = " << fDetectorWidth << " x " << fDetectorHeight << " mm^2\n";
	std::cout << " Pmt Size      (X, Y) = " << pmtWidth << " x " << pmtHeight << " mm^2\n";
	std::cout << " Grease Thickness     = " << fGreaseThickness << " mm\n";
	std::cout << " Window Thickness     = " << fWindowThickness << " mm\n";
	std::cout << " Sensitive Thickness  = " << fSensitiveThickness << " mm\n";
	std::cout << " Wrapping Thickness   = " << fWrappingThickness << " mm\n";
	std::cout << " Trapezoid Length     = " << fTrapezoidLength << " mm\n";
	std::cout << " Diffuser Length      = " << fDiffuserLength << " mm\n";
	std::cout << " Wrapping Material    = \"" << wrappingMaterialName << "\"\n";
	std::cout << " Detector Material    = \"" << detectorMaterialName << "\"\n";
	std::cout << " Geometry Type        = \"" << geomType << "\"\n";
	std::cout << " CopyNum  = " << scintCopyNum << std::endl;
	std::cout << " Position = (x=" << detectorPosition.getX() << ", y=" << detectorPosition.getY() << ", z=" << detectorPosition.getZ() << ")\n";
	std::cout << " Unit X   = (" << rowX.getX() << ", " << rowX.getY() << ", " << rowX.getZ() << ")\n";
	std::cout << " Unit Y   = (" << rowY.getX() << ", " << rowY.getY() << ", " << rowY.getZ() << ")\n";
	std::cout << " Unit Z   = (" << rowZ.getX() << ", " << rowZ.getY() << ", " << rowZ.getZ() << ")\n";
	std::cout << " Polished    : " << (fPolishedInterface ? "YES" : "NO") << std::endl;
	std::cout << " Square      : " << (fSquarePMTs ? "YES" : "NO") << std::endl;
	std::cout << " Start       : " << (isStart ? "YES" : "NO") << std::endl;
	if(IsSegmented() || PmtIsSegmented())
		std::cout << " Segmentation:\n";
	if(IsSegmented()){
		std::cout << "  Detector Segments (X, Y) = " << fNumColumns << " x " << fNumRows << std::endl;
		std::cout << "  Cell Size         (X, Y) = " << fSegmentWidth << " x " << fSegmentHeight << " mm^2\n";
	}
	if(PmtIsSegmented()){
		std::cout << "  Pmt Segments      (X, Y) = " << fNumColumnsPmt << " x " << fNumRowsPmt << "\n";
	}
}

void nDetDetectorParams::UpdateSize(){
	if(constantWidth)
		fSegmentWidth = (fDetectorWidth-(fNumColumns-1)*fWrappingThickness)/fNumColumns;
	else
		fDetectorWidth = fSegmentWidth*fNumColumns + (fNumColumns-1)*fWrappingThickness;

	if(constantHeight)
		fSegmentHeight = (fDetectorHeight-(fNumRows-1)*fWrappingThickness)/fNumRows;
	else
		fDetectorHeight = fSegmentHeight*fNumRows + (fNumRows-1)*fWrappingThickness;
}

///////////////////////////////////////////////////////////////////////////////
// class nDetDetector
///////////////////////////////////////////////////////////////////////////////

nDetDetector::nDetDetector(nDetConstruction *detector, nDetMaterials *matptr) : nDetDetectorParams(detector->GetDetectorParameters()),
                                                                                assembly_logV(NULL), assembly_physV(NULL),
                                                                                layerSizeX(0), layerSizeY(0), offsetZ(0),
	                                                                            parentCopyNum(0), firstSegmentCopyNum(0), lastSegmentCopyNum(0), 
	                                                                            checkOverlaps(false)
{
	copyCenterOfMass(*detector->GetCenterOfMassL(), *detector->GetCenterOfMassR());
	materials = matptr;
}

nDetDetector::~nDetDetector(){
	/*for(std::vector<nDetWorldObject*>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		delete (*iter); // This causes a seg-fault
	}
	userLayers.clear();*/
}

nDetDetector nDetDetector::clone() const {
	// Clone this detector's parameters
	nDetDetector retval(*this);
	
	// Copy the center of mass calculators explicitly
	retval.cmL = cmL.clone();
	retval.cmR = cmR.clone();

	return retval;
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
	for(std::vector<nDetWorldObject*>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		(*iter)->construct(this);
	}	
}

void nDetDetector::placeDetector(G4LogicalVolume *parent){
	assembly_physV = new G4PVPlacement(&detectorRotation, detectorPosition, assembly_logV, "Assembly", parent, 0, 0, false);
	assembly_physV->SetCopyNo(parentCopyNum);
	
	// Do some post-placement processing
	afterPlacement();
}

void nDetDetector::clear(){
	cmL.clear();
	cmR.clear();
}

void nDetDetector::copyCenterOfMass(const centerOfMass &left, const centerOfMass &right){
	cmL = left.clone();
	cmR = right.clone();
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

void nDetDetector::construct(){
	// Update the size of the assembly in the event it has changed
	UpdateSize(); 

	// Prepare to build the detector and compute the maximum size of the detector volume
	prepareToBuild();

	// Build the assembly volume
	constructAssembly();

	// Get materials and visual attributes
	scintMaterial = materials->getUserDetectorMaterial(detectorMaterialName);
	scintVisAtt = materials->getUserVisAttributes(detectorMaterialName);
	wrappingMaterial = materials->getUserSurfaceMaterial(wrappingMaterialName);
	wrappingVisAtt = materials->getUserVisAttributes(wrappingMaterialName);
	wrappingOpSurf = materials->getUserOpticalSurface(wrappingMaterialName);

	// Build the geometry
	buildDetector();

	// Generate all user-defined layers.
	buildAllLayers();

	// Attach PMTs.
	constructPSPmts();

	
   // HaGRID_Construction()
   
	auto n_daughter = assembly_logV->GetNoDaughters();
	std::vector<G4ThreeVector> hagrid_transformation = {G4ThreeVector(0,0,10*cm), G4ThreeVector(0,0,2*cm), G4ThreeVector(0,0,3*cm), G4ThreeVector(0,0,4*cm) };
	for (int i = 0; i < n_daughter ; i++) {
		auto logV = assembly_logV->GetDaughter(i);
		if(logV->GetName()=="3in_hag_3in_hag_pmt_p"){
			for(auto & vec : hagrid_transformation){
				std::string name = "hagrid" + std::to_string(i);
				new G4PVPlacement(NULL, vec, logV->GetLogicalVolume(), name, assembly_logV, false, i);
			}
		}
		std::cout << logV->GetName() << std::endl;
	/*	if(logV->GetName("3in_hag_3in_hag_crystal_p")){
			new G4PVPlacement();
		}
		if(logV->GetName("3in_hag_3in_hag_shell_p")){
			new G4PVPlacement();
		}*/
	
	}




}

G4LogicalVolume *nDetDetector::constructAssembly(){
	// Calculate the dimensions of the detector
	G4double assemblyWidth = maxBodySize.getX() + 2*fWrappingThickness;
	G4double assemblyHeight = maxBodySize.getY() + 2*fWrappingThickness;
	G4double assemblyLength = maxBodySize.getZ();

	// Account for the size of the PSPMT
	assemblyWidth = std::max(assemblyWidth, pmtWidth);
	assemblyHeight = std::max(assemblyHeight, pmtHeight);
	assemblyLength += 2*(fGreaseThickness+fWindowThickness+fSensitiveThickness);

	// Account for the additional component layers
	for(std::vector<nDetWorldObject*>::iterator iter = userLayers.begin(); iter != userLayers.end(); iter++){
		if(!(*iter)->decodeString()){
			std::cout << " nDetDetector: Invalid number of arguments given to ::decodeString(). Expected " << (*iter)->getNumRequiredArgs() << " but received " << (*iter)->getNumSuppliedArgs() << ".\n";
			std::cout << " nDetDetector:  SYNTAX: " << (*iter)->syntaxStr() << std::endl;
			continue;
		}
		assemblyWidth = std::max(assemblyWidth, (*iter)->getSizeX());
		assemblyHeight = std::max(assemblyHeight, (*iter)->getSizeY());
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

void nDetDetector::prepareToBuild(){ 
	maxBodySize = G4ThreeVector(fDetectorWidth, fDetectorHeight, fDetectorLength); 
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
	
		G4CSGSolid *grease_solidV = getVolume("window_solidV", pmtWidth, pmtHeight, fGreaseThickness);
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
	
		G4CSGSolid *window_solidV = getVolume("window_solidV", pmtWidth, pmtHeight, fWindowThickness);
		G4LogicalVolume *window_logV = new G4LogicalVolume(window_solidV, materials->fSiO2, "window_logV");
		
		window_logV->SetVisAttributes(materials->visWindow);

		addMirroredComponents(window_physV[0], window_physV[1], window_logV, windowZ, "Quartz");
	}

	// Build the wrapping.
	if(WrappingEnabled() && wrappingThickness > 0){
		G4CSGSolid *boundingBox = getVolume("", pmtWidth, pmtHeight, wrappingThickness);
		G4CSGSolid *wrappingBox = getVolume("", pmtWidth + 2*fWrappingThickness, pmtHeight + 2*fWrappingThickness, wrappingThickness);
		
		G4SubtractionSolid *greaseWrapping = new G4SubtractionSolid("", wrappingBox, boundingBox);
		G4LogicalVolume *greaseWrapping_logV = new G4LogicalVolume(greaseWrapping, wrappingMaterial, "greaseWrapping_logV");
		greaseWrapping_logV->SetVisAttributes(wrappingVisAtt);
		
		G4double wrappingZ = offsetZ + fGreaseThickness/2 + fWindowThickness/2;

		// Place the wrapping around the scintillator.
		G4PVPlacement *greaseWrapping_physV[2];
		
		addMirroredComponents(greaseWrapping_physV[0], greaseWrapping_physV[1], greaseWrapping_logV, wrappingZ, "Wrapping");
		
		if(grease_physV[0] && grease_physV[1]){
			new G4LogicalBorderSurface("Wrapping", grease_physV[0], greaseWrapping_physV[0], wrappingOpSurf);
			new G4LogicalBorderSurface("Wrapping", grease_physV[1], greaseWrapping_physV[1], wrappingOpSurf);
		}
		if(window_physV[0] && window_physV[1]){
			new G4LogicalBorderSurface("Wrapping", window_physV[0], greaseWrapping_physV[0], wrappingOpSurf);
			new G4LogicalBorderSurface("Wrapping", window_physV[1], greaseWrapping_physV[1], wrappingOpSurf);
		}
	}
	
    // The photon sensitive surface
    G4CSGSolid *sensitive_solidV = getVolume(name+"_solidV", pmtWidth, pmtHeight, fSensitiveThickness);
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

G4CSGSolid *nDetDetector::getVolume(const G4String &name, const G4double &width, const G4double &height, const G4double &length){
	G4CSGSolid *retval;
	if(fSquarePMTs)
		retval = new G4Box(name, width/2, height/2, length/2);
	else
		retval = new G4Tubs(name, 0, width/2, length/2, 0, 2*CLHEP::pi);
	return retval;
}

G4CSGSolid *nDetDetector::getLightGuideVolume(const G4String &name, const G4double &w1, const double &w2, const double &h1, const double &h2, const G4double &length){
	G4CSGSolid *retval;
	if(fSquarePMTs)
		retval = new G4Trd(name, w1/2, w2/2, h1/2, h2/2, length/2);
	else
		retval = new G4Cons(name, 0, w1/2, 0, w2/2, length/2, 0, 2*CLHEP::pi);
	return retval;
}

void nDetDetector::applyGreaseLayer(){
	this->applyGreaseLayer(layerSizeX, layerSizeY);
}

void nDetDetector::applyGreaseLayer(const G4double &x, const G4double &y, double thickness/*=0*/){
	if(thickness <= 0)
		thickness = fGreaseThickness;
	if(thickness > 0){
		G4CSGSolid *grease_solidV = getVolume("grease", x, y, thickness);
		G4LogicalVolume *grease_logV = new G4LogicalVolume(grease_solidV, materials->fGrease, "grease_logV");
		grease_logV->SetVisAttributes(materials->visGrease);

		// Add the optical grease to the assembly
		addMirroredComponents(grease_logV, offsetZ+thickness/2, "Grease");

		// Offset the other layers to account for the layer of optical grease
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
		G4CSGSolid *lightDiffuser = getVolume("lightDiffuser", x, y, thickness);
		G4LogicalVolume *lightDiffuserLog = new G4LogicalVolume(lightDiffuser, materials->fSiO2, "lightDiffuser_logV");

		// Add the optical grease to the assembly
		addMirroredComponents(lightDiffuserLog, offsetZ+thickness/2, "Diffuser");

		// Offset the other layers to account for the light-diffuser
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
        G4CSGSolid *lightGuide = getLightGuideVolume("lightGuide", x1, x2, y1, y2, thickness);
	    G4LogicalVolume *lightGuideLog = new G4LogicalVolume(lightGuide, materials->fSiO2, "lightGuide_logV");	

		G4RotationMatrix *rightRotation = new G4RotationMatrix();
		rightRotation->rotateX(CLHEP::pi);

		// Place the light guides.
		G4PVPlacement *trapPhysicalL = addLeftComponent(lightGuideLog, trapezoidZ, trapName);
		G4PVPlacement *trapPhysicalR = addRightComponent(lightGuideLog, trapezoidZ, trapName, rightRotation);

		// Build the wrapping.
		if(WrappingEnabled()){
			G4CSGSolid *wrappingSolid = getLightGuideVolume("wrapping", x1+2*deltaX, x2+2*deltaY, y1+2*deltaX, y2+2*deltaY, thickness);
			G4SubtractionSolid *wrapping = new G4SubtractionSolid("wrapping", wrappingSolid, lightGuide);
			G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrapping, wrappingMaterial, "wrapping_logV");		
			wrapping_logV->SetVisAttributes(wrappingVisAtt);

			// Place the wrapping around the light guides.
			G4PVPlacement *trapWrappingL = addLeftComponent(wrapping_logV, trapezoidZ, trapName);
			G4PVPlacement *trapWrappingR = addRightComponent(wrapping_logV, trapezoidZ, trapName, rightRotation);
		
			// Reflective wrapping.
			new G4LogicalBorderSurface("Wrapping", trapPhysicalL, trapWrappingL, wrappingOpSurf);
			new G4LogicalBorderSurface("Wrapping", trapPhysicalR, trapWrappingR, wrappingOpSurf);
		}
		
		// Offset the other layers to account for the light-guide
		layerSizeX = x2;
		layerSizeY = y2;
    	offsetZ += thickness;
	}
}

void nDetDetector::loadGDML(gdmlSolid *solid){
	if(!solid || !solid->isLoaded()) 
		return;

	std::cout << " nDetDetector: Loaded GDML model (name=" << solid->getName() << ") with size x=" << solid->getWidth() << " mm, y=" << solid->getThickness() << " mm, z=" << solid->getLength() << " mm\n";

	std::cout << solid->getPhysicalVolume()->GetName() << std::endl;
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
