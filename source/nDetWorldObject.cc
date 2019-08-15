
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Para.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Torus.hh"

#include "nDetWorldObject.hh"
#include "nDetMaterials.hh"
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
	shapeNameMap[""]       = DEFAULT;
	shapeNameMap["box"]    = BOX;
	shapeNameMap["cyl"]    = CYLINDER;
	shapeNameMap["cone"]   = CONE;
	shapeNameMap["para"]   = PARALLELEPIPED;
	shapeNameMap["trap"]   = TRAPEZOID;
	shapeNameMap["sphere"] = SPHERE;
	shapeNameMap["torus"]  = TORUS;
	
	// Select the default shape flag so we know that the user has not specified a shape
	shapeSelect = DEFAULT;
	
	// Default syntax string
	syntaxOutputString = "[[p1] [p2] ...]";
}

nDetWorldPrimitive::~nDetWorldPrimitive(){ 
}

bool nDetWorldPrimitive::decodeArgs(){ 
	// Expects a space-delimited string of the form:
	//  "addPrimitive shape posX(cm) posY(cm) posZ(cm) rotX(deg) rotY(deg) rotZ(deg) material [p1] [p2] ... [pn]"
	if(!setShape(args.at(0)))
		return false;
	position = G4ThreeVector(strtod(args.at(1).c_str(), NULL)*cm, strtod(args.at(2).c_str(), NULL)*cm, strtod(args.at(3).c_str(), NULL)*cm);
	setRotationMatrix(G4ThreeVector(strtod(args.at(4).c_str(), NULL), strtod(args.at(5).c_str(), NULL), strtod(args.at(6).c_str(), NULL)));
	material = args.at(7);
	for(std::vector<std::string>::iterator iter = args.begin()+8; iter != args.end(); iter++){
		size_t index = (*iter).find('=');
		if(index != std::string::npos){
			std::string pName = (*iter).substr(0, index);
			double pVal = strtod((*iter).substr(index+1).c_str(), NULL);
			sizeArgs[pName] = pVal;
		}
		else{ // Failed to find '='
			std::cout << " nDetWorldPrimitive: Error! Failed to find '=' in input argument (" << (*iter) << ")\n";
		}
	}
	return checkRequiredArgs();
}

std::string nDetWorldPrimitive::syntaxStr() const { 
	// This is a mess, but it does what I want it to do :) CRT
	std::stringstream retval;
	retval << "addPrimitive ";
	if(shapeSelect == DEFAULT)
		retval << "<shape>";
	else{
		for(auto iter : shapeNameMap){
			if(shapeSelect == iter.second){
				retval << "<\"" << iter.first << "\">" ;
				break;
			}
		}
	}
	retval << " <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <material> " << syntaxOutputString;
	return retval.str();
}

void nDetWorldPrimitive::placeObject(G4LogicalVolume *parent, nDetMaterials *materials){ 
	if(sizeArgs.empty()){
		Display::ErrorPrint("No shape has been selected by the user", "nDetWorldPrimitive");
		return;
	}
	if(buildGeometry()){
		G4Material *mat = materials->searchForMaterial(material);
		if(mat){
			geometry_logV = new G4LogicalVolume(geometry, mat, "");
			geometry_logV->SetVisAttributes(materials->visShadow);
			if(geometry)
				geometry_physV = new G4PVPlacement(&rotation, position, geometry_logV, "", parent, false, 0, true);
		}
		else{
			Display::ErrorPrint("Failed to find Geant material corresponding to user-specified material name", "nDetWorldPrimitive");
		}
	}
}

bool nDetWorldPrimitive::setShape(const G4String &shapeName){
	if(shapeName.empty())
		return false;
	for(auto iter : shapeNameMap){
		if(iter.first == shapeName){ // Found matching shape name
			// Set the selected shape
			shapeSelect = iter.second;

			// Set all initial parameters
			resetAllArgs(shapeSelect);

			// Reset the syntax string
			syntaxOutputString = getShapeArgString();

			return true;
		}
	}
	Display::ErrorPrint("Failed to find Geant primitive corresponding to user-specified shape name", "nDetWorldPrimitive");
	return false;
}

void nDetWorldPrimitive::listAllPrimitives(){
	std::cout << std::endl;
	std::cout << "nDetWorldPrimitive: The following shape types are defined in NEXTSim:\n";
	std::cout << "-------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Note: Parameter names are taken from the Geant4 application developer's handbook. See link below for detailed information\n";
	std::cout << "https://indico.cern.ch/event/679723/contributions/2792554/attachments/1559217/2453758/BookForApplicationDevelopers.pdf\n";
	std::cout << "-------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << " Name\tArguments\n";
	std::cout << " ----\t---------\n";
	for(auto iter : shapeNameMap){
		if(iter.second == DEFAULT)
			continue;
		resetAllArgs(iter.second);
		std::cout << " " << iter.first << "\t" << getShapeArgString() << std::endl;
	}
	std::cout << std::endl;
	// Reset the selected shape, just in case
	sizeArgs.clear();
	shapeSelect = DEFAULT;
}

