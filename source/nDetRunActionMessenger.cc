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

nDetRunActionMessenger::nDetRunActionMessenger() : messengerHandler("nDetRunActionMessenger"), fAction(NULL) { 
	addAllCommands(); 
}

nDetRunActionMessenger::nDetRunActionMessenger(nDetRunAction *action) : messengerHandler("nDetRunActionMessenger"), fAction(action) { 
	addAllCommands(); 
}

void nDetRunActionMessenger::addAllCommands(){
	addDirectory("/nDet/output/trace/", "Output light pulse parameters");

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

	addCommand(new G4UIcmdWithAnInteger("/nDet/output/trace/setFunction", this));
	addGuidance("Set the single photon response function (default=0)");

	addCommand(new G4UIcmdWithAString("/nDet/output/trace/print", this));
	addGuidance("Print the digitized light pulse");
	addCandidates("true false");
}

void nDetRunActionMessenger::SetNewValue(G4UIcommand *command, G4String newValue){
	size_t index;
	if(!findCommand(command, newValue, index)) return;

	pmtResponse *prL = fAction->getPmtResponseLeft();
	pmtResponse *prR = fAction->getPmtResponseRight();

	if(index == 0){
		G4double val = command->ConvertToDouble(newValue);
		prL->setRisetime(val);
		prR->setRisetime(val);
	}
	else if(index == 1){
		G4double val = command->ConvertToDouble(newValue);
		prL->setFalltime(val);
		prR->setFalltime(val);
	}
	else if(index == 2){
		G4double val = command->ConvertToDouble(newValue);
		prL->setGain(val);
		prR->setGain(val);
	}
	else if(index == 3){
		G4double val = command->ConvertToDouble(newValue);
		fAction->setBaselinePercentage(val);
	}
	else if(index == 4){
		G4double val = command->ConvertToDouble(newValue);
		fAction->setBaselineJitterPercentage(val);
	}
	else if(index == 5){
		G4double val = command->ConvertToDouble(newValue);
		fAction->setPolyCfdFraction(val);
	}
	else if(index == 6){
		G4double val = command->ConvertToDouble(newValue);
		prL->setTraceDelay(val);
		prR->setTraceDelay(val);
	}
	else if(index == 7){
		G4double val = command->ConvertToDouble(newValue);
		prL->setPulseLengthInNanoSeconds(val);
		prR->setPulseLengthInNanoSeconds(val);
	}
	else if(index == 8){
		G4double val = command->ConvertToDouble(newValue);
		prL->setTransitTimeSpread(val);
		prR->setTransitTimeSpread(val);
	}
	else if(index == 9){
		G4int val = command->ConvertToInt(newValue);
		fAction->setPulseIntegralLow(val);
	}
	else if(index == 10){
		G4int val = command->ConvertToInt(newValue);
		fAction->setPulseIntegralHigh(val);
	}
	else if(index == 11){
		G4int val = command->ConvertToInt(newValue);
		prL->setBitRange(val);
		prR->setBitRange(val);
	}
	else if(index == 12){
		G4int val = command->ConvertToInt(newValue);
		prL->setFunctionType(val);
		prR->setFunctionType(val);
	}
	else if(index == 13){
		fAction->setPrintTrace((newValue == "true") ? true : false);
	}
}
