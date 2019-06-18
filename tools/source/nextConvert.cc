#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "optionHandler.hh" // split_str
#include "nDetStructures.hpp" // Local library
#include "Structures.hpp" // From simpleScan

// Units (referenced to cm)
const double in = 2.54;
const double ft = 30.48;
const double m = 100;
const double dm = 10;
const double cm = 1;
const double mm = 0.1;
const double um = 1E4;
const double nm = 1E7;
const double deg = 3.14159265359/180;

const double cvac = 29.9792458; // cm/ns

double detectorPos[3] = {0, 0, 0};

///////////////////////////////////////////////////////////////////////////////
// class dataPack
///////////////////////////////////////////////////////////////////////////////

class dataPack{
  public:
	dataPack();

	bool isGoodEvent() const { return goodEvent; }

	bool setAddresses(TTree *tree);

	void setEnergyRange(const double &lo, const double &hi){ energyLimitLow = lo; energyLimitHigh = hi; }

	bool checkEnergy() const ;

	int getEntry(TTree *tree, long long entry);

	virtual void print(const size_t &) const { }

  protected:
	bool goodEvent;  
  
	double nInitEnergy;
	double energyLimitLow;
	double energyLimitHigh;
	
	nDetEventStructure *eventData;
	nDetDebugStructure *debugData;
	
	TBranch *branches[2];

	void fillVariables();

	virtual void fillChildVariables(){ }
};

dataPack::dataPack() : goodEvent(false), nInitEnergy(0), energyLimitLow(0), energyLimitHigh(-1), eventData(NULL), debugData(NULL) {
	branches[0] = NULL;
	branches[1] = NULL;
}

bool dataPack::setAddresses(TTree *tree){
	tree->SetBranchAddress("event", &eventData, &branches[0]);
	tree->SetBranchAddress("debug", &debugData, &branches[1]);
	return (branches[0] && branches[1]);
}

bool dataPack::checkEnergy() const {
	if(energyLimitHigh > energyLimitLow && (nInitEnergy < energyLimitLow || nInitEnergy > energyLimitHigh))
		return false;
	return true;
}

int dataPack::getEntry(TTree *tree, long long entry){
	int retval = tree->GetEntry(entry);
	fillVariables();
	return retval;
}

void dataPack::fillVariables(){
	goodEvent = eventData->goodEvent;
	nInitEnergy = eventData->nInitEnergy;
	fillChildVariables();
}

///////////////////////////////////////////////////////////////////////////////
// class dataPackPSPMT
///////////////////////////////////////////////////////////////////////////////

class dataPackPSPMT : public dataPack {
  public:
	dataPackPSPMT() : dataPack() { }
	
	~dataPackPSPMT(){ }
	
	void print(const size_t &index) const ;

	void decodeEntry(PSPmtStructure &pspmt, const double &t0=0);

  protected:
	virtual void fillChildVariables();

  private:
	unsigned int nPhotons[2];
	
	float pulsePhase[2];
	float pulseQDC[2];
	float anodeQDC[2][4];
};

void dataPackPSPMT::print(const size_t &index) const {
	if(index >= 2) return;
	std::cout << "nPhotons=" << nPhotons[index] << ", phase=" << pulsePhase[index] << ", tqdc=" << pulseQDC[index] << ", anodes=(" << anodeQDC[index][0] << ", " << anodeQDC[index][1] << ", " << anodeQDC[index][2] << ", " << anodeQDC[index][3] << "), good=" << goodEvent << std::endl;
}

