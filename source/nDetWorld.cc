
#include "nDetWorld.hh"
#include "nDetWorldObject.hh"
#include "nDetWorldMessenger.hh"
#include "nDetMaterials.hh"

#include "gdmlSolid.hh"
#include "optionHandler.hh" // split_str
#include "termColors.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"

#define DEFAULT_FLOOR_MATERIAL "G4_CONCRETE"

nDetWorld::nDetWorld() : solidV(NULL), logV(NULL), physV(NULL), fillMaterial("air"), floorMaterial(), floorThickness(0), floorSurfaceY(0) {
	// Set the default size of the experimental hall
	hallSize = G4ThreeVector(10*m, 10*m, 10*m);
	
	messenger = new nDetWorldMessenger(this);
}
	
bool nDetWorld::setWorldFloor(const G4String &input){
	// Expects a space-delimited string of the form:
	//  "centerY(cm) thickness(cm) [material=G4_CONCRETE]"
	std::vector<std::string> args;
	unsigned int Nargs = split_str(input, args);
	if(Nargs < 2){
		std::cout << " nDetConstruction: Invalid number of arguments given to ::SetWorldFloor(). Expected 2, received " << Nargs << ".\n";
		std::cout << " nDetConstruction:  SYNTAX: <centerY> <thickness> [material=G4_CONCRETE]\n";
		return false;
	}
	floorSurfaceY = strtod(args.at(0).c_str(), NULL)*cm;
	floorThickness = strtod(args.at(1).c_str(), NULL)*cm;
	if(Nargs < 3) // Defaults to concrete
		floorMaterial = DEFAULT_FLOOR_MATERIAL;
	else
		floorMaterial = args.at(2);
	return true;
}

void nDetWorld::buildExpHall(nDetMaterials *materials){
	solidV = new G4Box("solidV", hallSize.getX()/2, hallSize.getY()/2, hallSize.getZ()/2);

	G4Material *expHallFill = materials->searchForMaterial(fillMaterial);
	if(!expHallFill){ // Use the default material, if
		Display::WarningPrint("Failed to find user-specified world material!", "nDetConstruction");
		Display::WarningPrint(" Defaulting to filling world volume with air", "nDetConstruction");
		expHallFill = materials->fAir;
	}

	logV = new G4LogicalVolume(solidV, expHallFill, "expHallLogV", 0, 0, 0);
	logV->SetVisAttributes(G4VisAttributes::Invisible);

	// Add a floor to the experimental hall (disabled by default)
	if(!floorMaterial.empty() && floorThickness > 0){
		G4Material *mat = materials->searchForMaterial(floorMaterial);
		if(mat){
			G4Box *floorBox = new G4Box("floor", hallSize.getX()/2, floorThickness/2, hallSize.getZ()/2);
			G4LogicalVolume *floor_logV;
			if(floorPitSize.getX() > 0 && floorPitSize.getX() > 0 && floorPitSize.getX() > 0){ // Dig a pit
				G4double pitCenterOffsetY = 0.5*(floorThickness - floorPitSize.getY());
				G4Box *pitBox = new G4Box("pitBox", floorPitSize.getX()/2, floorPitSize.getY()/2, floorPitSize.getZ()/2);
				G4SubtractionSolid *floorWithPit = new G4SubtractionSolid("floorWithPit", floorBox, pitBox, NULL, G4ThreeVector(0, pitCenterOffsetY, 0));
				floor_logV = new G4LogicalVolume(floorWithPit, mat, "floor_logV");
			}
			else{
				floor_logV = new G4LogicalVolume(floorBox, mat, "floor_logV");
			}
			floor_logV->SetVisAttributes(materials->visShadow);
			logV->SetVisAttributes(materials->visAssembly);
			new G4PVPlacement(NULL, G4ThreeVector(0, -(floorSurfaceY+floorThickness/2), 0), floor_logV, "floorBox_physV", logV, 0, 0, false);
		}
		else{
			Display::WarningPrint("Failed to find user-specified floor material!", "nDetConstruction");
			Display::WarningPrint(" Disabling the use of a floor", "nDetConstruction");
			floorMaterial = "";
			floorThickness = 0;
		}
	}

	// Add additional objects
	for(auto obj : objects){
		obj->placeObject(logV, materials);
	}

	// Place the experimental hall into the world
	physV = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logV, "expHallPhysV", 0, false, 0);

	return;
}

nDetWorldPrimitive *nDetWorld::addNewPrimitive(const G4String &str){
	nDetWorldPrimitive *obj = new nDetWorldPrimitive(str);
	obj->decodeString();
	if(!obj->decodeString()){
		std::cout << " nDetWorld: Invalid number of arguments given to ::addNewPrimitive(). Expected at least " << obj->getNumRequiredArgs() << " but received " << obj->getNumSuppliedArgs() << ".\n";
		std::cout << " nDetWorld:  SYNTAX: " << obj->syntaxStr() << std::endl;
		delete obj;
		return NULL;
	}
	objects.push_back(obj);
	return obj;
}

void nDetWorld::listAllPrimitives(){
	nDetWorldPrimitive dummy("");
	dummy.listAllPrimitives();
}

void nDetWorld::printDefinedObjects(){
	if(!objects.empty()){
		for(auto obj : objects){
			std::cout << "***********************************************************\n";
			obj->print();
		}
		std::cout << "***********************************************************\n";
	}
	else
		std::cout << " nDetWorldPrimitive: No 3d primitives are currently defined\n";
}

void nDetWorld::reset(){
	for(auto obj : objects)
		delete obj;
	objects.clear();
}

gdmlObject *nDetWorld::loadGDML(const G4String &input){
	gdmlObject *obj = new gdmlObject(input);
	obj->decodeString();
	if(!obj->decodeString()){
		std::cout << " nDetWorld: Invalid number of arguments given to ::loadGDML(). Expected " << obj->getNumRequiredArgs() << " but received " << obj->getNumSuppliedArgs() << ".\n";
		std::cout << " nDetWorld:  SYNTAX: " << obj->syntaxStr() << std::endl;
		delete obj;
		return NULL;
	}
	objects.push_back(obj);
	return obj;
}
