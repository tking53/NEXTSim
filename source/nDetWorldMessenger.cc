
#include "G4UIcmdWith3VectorAndUnit.hh"
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
}

void nDetWorldMessenger::SetNewChildValue(G4UIcommand* command, G4String newValue){
	size_t index;
	if(!findCommand(command, newValue, index)) return;

	else if(index == 0){
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
}
