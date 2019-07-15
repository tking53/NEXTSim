//
// Created by David Pérez Loureiro on 1/31/17.
//

#include "nDetConstructionMessenger.hh"

#include "nDetConstruction.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4SystemOfUnits.hh"

void nDetConstructionMessenger::addAllCommands(){
///////////////////////////////////////////////////////////////////////////////
// Detector commands
///////////////////////////////////////////////////////////////////////////////

	addDirectory("/nDet/detector/", "Detector geometry control");

	addCommand(new G4UIcmdWithAString("/nDet/detector/addGeometry", this));
	addGuidance("Defines the Geometry of the detector");
	addCandidates("next module ellipse rectangle cylinder test");

	addCommand(new G4UIcmdWithAString("/nDet/detector/setPmtDimensions", this));
	addGuidance("Defines the size of the SiPMs in mm. SYNTAX: setPmtDimensions <sizeX> [sizeY]");

	addCommand(new G4UIcmdWithADouble("/nDet/detector/setDetectorLength", this));
	addGuidance("Defines the size of the plastic in cm");

	addCommand(new G4UIcmdWithADouble("/nDet/detector/setDetectorWidth", this));
	addGuidance("Defines the width of the plastic in cm");

	addCommand(new G4UIcmdWithADouble("/nDet/detector/setDetectorThickness", this));
	addGuidance("Defines the thickness of the plastic in mm");

	addCommand(new G4UIcmdWithADouble("/nDet/detector/setMylarThickness", this));
	addGuidance("Defines the thickness of the plastic the mylar in mm (0 for no mylar)");

	addCommand(new G4UIcmdWithADouble("/nDet/detector/setGreaseThickness", this));
	addGuidance("Defines the thickness of the optical grease layer in mm (0 for no grease)");

	addCommand(new G4UIcmdWithADouble("/nDet/detector/setTrapezoidLength", this));
	addGuidance("Defines the length of the trapezoidal part of ellipse in cm");

	addCommand(new G4UIcmdWithAnInteger("/nDet/detector/setNumColumns", this));
	addGuidance("Set the number of columns in a segmented scintillator.");
	
	addCommand(new G4UIcmdWithAnInteger("/nDet/detector/setNumRows", this));
	addGuidance("Set the number of rows in a segmented scintillator.");
	
	addCommand(new G4UIcmdWithAnInteger("/nDet/detector/setPmtColumns", this));
	addGuidance("Set the number of anode columns in a segmented PSPMT.");
	
	addCommand(new G4UIcmdWithAnInteger("/nDet/detector/setPmtRows", this));
	addGuidance("Set the number of anode rows in a segmented PSPMT.");
	
	addCommand(new G4UIcommand("/nDet/detector/update", this));
	addGuidance("Updates the detector Geometry");

	addCommand(new G4UIcmdWithAString("/nDet/detector/setSpectralResponse", this));
	addGuidance("Load PMT spectral response from a root file");
	addGuidance("Input file MUST contain a TGraph named \"spec\"");

	addCommand(new G4UIcmdWithAString("/nDet/detector/setGainMatrix", this));
	addGuidance("Load segmented PMT anode gain matrix file");
	
	addCommand(new G4UIcmdWithAString("/nDet/detector/loadGDML", this));
	addGuidance("Load a GDML geometry file for testing. SYNTAX: loadGDML <filename> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString>");
	
	addCommand(new G4UIcmdWithADouble("/nDet/detector/setDiffuserLength", this));
	addGuidance("Defines the length of the straight light diffuser in cm");
	
	addCommand(new G4UIcmdWithAString("/nDet/detector/setWrapping", this));
	addGuidance("Set the material to use for reflective wrapping");	
	addCandidates("teflon silicon mylar esr perfect");

	addCommand(new G4UIcmdWith3VectorAndUnit("/nDet/detector/setShadowBarSize", this));
	addGuidance("Set the size of a shadow-bar.");

	addCommand(new G4UIcmdWith3VectorAndUnit("/nDet/detector/setShadowBarPos", this));
	addGuidance("Set the position of the shadow-bar.");
	
	addCommand(new G4UIcmdWithAString("/nDet/detector/addShadowBar", this));
	addGuidance("Set the shadow-bar material to use.");	
	
	addCommand(new G4UIcmdWith3VectorAndUnit("/nDet/detector/setPosition", this)); // position of source in cartesian coordinates (x, y, z).
	addGuidance("Set the position of the detector in cartesian coordinates (x, y, z)");

	addCommand(new G4UIcmdWith3Vector("/nDet/detector/setCylindrical", this)); // position of source in cylindrical coordinates (r, theta, y).
	addGuidance("Set the position of the detector in cylindrical coordinates (r, theta, y) where r and y are in cm and theta is in degrees");

	addCommand(new G4UIcmdWith3Vector("/nDet/detector/setSpherical", this)); // position of source in spherical coordinates (r, theta, phi).
	addGuidance("Set the position of the detector in spherical coordinates (r, theta, phi) where r is in cm and theta and phi are in degrees");
	
	addCommand(new G4UIcmdWith3Vector("/nDet/detector/setRotation", this)); // rotation of detector.
	addGuidance("Set the rotation of the detector by specifying angles about the x, y, and z axes (in deg)");

	addCommand(new G4UIcmdWithAString("/nDet/detector/addGreaseLayer", this));
	addGuidance("Add a layer of optical grease (all units in mm). SYNTAX: addGreaseLayer <width> <height> [thickness]");	

	addCommand(new G4UIcmdWithAString("/nDet/detector/addDiffuserLayer", this));
	addGuidance("Add a straight diffuser to the assembly (all units in mm). SYNTAX: addDiffuserLayer <width> <height> <thickness> [material=G4_SILICON_DIOXIDE]");	

	addCommand(new G4UIcmdWithAString("/nDet/detector/addLightGuide", this));
	addGuidance("Add a trapezoidal light-guide to the assembly (all units in mm). SYNTAX: addLightGuide <width1> <width2> <height1> <height2> <thickness> [material=G4_SILICON_DIOXIDE]");	

	addCommand(new G4UIcmdWithAString("/nDet/detector/loadLightGuide", this));
	addGuidance("Load a light-guide from a GDML geometry file. SYNTAX: loadLightGuide <filename> <rotX> <rotY> <rotZ> <matString>");

	addCommand(new G4UIcmdWithAString("/nDet/detector/setMaterial", this));
	addGuidance("Set the material to use for detector construction.");
	addCandidates("ej200 ej276");
	
	addCommand(new G4UIcmdWithADouble("/nDet/detector/setWindowThickness", this));
	addGuidance("Defines the thickness of the pspmt quartz window in mm (0 for no window)");

	addCommand(new G4UIcmdWithAString("/nDet/detector/setPolished", this));
	addGuidance("Enable or disable polished optical grease faces (disabled by default)");
	addCandidates("true false");

	addCommand(new G4UIcmdWithoutParameter("/nDet/detector/clear", this));
	addGuidance("Clear all detector Geometry");
	
	addCommand(new G4UIcmdWithAString("/nDet/detector/addArray", this));
	addGuidance("Add an array of multiple detectors. SYNTAX: addArray <geom> <r0> <startTheta> <stopTheta> <Ndet>");

	addCommand(new G4UIcmdWithoutParameter("/nDet/detector/setStart", this));
	addGuidance("Mark the current detector as a start detector");

	addCommand(new G4UIcmdWithAString("/nDet/detector/setSquarePmt", this));
	addGuidance("Enable or disable square PMTs (enabled by default). If disabled, circular PMTs will be used");
	addCandidates("true false");

///////////////////////////////////////////////////////////////////////////////
// PMT & digitizer commands
///////////////////////////////////////////////////////////////////////////////

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

	addCommand(new G4UIcmdWithoutParameter("/nDet/output/trace/params", this));
	addGuidance("Print pulse and digitizer settings");
}

