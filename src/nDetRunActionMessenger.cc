//
// Created by Cory Thornsberry on 11/16/18.
//

#include "nDetRunActionMessenger.hh"
#include "centerOfMass.hh"

#include "nDetRunAction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

nDetRunActionMessenger::nDetRunActionMessenger(nDetRunAction *action) : messengerHandler(), fAction(action) {
	addDirectory("/nDet/output/", "Output file control");
	addDirectory("/nDet/output/trace/", "Output light pulse parameters");

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

	addCommand(new G4UIcmdWithAString("/nDet/output/trace/enabled", this));
	addGuidance("Enable or disable writing of light pulse to root tree");
	addCandidates("true false");

	addCommand(new G4UIcmdWithAnInteger("/nDet/output/index", this));
	addGuidance("Sets the run number suffix of the output root file.");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setRisetime", this));
	addGuidance("Set the PMT light response risetime (ns)");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setFalltime", this));
	addGuidance("Set the PMT light response falltime (ns)");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setGain", this));
	addGuidance("Set the gain of the PMT light response pulse");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setBaseline", this));
	addGuidance("Set the baseline of the PMT light response pulse as a percentage of the full ADC range");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setJitter", this));
	addGuidance("Set the baseline jitter of the PMT light response pulse as a percentage of the full ADC range");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setCfdFraction", this));
	addGuidance("Set the Cfd F parameter as a fraction of the maximum pulse height");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setTraceDelay", this));
	addGuidance("Set the delay of the PMT light response pulse (ns)");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setTraceLength", this));
	addGuidance("Set the length of the PMT light response pulse (ns)");

	addCommand(new G4UIcmdWithADouble("/nDet/output/trace/setTimeSpread", this));
	addGuidance("Set the FWHM spread in the photo-electron transit time of the PMT (ns)");

	addCommand(new G4UIcmdWithAnInteger("/nDet/output/trace/setIntegralLow", this));
	addGuidance("Set the low pulse integration limit in ADC bins");

	addCommand(new G4UIcmdWithAnInteger("/nDet/output/trace/setIntegralHigh", this));
	addGuidance("Set the high pulse integration limit in ADC bins");

	addCommand(new G4UIcmdWithAnInteger("/nDet/output/trace/setBitRange", this));
	addGuidance("Set the ADC dynamic bit range");
}

void nDetRunActionMessenger::SetNewValue(G4UIcommand *command, G4String newValue){
	size_t index;
	if(!findCommand(command, newValue, index)) return;

	pmtResponse *prL = fAction->getPmtResponseLeft();
	pmtResponse *prR = fAction->getPmtResponseRight();

	if(index == 0){
		fAction->setOutputFilename(newValue);
	}
	else if(index == 1){
		fAction->setOutputFileTitle(newValue);
	}
	else if(index == 2){
		fAction->setOverwriteOutputFile((newValue == "true") ? true : false);
	}
	else if(index == 3){
		fAction->setOutputEnabled((newValue == "true") ? true : false);
	}
	else if(index == 4){
		fAction->setPersistentMode((newValue == "true") ? true : false);
	}
	else if(index == 5){
		fAction->setOutputTraces((newValue == "true") ? true : false);
	}
	else if(index == 6){
		G4int val = command->ConvertToInt(newValue);
		fAction->setOutputFileIndex(val);
	}
	else if(index == 7){
		G4double val = command->ConvertToDouble(newValue);
		prL->setRisetime(val);
		prR->setRisetime(val);
	}
	else if(index == 8){
		G4double val = command->ConvertToDouble(newValue);
		prL->setFalltime(val);
		prR->setFalltime(val);
	}
	else if(index == 9){
		G4double val = command->ConvertToDouble(newValue);
		prL->setGain(val);
		prR->setGain(val);
	}
	else if(index == 10){
		G4double val = command->ConvertToDouble(newValue);
		fAction->setBaselinePercentage(val);
	}
	else if(index == 11){
		G4double val = command->ConvertToDouble(newValue);
		fAction->setBaselineJitterPercentage(val);
	}
	else if(index == 12){
		G4double val = command->ConvertToDouble(newValue);
		fAction->setPolyCfdFraction(val);
	}
	else if(index == 13){
		G4double val = command->ConvertToDouble(newValue);
		prL->setTraceDelay(val);
		prR->setTraceDelay(val);
	}
	else if(index == 14){
		G4double val = command->ConvertToDouble(newValue);
		prL->setPulseLengthInNanoSeconds(val);
		prR->setPulseLengthInNanoSeconds(val);
	}
	else if(index == 15){
		G4double val = command->ConvertToDouble(newValue);
		prL->setTransitTimeSpread(val);
		prR->setTransitTimeSpread(val);
	}
	else if(index == 16){
		G4int val = command->ConvertToInt(newValue);
		fAction->setPulseIntegralLow(val);
	}
	else if(index == 17){
		G4int val = command->ConvertToInt(newValue);
		fAction->setPulseIntegralHigh(val);
	}
	else if(index == 18){
		G4int val = command->ConvertToInt(newValue);
		prL->setBitRange(val);
		prR->setBitRange(val);
	}
}
