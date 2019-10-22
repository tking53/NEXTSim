#include "G4SystemOfUnits.hh"

#include "nDetDetector.hh"
#include "nDetDetectorLayer.hh"
#include "optionHandler.hh" // split_str

///////////////////////////////////////////////////////////////////////////////
// class greaseLayer
///////////////////////////////////////////////////////////////////////////////

bool greaseLayer::decodeArgs(){
	// Expects a space-delimited string of the form:
	//  "addGreaseLayer width(mm) height(mm) thickness(mm)"
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

diffuserLayer::diffuserLayer(const G4String &arg_) : nDetWorldObject(arg_, 3), x(0), y(0), thickness(0) { 
	material = "G4_SILICON_DIOXIDE"; 
}

bool diffuserLayer::decodeArgs(){
	// Expects a space-delimited string of the form:
	//  "addDiffuserLayer width(mm) height(mm) thickness(mm) material"
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

lightGuideLayer::lightGuideLayer(const G4String &arg_) : nDetWorldObject(arg_, 5), x1(0), x2(0), y1(0), y2(0), thickness(0) { 
	material = "G4_SILICON_DIOXIDE"; 
}

bool lightGuideLayer::decodeArgs(){
	// Expects a space-delimited string of the form:
	//  "addLightGuide width1(mm) width2(mm) height1(mm) height2(mm) thickness(mm) material"
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
// class gdmlLightGuideLayer
///////////////////////////////////////////////////////////////////////////////

bool gdmlLightGuideLayer::decodeArgs(){
	// Expects a space-delimited string of the form:
	//  "filename rotX(deg) rotY(deg) rotZ(deg) material"
	filename = args.at(0);
	rotVector = G4ThreeVector(strtod(args.at(1).c_str(), NULL)*deg, strtod(args.at(2).c_str(), NULL)*deg, strtod(args.at(3).c_str(), NULL)*deg);
	material = args.at(4);

	// Load the model
	solid.read(filename, material, false);
	solid.setRotation(rotVector);
	
	// Get the size of the model
	size = G4ThreeVector(solid.getWidth(), solid.getThickness(), solid.getLength());
	
	return true;
}

void gdmlLightGuideLayer::construct(nDetDetector *obj){
	obj->loadLightGuide(&solid, rotVector);
}

std::string gdmlLightGuideLayer::syntaxStr() const {
	return std::string("loadLightGuide <filename> <rotX> <rotY> <rotZ> <matString>");
}
