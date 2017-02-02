//
// Created by David Pérez Loureiro on 1/31/17.
//

#include "nDetConstructionMessenger.hh"

#include "nDetConstruction.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

nDetConstructionMessenger::nDetConstructionMessenger(nDetConstruction* detector)
        :fDetector(detector){

    fDetectorDir=new G4UIdirectory("/nDet/detector");
    fDetectorDir->SetGuidance("Detector geometry control");

    fGeometryCmd=new G4UIcmdWithAString("/nDet/detector/setGeometry",this);
    fGeometryCmd->SetGuidance("Defines the Geometry of the detector");
    fGeometryCmd->SetGuidance("Default is the disk");
    fGeometryCmd->SetCandidates("disk hexagon ellipse rectangle");
    fGeometryCmd->SetParameterName("geometry",true);
    fGeometryCmd->SetDefaultValue("disk");
    fGeometryCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    fSiliconDimensionsCmd=new G4UIcmdWith3VectorAndUnit("/nDet/detector/setSiPMdimensions",this);
    fSiliconDimensionsCmd->SetGuidance("Defines the size of the SiPMs");

    //std::cout<<"nDetConstructionMessenger::nDetConstructionMessenger()->"<<fGeometryCmd<<std::endl;

}

nDetConstructionMessenger::~nDetConstructionMessenger(){

    //G4cout<<"nDetConstructionMessenger::~nDetConstructionMessenger()->"<<fGeometryCmd<<G4endl;

    delete fDetectorDir;

    delete fSiliconDimensionsCmd;

    delete fGeometryCmd;

    //G4cout<<"nDetConstructionMessenger::~nDetConstructionMessenger()->"<<fGeometryCmd<<G4endl;
}

void nDetConstructionMessenger::SetNewValue(G4UIcommand* command,G4String newValue){

    if(command == fGeometryCmd){
        ;
    }

    if(command == fSiliconDimensionsCmd) {
;
    }
}