void dataPackPSPMT::decodeEntry(PSPmtStructure &pspmt, const double &t0/*=0*/){
	const bool isBarDet = true;

	for(size_t i = 0; i < 2; i++){ // i=0 left, i=1 right
		// Channel ID encoder from PSPmtProcessor.
		unsigned short chanIdentifier = 0xFFFF;

		bool isRightEnd = (i == 1);
		unsigned short tqdcIndex = 0; // 0=Dynode, 1-4=Anode
		unsigned short chanNum = i;

		chanIdentifier &= ~(0x000F & (chanNum));         // Pixie module channel number
		chanIdentifier &= ~(0x0010 & (isBarDet << 4));   // Is this detector part of a double-ended bar?
		chanIdentifier &= ~(0x0020 & (isRightEnd << 5)); // Is this the right side of the bar detector?
		chanIdentifier &= ~(0x00C0 & (tqdcIndex << 6));  // Index of TQDC signal (if applicable).
		chanIdentifier = ~chanIdentifier;
	
		pspmt.Append(pulsePhase[i]+t0, pulseQDC[i], 0, nPhotons[i], chanIdentifier, chanNum);
	}
	
	// Handle the anodes.
	for(size_t i = 0; i < 2; i++){ // i=0 left, i=1 right
		for(size_t j = 0; j < 4; j++){ // Over 4 anodes.
			// Channel ID encoder from PSPmtProcessor.
			unsigned short chanIdentifier = 0xFFFF;
			
			bool isRightEnd = (i == 1);
			unsigned short tqdcIndex = j+1; // 0=Dynode, 1-4=Anode
			unsigned short chanNum = 4*i+j+2;
			
			chanIdentifier &= ~(0x000F & (chanNum));         // Pixie module channel number
			chanIdentifier &= ~(0x0010 & (isBarDet << 4));   // Is this detector part of a double-ended bar?
			chanIdentifier &= ~(0x0020 & (isRightEnd << 5)); // Is this the right side of the bar detector?
			chanIdentifier &= ~(0x00C0 & (tqdcIndex << 6));  // Index of TQDC signal (if applicable).
			chanIdentifier = ~chanIdentifier;			
		
			if(!isRightEnd)
				pspmt.Append(0, anodeQDC[i][j], 0, anodeQDC[i][j], chanIdentifier, chanNum);
			else
				pspmt.Append(0, anodeQDC[i][3-j], 0, anodeQDC[i][3-j], chanIdentifier, chanNum);
		}
	}
}

