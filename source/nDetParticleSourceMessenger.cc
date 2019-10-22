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
	addCandidates("point circle annulus ellipse square rectangle vertical horizontal gauss");

	addCommand(new G4UIcmdWithAnInteger("/nDet/source/iso", this));
	addGuidance("Set the source isotropic mode (0=off, 1=psuedo, 2=realistic)");
	
	addCommand(new G4UIcmdWith3Vector("/nDet/source/range", this));
	addGuidance("Set the energy range of a continuous distribution source");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/reaction", this));
	addGuidance("Load a reaction file. SYNTAX: reaction <filename>");

	addCommand(new G4UIcmdWithAString("/nDet/source/edist", this));
	addGuidance("Read the source energy distribution from an ascii file. SYNTAX: edist <filename>");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/addLevel", this));
	addGuidance("Add a discrete energy level to the current source. SYNTAX: addLevel <energy(keV)> [intensity] [particle]");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/test", this));
	addGuidance("Simulate a specified number of events and write particle energies to an output file. SYNTAX: test <filename> <Nevents>");

	addCommand(new G4UIcmdWithoutParameter("/nDet/source/reset", this));
	addGuidance("Reset the source and clear all defined energy levels");
	
	addCommand(new G4UIcmdWithAString("/nDet/source/inter", this));
	addGuidance("Set the default interpolation method for user defined energy distributions");
	addCandidates("Lin Log Exp Spline");
	
	addCommand(new G4UIcmdWithADouble("/nDet/source/setEnergy", this));
	addGuidance("Set the current energy level to mono-energetic and set the energy (in MeV)");

	addCommand(new G4UIcmdWithAString("/nDet/source/setGaussianEnergy", this));
	addGuidance("Set the current energy level to a gaussian distribution and set the energy and sigma. SYNTAX: setGaussianEnergy <E(MeV)> <dE(MeV)>");
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
		fAction->SetIsotropicMode(G4UIcommand::ConvertToInt(newValue));
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
		fAction->AddDiscreteEnergy(newValue);
	}
	else if(index == 11){
		fAction->Test(newValue);
	}
	else if(index == 12){
		fAction->Reset();
	}
	else if(index == 13){
		fAction->SetInterpolationMethod(newValue);
	}
	else if(index == 14)
		fAction->SetBeamEnergy(G4UIcommand::ConvertToDouble(newValue));
	else if(index == 15)
		fAction->SetBeamEnergySigma(newValue);
}
