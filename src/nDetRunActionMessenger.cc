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

nDetRunActionMessenger::nDetRunActionMessenger(nDetRunAction *action) : fAction(action) {
	fOutputDir[0]=new G4UIdirectory("/nDet/output/");
	fOutputDir[0]->SetGuidance("Output file control");

	fOutputDir[1]=new G4UIdirectory("/nDet/output/trace/");
	fOutputDir[1]->SetGuidance("Output light pulse parameters");
	
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

	fOutputFileCmd[5] = new G4UIcmdWithAString("/nDet/output/trace/enabled", this);
	fOutputFileCmd[5]->SetGuidance("Enable or disable writing of light pulse to root tree");
	fOutputFileCmd[5]->SetCandidates("true false");

	fOutputFileIndex = new G4UIcmdWithAnInteger("/nDet/output/index", this);
	fOutputFileIndex->SetGuidance("Sets the run number suffix of the output root file.");
	
	fOutputTraceParams[0] = new G4UIcmdWithADouble("/nDet/output/trace/setRisetime", this);
	fOutputTraceParams[0]->SetGuidance("Set the PMT light response risetime (ns)");
	
	fOutputTraceParams[1] = new G4UIcmdWithADouble("/nDet/output/trace/setFalltime", this);
	fOutputTraceParams[1]->SetGuidance("Set the PMT light response falltime (ns)");
	
	fOutputTraceParams[2] = new G4UIcmdWithADouble("/nDet/output/trace/setGain", this);
	fOutputTraceParams[2]->SetGuidance("Set the gain of the PMT light response pulse");

	fOutputTraceParams[3] = new G4UIcmdWithADouble("/nDet/output/trace/setBaseline", this);
	fOutputTraceParams[3]->SetGuidance("Set the baseline of the PMT light response pulse as a percentage of the full ADC range");
	
	fOutputTraceParams[4] = new G4UIcmdWithADouble("/nDet/output/trace/setJitter", this);
	fOutputTraceParams[4]->SetGuidance("Set the baseline jitter of the PMT light response pulse as a percentage of the full ADC range");
	
	fOutputTraceParams[5] = new G4UIcmdWithADouble("/nDet/output/trace/setCfdFraction", this);
	fOutputTraceParams[5]->SetGuidance("Set the Cfd F parameter as a fraction of the maximum pulse height");	

	fOutputTraceParams[6] = new G4UIcmdWithADouble("/nDet/output/trace/setTraceDelay", this);
	fOutputTraceParams[6]->SetGuidance("Set the delay of the PMT light response pulse (ns)");

	fOutputTraceParams[7] = new G4UIcmdWithADouble("/nDet/output/trace/setTraceLength", this);
	fOutputTraceParams[7]->SetGuidance("Set the length of the PMT light response pulse (ns)");	
	
	fOutputTraceAnalysis[0] = new G4UIcmdWithAnInteger("/nDet/output/trace/setIntegralLow", this);
	fOutputTraceAnalysis[0]->SetGuidance("Set the low pulse integration limit in ADC bins");
	
	fOutputTraceAnalysis[1] = new G4UIcmdWithAnInteger("/nDet/output/trace/setIntegralHigh", this);
	fOutputTraceAnalysis[1]->SetGuidance("Set the high pulse integration limit in ADC bins");
	
	fOutputTraceAnalysis[2] = new G4UIcmdWithAnInteger("/nDet/output/trace/setBitRange", this);
	fOutputTraceAnalysis[2]->SetGuidance("Set the ADC dynamic bit range");
}

nDetRunActionMessenger::~nDetRunActionMessenger() {
	delete fOutputDir[0];
	delete fOutputDir[1];
	for(int i = 0; i < 6; i++){
		delete fOutputFileCmd[i];
	}
	for(int i = 0; i < 8; i++){
		delete fOutputTraceParams[i];
	}
	for(int i = 0; i < 2; i++){
		delete fOutputTraceAnalysis[i];
	}
	delete fOutputFileIndex;
}

void nDetRunActionMessenger::SetNewValue(G4UIcommand *command, G4String newValue){
	pmtResponse *prL = fAction->getPmtResponseLeft();
	pmtResponse *prR = fAction->getPmtResponseRight();
	if(command == fOutputFileCmd[0]){
		fAction->setOutputFilename(newValue);
	}
	else if(command == fOutputFileCmd[1]){
		fAction->setOutputFileTitle(newValue);
	}
	else if(command == fOutputFileCmd[2]){
		fAction->setOverwriteOutputFile((newValue == "true") ? true : false);
	}
	else if(command == fOutputFileCmd[3]){
		fAction->setOutputEnabled((newValue == "true") ? true : false);
	}
	else if(command == fOutputFileCmd[4]){
		fAction->setPersistentMode((newValue == "true") ? true : false);
	}
	else if(command == fOutputFileCmd[5]){
		fAction->setOutputTraces((newValue == "true") ? true : false);
	}
	else if(command == fOutputTraceParams[0]){
		G4double val = fOutputTraceParams[0]->ConvertToDouble(newValue);
		prL->setRisetime(val);
		prR->setRisetime(val);
	}
	else if(command == fOutputTraceParams[1]){
		G4double val = fOutputTraceParams[1]->ConvertToDouble(newValue);
		prL->setFalltime(val);
		prR->setFalltime(val);
	}
	else if(command == fOutputTraceParams[2]){
		G4double val = fOutputTraceParams[2]->ConvertToDouble(newValue);
		prL->setGain(val);
		prR->setGain(val);
	}
	else if(command == fOutputTraceParams[3]){
		G4double val = fOutputTraceParams[3]->ConvertToDouble(newValue);
		fAction->setBaselinePercentage(val);
	}
	else if(command == fOutputTraceParams[4]){
		G4double val = fOutputTraceParams[4]->ConvertToDouble(newValue);
		fAction->setBaselineJitterPercentage(val);
	}
	else if(command == fOutputTraceParams[5]){
		G4double val = fOutputTraceParams[5]->ConvertToDouble(newValue);
		fAction->setPolyCfdFraction(val);
	}
	else if(command == fOutputTraceParams[6]){
		G4double val = fOutputTraceParams[6]->ConvertToDouble(newValue);
		prL->setTraceDelay(val);
		prR->setTraceDelay(val);
	}
	else if(command == fOutputTraceParams[7]){
		G4double val = fOutputTraceParams[7]->ConvertToDouble(newValue);
		prL->setPulseLengthInNanoSeconds(val);
		prR->setPulseLengthInNanoSeconds(val);
	}
	else if(command == fOutputTraceAnalysis[0]){
		G4int val = fOutputTraceAnalysis[0]->ConvertToInt(newValue);
		fAction->setPulseIntegralLow(val);
	}
	else if(command == fOutputTraceAnalysis[1]){
		G4int val = fOutputTraceAnalysis[1]->ConvertToInt(newValue);
		fAction->setPulseIntegralHigh(val);
	}
	else if(command == fOutputTraceAnalysis[2]){
		G4int val = fOutputTraceAnalysis[2]->ConvertToInt(newValue);
		prL->setBitRange(val);
		prR->setBitRange(val);
	}
	else if(command == fOutputFileIndex){
		G4int val = fOutputFileIndex->ConvertToInt(newValue);
		fAction->setOutputFileIndex(val);
	}
}
