//
// Created by Cory Thornsberry on 11/16/18.
//

#include "nDetRunActionMessenger.hh"

#include "nDetRunAction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

nDetRunActionMessenger::nDetRunActionMessenger(nDetRunAction *action) : fAction(action) {
	fOutputDir=new G4UIdirectory("/nDet/output/");
	fOutputDir->SetGuidance("Output Control");
	
	fOutputFileCmd[0] = new G4UIcmdWithAString("/nDet/output/filename", this);
	fOutputFileCmd[0]->SetGuidance("Sets the filename prefix of the output ROOT file.");
	
	fOutputFileCmd[1] = new G4UIcmdWithAString("/nDet/output/title", this);
	fOutputFileCmd[1]->SetGuidance("Sets the title of the output root file.");

	fOutputFileCmd[2] = new G4UIcmdWithAString("/nDet/output/overwrite", this);
	fOutputFileCmd[2]->SetGuidance("Set whether or not an existing output file will be overwritten");
	fOutputFileCmd[2]->SetCandidates("true false");

	fOutputFileCmd[3] = new G4UIcmdWithAString("/nDet/output/enabled", this);
	fOutputFileCmd[3]->SetGuidance("Enable or disable output file generation");
	fOutputFileCmd[3]->SetCandidates("true false");

	fOutputFileCmd[4] = new G4UIcmdWithAString("/nDet/output/persistent", this);
	fOutputFileCmd[4]->SetGuidance("Enable or disable persistent mode (i.e. to keep the output file open)");
	fOutputFileCmd[4]->SetCandidates("true false");
	
	fOutputFileIndex = new G4UIcmdWithAnInteger("/nDet/output/index", this);
	fOutputFileIndex->SetGuidance("Sets the run number suffix of the output root file.");
}

nDetRunActionMessenger::~nDetRunActionMessenger() {
	delete fOutputDir;
	for(int i = 0; i < 5; i++){
		delete fOutputFileCmd[i];
	}
	delete fOutputFileIndex;
}

void nDetRunActionMessenger::SetNewValue(G4UIcommand *command, G4String newValue){
	if(command == fOutputFileCmd[0]){
		fAction->setOutputFilename(newValue);
	}
	if(command == fOutputFileCmd[1]){
		fAction->setOutputFileTitle(newValue);
	}
	if(command == fOutputFileCmd[2]){
		fAction->setOverwriteOutputFile((newValue == "true") ? true : false);
	}
	if(command == fOutputFileCmd[3]){
		fAction->setOutputEnabled((newValue == "true") ? true : false);
	}
	if(command == fOutputFileCmd[4]){
		fAction->setPersistentMode((newValue == "true") ? true : false);
	}
	if(command == fOutputFileIndex){
		G4int val = fOutputFileIndex->ConvertToInt(newValue);
		fAction->setOutputFileIndex(val);
	}
}
