
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAString.hh"

#include "nDetWorld.hh"
#include "nDetWorldMessenger.hh"

void nDetWorldMessenger::addAllCommands(){
	addDirectory("/nDet/world/", "Simulation world control");

	addCommand(new G4UIcmdWith3VectorAndUnit("/nDet/world/setWorldSize", this));
	addGuidance("Set the bounding box size of the simulation world along the X, Y, and Z axes");

	addCommand(new G4UIcmdWithAString("/nDet/world/setWorldFill", this));
	addGuidance("Set the name of the material to use to fill the simulation world");
	
	addCommand(new G4UIcmdWithAString("/nDet/world/setWorldFloor", this));
	addGuidance("Setup a rectangular floor slab for the bottom of the simulation world. SYNTAX: setWorldFloor <surfaceY> <thickness> [material=G4_CONCRETE]");

	addCommand(new G4UIcmdWith3VectorAndUnit("/nDet/world/setWorldPitSize", this));
	addGuidance("Set the size of the floor pit along the X, Y, and Z axes");
	
	addCommand(new G4UIcmdWithAString("/nDet/world/addPrimitive", this));
	addGuidance("Add a Geant primitive geometry object to the setup area. SYNTAX: addPrimitive <shape> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString> [[p1] ... [pn]]");
	
	addCommand(new G4UIcmdWithoutParameter("/nDet/world/listPrimitives", this));
	addGuidance("List all Geant primitives which may be added to the experimental setup area");
}

void nDetWorldMessenger::SetNewChildValue(G4UIcommand* command, G4String newValue){
	size_t index;
	if(!findCommand(command, newValue, index)) return;

	if(index == 0){
		fWorld->setWorldSize(command->ConvertToDimensioned3Vector(newValue));
	}
	else if(index == 1){
		fWorld->setWorldMaterial(newValue);
	}
	else if(index == 2){
		fWorld->setWorldFloor(newValue);
	}
	else if(index == 3){
		fWorld->setWorldFloorPitSize(command->ConvertToDimensioned3Vector(newValue));
	}
	else if(index == 4){
		fWorld->addNewPrimitive(newValue);
	}
	else if(index == 5){
		fWorld->listAllPrimitives();
	}
}
