
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Para.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Torus.hh"

#include "nDetWorldObject.hh"
#include "optionHandler.hh" // split_str
#include "termColors.hh"

///////////////////////////////////////////////////////////////////////////////
// class nDetWorldObject
///////////////////////////////////////////////////////////////////////////////

bool nDetWorldObject::decodeString(){
	args.clear();
	nUserArgs = split_str(argStr, args);
	if(nUserArgs < nReqArgs)
		return false;
	return this->decodeArgs();
}

bool nDetWorldObject::isNumerical(const std::string &str){
	for(size_t i = 0; i < str.length(); i++){
		if((str[i] < 0x31 || str[i] > 0x39) && str[i] != 0x2E)
			return false;
	}
	return true;
}

void nDetWorldObject::setRotationMatrix(const G4ThreeVector &vec){
	rotation = G4RotationMatrix();
	rotation.rotateX(vec.getX()*deg);
	rotation.rotateY(vec.getY()*deg);
	rotation.rotateZ(vec.getZ()*deg);
}

///////////////////////////////////////////////////////////////////////////////
// class nDetWorldPrimitive
///////////////////////////////////////////////////////////////////////////////

nDetWorldPrimitive::nDetWorldPrimitive(const G4String &arg_) : nDetWorldObject(arg_, 8) {
	shapeNameMap["box"]    = BOX;
	shapeNameMap["cyl"]    = CYLINDER;
	shapeNameMap["cone"]   = CONE;
	shapeNameMap["para"]   = PARALLELEPIPED;
	shapeNameMap["trap"]   = TRAPEZOID;
	shapeNameMap["sphere"] = SPHERE;
	shapeNameMap["torus"]  = TORUS;
}

nDetWorldPrimitive::~nDetWorldPrimitive(){ }

bool nDetWorldPrimitive::decodeArgs(){ 
	// Expects a space-delimited string of the form:
	//  "addPrimitive shape posX(cm) posY(cm) posZ(cm) rotX(deg) rotY(deg) rotZ(deg) material [p1] [p2] ... [pn]"
	if(!setShape(args.at(0)))
		return false;
	position = G4ThreeVector(strtod(args.at(1).c_str(), NULL)*cm, strtod(args.at(2).c_str(), NULL)*cm, strtod(args.at(3).c_str(), NULL)*cm);
	setRotationMatrix(G4ThreeVector(strtod(args.at(4).c_str(), NULL), strtod(args.at(5).c_str(), NULL), strtod(args.at(6).c_str(), NULL)));
	material = args.at(7);
	for(std::vector<std::string>::iterator iter = args.begin()+8; iter != args.end(); iter++){
		std::cout << " debug: iter=" << (*iter) << std::endl;
	}
	return true;
}

std::string nDetWorldPrimitive::syntaxStr() const { 
	return std::string("loadGDML <shape> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <material> [[p1] [p2] ...]");
}

void nDetWorldPrimitive::placeObject(G4LogicalVolume *parent){ 
	if(sizeArgs.empty()){
		Display::ErrorPrint("No shape has been selected by the user", "nDetWorldPrimitive");
		return;
	}
	buildGeometry();
	if(geometry)
		geometry_physV = new G4PVPlacement(&rotation, position, geometry_logV, "", parent, false, 0, true);
}

bool nDetWorldPrimitive::setShape(const G4String &shapeName){
	for(auto iter : shapeNameMap){
		if(iter.first == shapeName){ // Found matching shape name
			shapeSelect = iter.second;

			// Clear current arguments (if there are any)
			sizeArgs.clear();

			// Set all initial parameters
			resetAllArgs();

			return true;
		}
	}
	Display::ErrorPrint("Failed to find Geant primitive corresponding to user-specified shape name", "nDetWorldPrimitive");
	return false;
}

