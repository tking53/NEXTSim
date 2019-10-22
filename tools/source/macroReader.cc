
#include <iostream>
#include <fstream>
#include <string>

#include "TFile.h"
#include "TObject.h"
#include "TCollection.h" // TIter class

#include "optionHandler.hh"
#include "termColors.hh"

int main(int argc, char** argv){
	optionHandler handler;
	handler.add(optionExt("input", required_argument, NULL, 'i', "<filename>", "Specify an input geant macro."));
	handler.add(optionExt("output", required_argument, NULL, 'o', "<filename>", "Specify the name of the output file."));
	handler.add(optionExt("verbose", no_argument, NULL, 'V', "", "Toggle verbose mode."));
	
	// Handle user input.
	if(!handler.setup(argc, argv))
		return 1;

	std::string inputFilename;
	if(handler.getOption(0)->active) // Set input filename
		inputFilename = handler.getOption(0)->argument;

	std::string outputFilename;
	if(handler.getOption(1)->active) // Set output filename
		outputFilename = handler.getOption(1)->argument;

	bool verboseMode = false;
	if(handler.getOption(2)->active) // Toggle verbose flag
		verboseMode = true;

	if(inputFilename.empty()){
		Display::ErrorPrint("Input root filename not specified!", "nextMacReader");
		return 1;
	}

	if(outputFilename.empty()){
		outputFilename = inputFilename;
		size_t index = inputFilename.find_last_of('.');
		if(index != std::string::npos){
			outputFilename = inputFilename.substr(0, index);
		}
		outputFilename += ".mac";
		std::cout << " outputFilename=" << outputFilename << std::endl;
	}

	TFile *finput = new TFile(inputFilename.c_str(), "READ");
	if(!finput->IsOpen()){
		Display::ErrorPrint("Failed to open input root file!", "nextMacReader");
		return 1;
	}

	if(!finput->cd("setup")){
		Display::ErrorPrint("Failed to find TDirectory named 'setup' in input file!", "nextMacReader");
		finput->Close();
		return 1;
	}

	std::ofstream foutput(outputFilename.c_str());
	if(!foutput.good()){
		Display::ErrorPrint("Failed to open output file!", "nextMacReader");
		finput->Close();
		return 1;
	}	

	// Load the currect directory.
	TDirectory *dir = finput->GetDirectory("setup");

	// Get list of map entries.
	TIter iter(dir->GetListOfKeys());

	while(true){
		TObject *obj = iter();
		if(!obj) break;

		if(verboseMode)
			std::cout << obj->GetTitle() << std::endl;

		// Write the map entry to the file.
		foutput << obj->GetTitle() << std::endl;
	}
	
	return 0;
}