void dataPackPSPMT::fillChildVariables(){
	for(size_t i = 0; i < 2; i++){
		nPhotons[i] = debugData->nPhotons[i];
		pulsePhase[i] = debugData->pulsePhase[i];
		pulseQDC[i] = debugData->pulseQDC[i];
		for(size_t j = 0; j < 4; j++){
			anodeQDC[i][j] = debugData->anodeQDC[i][j];
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
// class dataPackGenericBar
///////////////////////////////////////////////////////////////////////////////

class dataPackGenericBar : public dataPack {
  public:
	dataPackGenericBar() : dataPack() { }
	
	~dataPackGenericBar(){ }
	
	void print(const size_t &index) const ;

	void decodeEntry(GenericBarStructure &bar, const double &t0=0);

  protected:
	virtual void fillChildVariables();

  private:
	float pulsePhase[2];
	float pulseQDC[2];
};

void dataPackGenericBar::print(const size_t &index) const {
	if(index >= 2) return;
	std::cout << "phase=" << pulsePhase[index] << ", tqdc=" << pulseQDC[index] << ", good=" << goodEvent << std::endl;
}

void dataPackGenericBar::decodeEntry(GenericBarStructure &bar, const double &t0/*=0*/){
	bar.Append(pulsePhase[0]+t0, pulsePhase[1]+t0, pulseQDC[0], pulseQDC[1], 0);
}

void dataPackGenericBar::fillChildVariables(){
	for(size_t i = 0; i < 2; i++){
		pulsePhase[i] = debugData->pulsePhase[i];
		pulseQDC[i] = debugData->pulseQDC[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
// processPSPMT
///////////////////////////////////////////////////////////////////////////////

bool processPSPMT(TTree *data, const std::string &fname, const double &t0=0, const double &Elo=0, const double &Ehi=0){
	PSPmtStructure pspmt;
	dataPackPSPMT pack;
	
	if(!pack.setAddresses(data))
		return false;

	if(Ehi > Elo)
		pack.setEnergyRange(Elo, Ehi);

	TFile *ofile = new TFile(fname.c_str(), "RECREATE");
	TTree *otree = new TTree("data", "simpleScan style PSPMT data generated from nextSim output");

	otree->Branch("pspmt", &pspmt);

	std::cout << " Converting " << data->GetEntries() << " PSPMT events...\n";

	for(unsigned int i = 0; i < data->GetEntries(); i++){
		pack.getEntry(data, i);
		
		if(!pack.isGoodEvent() || !pack.checkEnergy()) continue;
		
		pack.decodeEntry(pspmt, t0);

		otree->Fill();

		pspmt.Zero();
	}
	
	std::cout << "  Done! wrote " << otree->GetEntries() << " to output root file.\n";
	
	ofile->cd();
	otree->Write();
	ofile->Close();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// processGenericBar
///////////////////////////////////////////////////////////////////////////////

bool processGenericBar(TTree *data, const std::string &fname, const double &t0=0, const double &Elo=0, const double &Ehi=0){
	GenericBarStructure bar;
	dataPackGenericBar pack;

	if(!pack.setAddresses(data))
		return false;

	if(Ehi > Elo)
		pack.setEnergyRange(Elo, Ehi);

	TFile *ofile = new TFile(fname.c_str(), "RECREATE");
	TTree *otree = new TTree("data", "simpleScan style GenericBar data generated from nextSim output");

	otree->Branch("genericbar", &bar);

	std::cout << " Converting " << data->GetEntries() << " GenericBar events...\n";

	for(unsigned int i = 0; i < data->GetEntries(); i++){
		pack.getEntry(data, i);
		
		if(!pack.isGoodEvent() || !pack.checkEnergy()) continue;
		
		pack.decodeEntry(bar, t0);

		otree->Fill();

		bar.Zero();
	}
	
	std::cout << "  Done! wrote " << otree->GetEntries() << " to output root file.\n";
	
	ofile->cd();
	otree->Write();
	ofile->Close();
	
	return true;
}

void help(char * prog_name_){
	std::cout << "  SYNTAX: " << prog_name_ << " <filename> [options] [output]\n";
	std::cout << "   Available options:\n";
	std::cout << "    --help (-h)             | Display this dialogue.\n";
	std::cout << "    --energy <Elow> <Ehigh> | Specify an incident neutron energy range.\n";
	std::cout << "    --generic               | Process non-segmented bar detector.\n";
}

int main(int argc, char *argv[]){
	if(argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)){
		help(argv[0]);
		return 1;
	}
	else if(argc < 2){
		std::cout << " Error: Invalid number of arguments to " << argv[0] << ". Expected 1, received " << argc-1 << ".\n";
		help(argv[0]);
		return 1;
	}

	int index = 1;
	bool genericBarMode = false;
	std::string inputFilename = "";
	std::string outputFilename = "converted.root";
	double energyLimitLow = 0;
	double energyLimitHigh = 0;
	while(index < argc){
		if(strcmp(argv[index], "--generic") == 0){
			std::cout << " Note: Outputing GenericBar type data.\n";
			genericBarMode = true;
		}
		else if(strcmp(argv[index], "--energy") == 0){
			if(argc <= index+2){
				std::cout << " Error: Invalid number of arguments to --energy.\n";
				help(argv[0]);
				return 1;
			}
			energyLimitLow = strtod(argv[++index], NULL);
			energyLimitHigh = strtod(argv[++index], NULL);
			std::cout << " Note: Only considering events with initial energy in range [" << energyLimitLow << " MeV, " << energyLimitHigh << " MeV].\n";
		}
		else if(inputFilename.empty()){
			inputFilename = std::string(argv[index]);
		}
		else{
			outputFilename = std::string(argv[index]);
		}
		index++;
	}

	TFile *f = new TFile(inputFilename.c_str(), "READ");
	if(!f->IsOpen()) return 1;
	
	// Get the run title.
	std::cout << "Processing input file \"" << inputFilename << "\"\n";
	std::cout << " Title: \"" << f->GetTitle() << "\"\n"; 
	
	// Get the position of the detector (assume the user has not moved the source).
	int positionType;
	TNamed *named = NULL;
	if((named = (TNamed*)f->Get("setup/setPosition"))) // x y z unit
		positionType = 0;
	else if((named = (TNamed*)f->Get("setup/setCylindrical"))) // r theta y (cm, deg, cm)
		positionType = 1;
	else if((named = (TNamed*)f->Get("setup/setSpherical"))) // r theta phi (cm, deg, deg)
		positionType = 2;
	else{
		f->Close();
		return 1;
	}
	std::string detectorPosStr(named->GetTitle());

	std::vector<std::string> args;
	unsigned int nArgs = split_str(detectorPosStr, args);
	if((positionType == 0 && nArgs < 5) || ((positionType == 1 || positionType == 2) && nArgs < 4)){
		std::cout << " Warning! Invalid source position specification (" << detectorPosStr << ")!\n";
	}
	else if(positionType == 0){ // Cartesian coordinates.
		detectorPos[0] = strtod(args[1].c_str(), NULL);
		detectorPos[1] = strtod(args[2].c_str(), NULL);
		detectorPos[2] = strtod(args[3].c_str(), NULL);

		std::string unitStr = args[4];
		
		double validUnits[8] = {in, ft, m, dm, cm, mm, um, nm};
		std::string unitNames[8] = {"in", "ft", "m", "dm", "cm", "mm", "um", "nm"};

		bool foundValidUnit = false;
		for(size_t i = 0; i < 8; i++){
			if(unitStr == unitNames[i]){
				std::cout << " Note: Input source position specified in units of \"" << unitNames[i] << "\".\n";
				if(unitNames[i] != "cm"){
					double multiplier = validUnits[i]/cm;
					detectorPos[0] *= multiplier;
					detectorPos[1] *= multiplier;
					detectorPos[2] *= multiplier;
					std::cout << " Note: Used unit conversion of " << multiplier << " cm/" << unitNames[i] << ".\n";
				}
				foundValidUnit = true;
				break;
			}
		}

		if(!foundValidUnit){
			std::cout << " Error: Unit other than cm specified (" << unitStr << ")!\n";
			return 1;
		}
	}
	else if(positionType == 1){ // Cylindrical coordinates.
		double r0 = strtod(args[1].c_str(), NULL)*cm;
		double theta = strtod(args[2].c_str(), NULL)*deg;
		detectorPos[0] = r0*std::cos(theta);
		detectorPos[1] = strtod(args[3].c_str(), NULL)*cm;
		detectorPos[2] = r0*std::sin(theta);
	}
	else if(positionType == 2){
		double r0 = strtod(args[1].c_str(), NULL)*cm;
		double theta = strtod(args[2].c_str(), NULL)*deg;
		double phi = strtod(args[3].c_str(), NULL)*deg;
		detectorPos[0] = r0*std::sin(theta)*std::cos(phi);
		detectorPos[1] = r0*std::sin(theta)*std::sin(phi);
		detectorPos[2] = r0*std::cos(theta);
	}

	// Compute the gamma flash offset. This will counter-act barifier and pspmt analyzer gamma shifts.
	const double t0 = -std::sqrt(detectorPos[0]*detectorPos[0]+detectorPos[1]*detectorPos[1]+detectorPos[2]*detectorPos[2])/cvac; // ns
	
	std::cout << " Using detector position of (x=" << detectorPos[0] << " cm, y=" << detectorPos[1] << " cm, z=" << detectorPos[2] << " cm)\n";
	std::cout << "  * Corresponding gamma-flash time offset is " << t0 << " ns\n";	
	
	TTree *t = (TTree*)f->Get("data");
	if(!t) return 1;

	bool retval;
	if(!genericBarMode)
		retval = processPSPMT(t, outputFilename, t0, energyLimitLow, energyLimitHigh);
	else
		retval = processGenericBar(t, outputFilename, t0, energyLimitLow, energyLimitHigh);

	if(!retval){
		std::cout << " Error: Failed to process input file.\n";
		return 1;
	}

	return 0;
}
