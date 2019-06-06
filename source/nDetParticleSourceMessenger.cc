#include "nDetParticleSourceMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAnInteger.hh"

void nDetParticleSourceMessenger::addAllCommands(){
	addDirectory("/nDet/source/", "Particle Source Control");
	
	G4UIcmdWithAnInteger *cmd = new G4UIcmdWithAnInteger("/nDet/source/sample", this);
	cmd->SetParameterName("Nsamples", true);
	cmd->SetGuidance("Test distribution by outputting a random energy");
	cmd->SetDefaultValue(1);
	addCommand(cmd); // test function
	
	addCommand(new G4UIcmdWith3Vector("/nDet/source/direction", this)); // direction of source
	addGuidance("Set the direction of the source by specifying angles about the x, y, and z axes (in deg)");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/type", this)); // type of source (252Cf, 137Cs, etc)
	addGuidance("Set a pre-defined isotropic particle source");
	addGuidance("252Cf 137Cs 60Co 133Ba 241Am 90Sr neutron gamma electron laser");
	
	addCommand(new G4UIcmdWithADouble("/nDet/source/spot", this)); // beamspot radius (mm)
	addGuidance("Set the radius of the beam (in mm)");

	addCommand(new G4UIcmdWithADouble("/nDet/source/spot0", this)); // beamspot radius (mm)
	addGuidance("Set the secondary radius of the beam (in mm)");

	addCommand(new G4UIcmdWithAString("/nDet/source/shape", this)); // beamspot shape type
	addGuidance("Set the shape of the beamspot");
	addCandidates("circle annulus ellipse square rectangle vertical horizontal gauss");

	addCommand(new G4UIcmdWithAString("/nDet/source/iso", this));
	addGuidance("Set the source to psuedo-isotropic mode");
	addCandidates("true false");
	
	addCommand(new G4UIcmdWith3Vector("/nDet/source/range", this));
	addGuidance("Set the energy range of a continuous distribution source");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/reaction", this));
	addGuidance("Load a reaction file");

	addCommand(new G4UIcmdWithAString("/nDet/source/edist", this));
	addGuidance("Read the source energy distribution from an ascii file");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/test", this));
	addGuidance("Simulate a specified number of events and write particle energies to an output file");
}

void nDetParticleSourceMessenger::SetNewChildValue(G4UIcommand* command, G4String newValue){ 
	size_t index;
	if(!findCommand(command, newValue, index)) return;
	if(index == 0)
		fAction->Print(G4UIcommand::ConvertToInt(newValue));
	else if(index == 1)
		fAction->SetSourceDirection(G4UIcommand::ConvertTo3Vector(newValue));
	else if(index == 2)
		fAction->SetSourceType(newValue);
	else if(index == 3)
		fAction->SetBeamspotRadius(G4UIcommand::ConvertToDouble(newValue));
	else if(index == 4)
		fAction->SetBeamspotRadius0(G4UIcommand::ConvertToDouble(newValue));
	else if(index == 5)
		fAction->SetBeamspotType(newValue);
	else if(index == 6)
		fAction->SetIsotropicMode((newValue == "true") ? true : false);
	else if(index == 7){
		G4ThreeVector vec = G4UIcommand::ConvertTo3Vector(newValue);
		fAction->SetEnergyLimits(vec.getX(), vec.getY());
	}
	else if(index == 8){
		fAction->ReadReactionFile(newValue);
	}
	else if(index == 9){
		fAction->ReadEnergyFile(newValue);
	}
	else if(index == 10){
		fAction->Test(newValue);
	}
}
