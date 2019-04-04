#include "ParticleSourceMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3Vector.hh"

ParticleSourceMessenger::ParticleSourceMessenger(ParticleSource* Gun) : fAction(Gun){
	addDirectory("/nDet/source/", "Particle Source Control");
	
	addCommand(new G4UIcmdWithoutParameter("/nDet/source/sample", this)); // test function
	addGuidance("Test distribution by outputing a random energy");
	
	addCommand(new G4UIcmdWith3VectorAndUnit("/nDet/source/position", this)); // position of source
	addGuidance("Set the position of the source in the lab frame");
	
	addCommand(new G4UIcmdWith3Vector("/nDet/source/direction", this)); // direction of source
	addGuidance("Set the direction of the source by specifying angles about the x, y, and z axes (in deg)");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/type", this)); // type of source (252Cf, 137Cs, etc)
	addGuidance("Set a pre-defined isotropic particle source");
	addCandidates("252Cf 137Cs 60Co 133Ba 241Am 90Sr");

	addCommand(new G4UIcmdWithAString("/nDet/source/beam", this)); // type of beam (neutron, gamma, electron)
	addGuidance("Set a pre-defined particle pencil beam");
	
	addCommand(new G4UIcmdWithADouble("/nDet/source/spot", this)); // beamspot radius (mm)
	addGuidance("Set the radius of the beam (in mm)");

	addCommand(new G4UIcmdWithAString("/nDet/source/shape", this)); // beamspot shape type
	addGuidance("Set the shape of the beamspot");
	addCandidates("circle square vertical horizontal");

	addCommand(new G4UIcmdWithAString("/nDet/source/iso", this));
	addGuidance("Set the source to psuedo-isotropic mode");
	addCandidates("true false");
	
	addCommand(new G4UIcmdWith3Vector("/nDet/source/range", this));
	addGuidance("Set the energy range of a continuous distribution source");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/reaction", this));
	addGuidance("Load a reaction file");
}

void ParticleSourceMessenger::SetNewValue(G4UIcommand* command, G4String newValue){ 
	size_t index;
	if(!findCommand(command, newValue, index)) return;
	if(index == 0)
		fAction->Sample(true);
	else if(index == 1)
		fAction->SetPosition(G4UIcommand::ConvertToDimensioned3Vector(newValue));
	else if(index == 2)
		fAction->SetDirection(G4UIcommand::ConvertTo3Vector(newValue));
	else if(index == 3)
		fAction->SetType(newValue);
	else if(index == 4)
		fAction->SetBeamType(newValue);
	else if(index == 5)
		fAction->SetBeamspotRadius(G4UIcommand::ConvertToDouble(newValue));
	else if(index == 6)
		fAction->SetBeamspotType(newValue);
	else if(index == 7)
		fAction->SetIsotropicMode((newValue == "true") ? true : false);
	else if(index == 8){
		G4ThreeVector vec = G4UIcommand::ConvertTo3Vector(newValue);
		fAction->SetEnergyLimits(vec.getX(), vec.getY());
	}
	else if(index == 9){
		fAction->LoadReactionFile(newValue);
	}
}