std::string nDetWorldPrimitive::getShapeArgString(){
	// Get the extra required parameters
	std::stringstream syntaxStream;
	
	// Update the number of required arguments and add them to the syntax string
	nReqArgs = 8;
	for(auto arg : sizeArgs){
		if(arg.second < 0){ // Required argument
			syntaxStream << "<" << arg.first << "> ";
			nReqArgs++;
		}
	}
	
	// Add optional parameters to the syntax string
	for(auto arg : sizeArgs){
		if(arg.second >= 0) // Optional argument
			syntaxStream << "[" << arg.first << "=" << arg.second << "] ";
	}
	
	// Return the string
	return syntaxStream.str();	
}

void nDetWorldPrimitive::resetAllArgs(const G4SHAPETYPE &shape){
	// Clear current arguments (if there are any)
	sizeArgs.clear();

	switch(shape){
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
			sizeArgs["pDPhi"] = 360;
			break;
		case CONE: // G4Cons
			sizeArgs["pRmin1"] = 0;
			sizeArgs["pRmax1"] = -1;
			sizeArgs["pRmin2"] = 0;
			sizeArgs["pRmax2"] = -1;
			sizeArgs["pDz"]    = -1;
			sizeArgs["pSPhi"]  = 0;
			sizeArgs["pDPhi"]  = 360;
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
			sizeArgs["pDPhi"]   = 360;
			sizeArgs["pSTheta"] = 0;
			sizeArgs["pDTheta"] = 360;
			break;
		case TORUS: // G4Torus
			sizeArgs["pRmin"] = 0;
			sizeArgs["pRmax"] = -1;
			sizeArgs["pRtor"] = -1;
			sizeArgs["pSPhi"] = 0;
			sizeArgs["pDPhi"] = 360;
			break;
		default:
			break;
	}
}

bool nDetWorldPrimitive::checkRequiredArgs() const {
	bool retval = true;
	std::stringstream stream;
	stream << " nDetWorldPrimitive: Error! The following required parameters are not defined [ ";
	for(auto iter : sizeArgs){
		if(iter.second < 0){ // Missing required argument
			stream << iter.first << " ";
			retval = false;
		}
	}
	if(!retval){
		stream << "]";
		std::cout << stream.str() << std::endl;
	}
	return retval;
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
			newObj = new G4Tubs(extName, sizeArgs["pRMin"], sizeArgs["pRMax"], sizeArgs["pDz"]/2, sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg);
			break;
		case CONE:
			newObj = new G4Cons(extName, sizeArgs["pRMin1"], sizeArgs["pRMax1"], sizeArgs["pRMin2"], sizeArgs["pRMax2"], sizeArgs["pDz"]/2, sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg);
			break;
		case PARALLELEPIPED:
			newObj = new G4Para(extName, sizeArgs["dx"]/2, sizeArgs["dy"]/2, sizeArgs["dz"]/2, sizeArgs["alpha"]*deg, sizeArgs["theta"]*deg, sizeArgs["phi"]*deg);
			break;
		case TRAPEZOID:
			newObj = new G4Trd(extName, sizeArgs["dx1"]/2, sizeArgs["dx2"]/2, sizeArgs["dy1"]/2, sizeArgs["dy2"]/2, sizeArgs["dz"]/2);
			/*newObj = new G4Trap(extName, sizeArgs["pZ"], sizeArgs["pY"], sizeArgs["pX"], sizeArgs["pLTX"]);
			newObj = new G4Trap(extName, sizeArgs["pDz"]/2, sizeArgs["pTheta"]*deg, sizeArgs["pPhi"]*deg, 
			                             sizeArgs["pDy1"]/2, sizeArgs["pDx1"]/2, sizeArgs["pDx2"]/2, sizeArgs["pAlp1"]*deg, 
			                             sizeArgs["pDy2"]/2, sizeArgs["pDx3"]/2, sizeArgs["pDx4"]/2, sizeArgs["pAlp2"]*deg);*/
			break;
		case SPHERE:
			newObj = new G4Sphere(extName, sizeArgs["pRmin"], sizeArgs["pRmax"], sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg, sizeArgs["pSTheta"]*deg, sizeArgs["pDTheta"]*deg);
			break;
		case TORUS:
			newObj = new G4Torus(extName, sizeArgs["pRmin"], sizeArgs["pRmax"], sizeArgs["pRtor"], sizeArgs["pSPhi"]*deg, sizeArgs["pDPhi"]*deg);
			break;
		default:
			break;
	}
	
	if(newObj){
		if(geometry) delete geometry;
		geometry = newObj;
	}
	
	return newObj;
}
