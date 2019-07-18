#include "G4SystemOfUnits.hh"

#include "nDetDetector.hh"
#include "nDetDetectorLayer.hh"
#include "optionHandler.hh" // split_str

///////////////////////////////////////////////////////////////////////////////
// class greaseLayer
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// class diffuserLayer
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// class lightGuideLayer
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// class gdmlLayer
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// class gdmlLightGuideLayer
///////////////////////////////////////////////////////////////////////////////

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
