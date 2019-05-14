#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "nDetMasterOutputFile.hh"
#include "nDetMasterOutputFileMessenger.hh"

nDetMasterOutputFileMessenger::nDetMasterOutputFileMessenger() : messengerHandler("nDetMasterOutputFileMessenger"), fOutputFile(NULL) { 
	addAllCommands(); 
}

nDetMasterOutputFileMessenger::nDetMasterOutputFileMessenger(nDetMasterOutputFile *action) : messengerHandler("nDetMasterOutputFileMessenger"), fOutputFile(action) { 
	addAllCommands(); 
}

void nDetMasterOutputFileMessenger::addAllCommands(){
	addDirectory("/nDet/output/", "Output file control");

	addCommand(new G4UIcmdWithAString("/nDet/output/filename", this));
	addGuidance("Sets the filename prefix of the output ROOT file.");

	addCommand(new G4UIcmdWithAString("/nDet/output/title", this));
	addGuidance("Sets the title of the output root file.");

	addCommand(new G4UIcmdWithAString("/nDet/output/overwrite", this));
	addGuidance("Set whether or not an existing output file will be overwritten");
	addCandidates("true false");

	addCommand(new G4UIcmdWithAString("/nDet/output/enabled", this));
	addGuidance("Enable or disable output file generation");
	addCandidates("true false");

	addCommand(new G4UIcmdWithAString("/nDet/output/persistent", this));
	addGuidance("Enable or disable persistent mode (i.e. to keep the output file open)");
	addCandidates("true false");

	addCommand(new G4UIcmdWithAString("/nDet/output/trace", this));
	addGuidance("Enable or disable writing of light pulse to root tree");
	addCandidates("true false");

	addCommand(new G4UIcmdWithAnInteger("/nDet/output/index", this));
	addGuidance("Sets the run number suffix of the output root file.");
	
	addCommand(new G4UIcmdWithAString("/nDet/output/debug", this));
	addGuidance("Enable or disable writing of detailed debug information to output file");
	addCandidates("true false");
	
	addCommand(new G4UIcmdWithAString("/nDet/output/badEvents", this));
	addGuidance("Enable or disable writing of non-detection events to the output file");
	addCandidates("true false");
}
	
void nDetMasterOutputFileMessenger::SetNewValue(G4UIcommand *command, G4String newValue){
	size_t index;
	if(!findCommand(command, newValue, index)) return;
		
	if(index == 0){
		fOutputFile->setOutputFilename(newValue);
	}
	else if(index == 1){
		fOutputFile->setOutputFileTitle(newValue);
	}
	else if(index == 2){
		fOutputFile->setOverwriteOutputFile((newValue == "true") ? true : false);
	}
	else if(index == 3){
		fOutputFile->setOutputEnabled((newValue == "true") ? true : false);
	}
	else if(index == 4){
		fOutputFile->setPersistentMode((newValue == "true") ? true : false);
	}
	else if(index == 5){
		fOutputFile->setOutputTraces((newValue == "true") ? true : false);
	}
	else if(index == 6){
		G4int val = command->ConvertToInt(newValue);
		fOutputFile->setOutputFileIndex(val);
	}
	else if(index == 7){
		fOutputFile->setOutputDebug((newValue == "true") ? true : false);
	}
	else if(index == 8){
		fOutputFile->setOutputBadEvents((newValue == "true") ? true : false);
	}
}
