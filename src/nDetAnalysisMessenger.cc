//
// Created by David Pérez Loureiro on 3/31/17.
//

#include "nDetAnalysisMessenger.hh"

#include "nDetAnalysisManager.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

nDetAnalysisMessenger::nDetAnalysisMessenger(nDetAnalysisManager *theManager) :fManager(theManager){

    fAnalysisDir=new G4UIdirectory("/nDet/analysis/");
    fAnalysisDir->SetGuidance("Analysis Control");
    fOutputFileCmd=new G4UIcmdWithAString("/nDet/analysis/outputFile",this);
    fOutputFileCmd->SetGuidance("Sets the name of the ROOT output file");
    fOutputFileCmd->SetDefaultValue("Out.root");
    fOutputFileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

nDetAnalysisMessenger::~nDetAnalysisMessenger() {

    delete fAnalysisDir;
    delete fOutputFileCmd;
}


void nDetAnalysisMessenger::SetNewValue(G4UIcommand *command, G4String newValue){

    if(command==fOutputFileCmd){

        fManager->SetOutputFileName(newValue);
    }

}