void nDetConstructionMessenger::SetNewChildValue(G4UIcommand* command, G4String newValue){
	size_t index;
	if(!findCommand(command, newValue, index)) return;

	pmtResponse *prL = fDetector->GetPmtResponseL();
	pmtResponse *prR = fDetector->GetPmtResponseR();

	if(index == 0){
		fDetector->AddGeometry(newValue);
	}
	else if(index == 1) {
		fDetector->SetPmtDimension(newValue);
	}
	else if(index == 2) {
		G4double length = command->ConvertToDouble(newValue);
		fDetector->SetDetectorLength(length*cm);
	}
	else if(index == 3) {
		G4double length = command->ConvertToDouble(newValue);
		fDetector->SetDetectorWidth(length*cm);
	}
	else if(index == 4) {
		G4double thickness = command->ConvertToDouble(newValue);
		fDetector->SetDetectorThickness(thickness*mm);
	}
	else if(index == 5) {
		G4double val = command->ConvertToDouble(newValue);
		fDetector->SetMylarThickness(val*mm);
	}	
	else if(index == 6) {
		G4double val = command->ConvertToDouble(newValue);
		fDetector->SetGreaseThickness(val*mm);
	}
	else if(index == 7) {
		G4double val = command->ConvertToDouble(newValue);
		fDetector->SetTrapezoidLength(val*cm);
	}
	else if(index == 8){
		G4int val = command->ConvertToInt(newValue);
		fDetector->SetNumColumns(val);
	}
	else if(index == 9){
		G4int val = command->ConvertToInt(newValue);
		fDetector->SetNumRows(val);
	}
	else if(index == 10){
		G4int val = command->ConvertToInt(newValue);
		fDetector->SetNumPmtColumns(val);
	}
	else if(index == 11){
		G4int val = command->ConvertToInt(newValue);
		fDetector->SetNumPmtRows(val);
	}
	else if(index == 12){
		fDetector->UpdateGeometry();
	}	
	else if(index == 13){
		fDetector->setPmtSpectralResponse(newValue.c_str());
	}
	else if(index == 14){
		fDetector->setPmtGainMatrix(newValue.c_str());
	}
	else if(index == 15){
		fDetector->LoadGDML(newValue);
	}
	else if(index == 16){
		G4double val = command->ConvertToDouble(newValue);
		fDetector->SetDiffuserLength(val*cm);
	}
	else if(index == 17){
		fDetector->SetWrappingMaterial(newValue);
	}
	else if(index == 18){
		G4ThreeVector val = command->ConvertToDimensioned3Vector(newValue);
		fDetector->SetShadowBarSize(val);
	}
	else if(index == 19){
		G4ThreeVector val = command->ConvertToDimensioned3Vector(newValue);
		fDetector->SetShadowBarPosition(val);
	}
	else if(index == 20){
		fDetector->SetShadowBarMaterial(newValue);
	}
	else if(index == 21){
		G4ThreeVector val = command->ConvertToDimensioned3Vector(newValue);
		fDetector->SetPosition(val);
	}
	else if(index == 22){
		G4ThreeVector val = command->ConvertTo3Vector(newValue);
		fDetector->SetPositionCylindrical(val);
	}
	else if(index == 23){
		G4ThreeVector val = command->ConvertTo3Vector(newValue);
		fDetector->SetPositionSpherical(val);
	}
	else if(index == 24){
		G4ThreeVector val = command->ConvertTo3Vector(newValue);
		fDetector->SetRotation(val);
	}
	else if(index == 25){
		fDetector->AddGrease(newValue);
	}
	else if(index == 26){
		fDetector->AddDiffuser(newValue);
	}
	else if(index == 27){
		fDetector->AddLightGuide(newValue);
	}
	else if(index == 28){
		fDetector->AddLightGuideGDML(newValue);
	}
	else if(index == 29){
		fDetector->SetDetectorMaterial(newValue);
	}
	else if(index == 30) {
		G4double val = command->ConvertToDouble(newValue);
		fDetector->SetWindowThickness(val*mm);
	}
	else if(index == 31){
		fDetector->SetPolishedInterface((newValue == "true") ? true : false);
	}
	else if(index == 32){
		fDetector->ClearGeometry();
	}
	else if(index == 33){
		fDetector->AddDetectorArray(newValue);
	}
	else if(index == 34){
		fDetector->getCurrentDetector()->setAsStart(true);
	}
	else if(index == 35){
		fDetector->SetSquarePMTs((newValue == "true") ? true : false);
	}
	else if(index == 36){
		G4double val = command->ConvertToDouble(newValue);
		prL->setRisetime(val);
		prR->setRisetime(val);
	}
	else if(index == 37){
		G4double val = command->ConvertToDouble(newValue);
		prL->setFalltime(val);
		prR->setFalltime(val);
	}
	else if(index == 38){
		G4double val = command->ConvertToDouble(newValue);
		prL->setGain(val);
		prR->setGain(val);
	}
	else if(index == 39){
		G4double val = command->ConvertToDouble(newValue);
		prL->setBaselinePercentage(val);
		prR->setBaselinePercentage(val);
	}
	else if(index == 40){
		G4double val = command->ConvertToDouble(newValue);
		prL->setBaselineJitterPercentage(val);
		prR->setBaselineJitterPercentage(val);
	}
	else if(index == 41){
		G4double val = command->ConvertToDouble(newValue);
		prL->setPolyCfdFraction(val);
		prR->setPolyCfdFraction(val);
	}
	else if(index == 42){
		G4double val = command->ConvertToDouble(newValue);
		prL->setTraceDelay(val);
		prR->setTraceDelay(val);
	}
	else if(index == 43){
		G4double val = command->ConvertToDouble(newValue);
		prL->setPulseLengthInNanoSeconds(val);
		prR->setPulseLengthInNanoSeconds(val);
	}
	else if(index == 44){
		G4double val = command->ConvertToDouble(newValue);
		prL->setTransitTimeSpread(val);
		prR->setTransitTimeSpread(val);
	}
	else if(index == 45){
		G4int val = command->ConvertToInt(newValue);
		prL->setPulseIntegralLow(val);
	}
	else if(index == 46){
		G4int val = command->ConvertToInt(newValue);
		prL->setPulseIntegralHigh(val);
		prR->setPulseIntegralHigh(val);
	}
	else if(index == 47){
		G4int val = command->ConvertToInt(newValue);
		prL->setBitRange(val);
		prR->setBitRange(val);
	}
	else if(index == 48){
		G4int val = command->ConvertToInt(newValue);
		prL->setFunctionType(val);
		prR->setFunctionType(val);
	}
	else if(index == 49){
		prL->setPrintTrace((newValue == "true") ? true : false);
		prR->setPrintTrace((newValue == "true") ? true : false);
	}
	else if(index == 50){
		prL->print(); // Only show the left side, because they're both the same
	}
}