void nDetWorldPrimitive::resetAllArgs(){
	switch(shapeSelect){
		case BOX: // G4Box
			sizeArgs["pX"] = -1;
			sizeArgs["pY"] = -1;
			sizeArgs["pZ"] = -1;
			break;
		case CYLINDER: // G4Tubs
			sizeArgs["pRMin"] = 0;
			sizeArgs["pRMax"] = -1;
			sizeArgs["pDz"]   = -1;
			sizeArgs["pSPhi"] = 0;
			sizeArgs["pDPhi"] = 2*pi;
			break;
		case CONE: // G4Cons
			sizeArgs["pRmin1"] = 0;
			sizeArgs["pRmax1"] = -1;
			sizeArgs["pRmin2"] = 0;
			sizeArgs["pRmax2"] = -1;
			sizeArgs["pDz"]    = -1;
			sizeArgs["pSPhi"]  = 0;
			sizeArgs["pDPhi"]  = 2*pi;
			break;
		case PARALLELEPIPED: // G4Para
			sizeArgs["dx"]    = -1;
			sizeArgs["dy"]    = -1;
			sizeArgs["dz"]    = -1;
			sizeArgs["alpha"] = -1;
			sizeArgs["theta"] = -1;
			sizeArgs["phi"]   = -1;
			break;
		case TRAPEZOID: // G4Trd
			sizeArgs["dx1"] = -1;
			sizeArgs["dx2"] = -1;
			sizeArgs["dy1"] = -1;
			sizeArgs["dy2"] = -1;
			sizeArgs["dz"]  = -1;
			/*// G4Trap
			sizeArgs["pZ"]   = -1;
			sizeArgs["pY"]   = -1;
			sizeArgs["pX"]   = -1;
			sizeArgs["pLTX"] = -1;
			// G4Trap
			sizeArgs["pDz"]    = -1;
			sizeArgs["pTheta"] = -1;
			sizeArgs["pPhi"]   = -1;
			sizeArgs["pDy1"]   = -1;
			sizeArgs["pDx1"]   = -1;
			sizeArgs["pDx2"]   = -1;
			sizeArgs["pAlp1"]  = -1;
			sizeArgs["pDy2"]   = -1;
			sizeArgs["pDx3"]   = -1;
			sizeArgs["pDx4"]   = -1;
			sizeArgs["pAlp2"]  = -1;*/
			break;
		case SPHERE: // G4Sphere
			sizeArgs["pRmin"]   = 0;
			sizeArgs["pRmax"]   = -1;
			sizeArgs["pSPhi"]   = 0;
			sizeArgs["pDPhi"]   = 2*pi;
			sizeArgs["pSTheta"] = 0;
			sizeArgs["pDTheta"] = 2*pi;
			break;
		case TORUS: // G4Torus
			sizeArgs["pRmin"] = 0;
			sizeArgs["pRmax"] = -1;
			sizeArgs["pRtor"] = -1;
			sizeArgs["pSPhi"] = 0;
			sizeArgs["pDPhi"] = 2*pi;
			break;
		default:
			break;
	}
}

bool nDetWorldPrimitive::checkRequiredArgs() const {
	for(auto iter : sizeArgs){
		if(iter.second < 0){ // Missing required argument
			std::cout << " missing required argument (" << iter.first << ")\n";
			return false;
		}
	}
	return true;
}

G4CSGSolid *nDetWorldPrimitive::buildGeometry(const G4String &extName/*=""*/){
	if(!checkRequiredArgs()){ // Check for required arguments
		return NULL;
	}
	
	G4CSGSolid *newObj = NULL;
	switch(shapeSelect){
		case BOX:
			newObj = new G4Box(extName, sizeArgs["pX"]/2, sizeArgs["pY"]/2, sizeArgs["pZ"]/2);
			break;
		case CYLINDER:
			new G4Tubs(extName, sizeArgs["pRMin"], sizeArgs["pRMax"], sizeArgs["pDz"]/2, sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg);
			break;
		case CONE:
			new G4Cons(extName, sizeArgs["pRMin1"], sizeArgs["pRMax1"], sizeArgs["pRMin2"], sizeArgs["pRMax2"], sizeArgs["pDz"]/2, sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg);
			break;
		case PARALLELEPIPED:
			new G4Para(extName, sizeArgs["dx"]/2, sizeArgs["dy"]/2, sizeArgs["dz"]/2, sizeArgs["alpha"]*deg, sizeArgs["theta"]*deg, sizeArgs["phi"]*deg);
			break;
		case TRAPEZOID:
			new G4Trd(extName, sizeArgs["dx1"]/2, sizeArgs["dx2"]/2, sizeArgs["dy1"]/2, sizeArgs["dy2"]/2, sizeArgs["dz"]/2);
			/*new G4Trap(extName, sizeArgs["pZ"], sizeArgs["pY"], sizeArgs["pX"], sizeArgs["pLTX"]);
			new G4Trap(extName, sizeArgs["pDz"]/2, sizeArgs["pTheta"]*deg, sizeArgs["pPhi"]*deg, 
			                    sizeArgs["pDy1"]/2, sizeArgs["pDx1"]/2, sizeArgs["pDx2"]/2, sizeArgs["pAlp1"]*deg, 
			                    sizeArgs["pDy2"]/2, sizeArgs["pDx3"]/2, sizeArgs["pDx4"]/2, sizeArgs["pAlp2"]*deg);*/
			break;
		case SPHERE:
			new G4Sphere(extName, sizeArgs["pRmin"], sizeArgs["pRmax"], sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg, sizeArgs["pSTheta"]*deg, sizeArgs["pDTheta"]*deg);
			break;
		case TORUS:
			new G4Torus(extName, sizeArgs["pRmin"], sizeArgs["pRmax"], sizeArgs["pRtor"], sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg);
			break;
		default:
			break;
	}
	
	if(newObj){
		if(geometry) delete geometry;
		geometry = newObj;
		geometry_logV = new G4LogicalVolume(geometry, NULL, "");
	}
	
	return newObj;
}
