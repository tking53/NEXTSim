#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Element.hh"

#include "termColors.hh"
#include "nDetMaterials.hh"
#include "nDetDynamicMaterial.hh"
#include "optionHandler.hh" // split_str

nDetDynamicMaterial::nDetDynamicMaterial() : prefix(), name(), 
                                             init(false), debug(false), needsConverted(false), needsScaled(false), newMaterial(false),
                                             density(0), convFactor(1), scalingFactor(1), nElements(0), valuesToRead(0), 
                                             visatt(NULL), material(NULL), mpt(NULL), currArray(NULL), matHandler(NULL) { 
	validCommands["setDebug"]         = 1;
	validCommands["setName"]          = 2;
	validCommands["setDensity"]       = 2;
	validCommands["setMaterial"]      = 2;
	validCommands["addElement"]       = 3;
	validCommands["addArray"]         = 3;
	validCommands["addScalableArray"] = 3;
	validCommands["addConstProperty"] = 3;
	validCommands["addProperty"]      = 4;
	validCommands["setVisColor"]      = 4;
	validCommands["setVisLineStyle"]  = 2;
	validCommands["setVisLineWidth"]  = 2;
	validCommands["setVisWireframe"]  = 1;
	validCommands["setVisSolid"]      = 1;
	visatt = new G4VisAttributes();
}

nDetDynamicMaterial::~nDetDynamicMaterial(){
	if(init){
		if(newMaterial)
			delete material;
		delete mpt;
	}
	delete visatt;
}

bool nDetDynamicMaterial::read(const std::string &fname, nDetMaterials* materials){
	if(init)
		return false;
	
	// Open the input file
	std::ifstream ifile(fname.c_str());
	if(!ifile.good()){
		std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Failed to read from input file \"" << fname << "\"!\n" << Display::ResetStr();
		return false;
	}
	
	// Get the filename prefix
	size_t index1 = (fname.find_last_of('/') != std::string::npos) ? fname.find_last_of('/')+1 : 0;
	size_t index2 = (fname.find_last_of('.') != std::string::npos) ? fname.find_last_of('.')-index1 : std::string::npos;
	prefix = fname.substr(index1, index2);
	
	init = true;
	mpt = new G4MaterialPropertiesTable();
	matHandler = materials;
	
	std::string line;
	while(true){
		std::getline(ifile, line);
		if(ifile.eof()) // Check for end-of-file
			break;
		if(line.empty() || line.at(0) == '#') // Check for comment lines
			continue;
			
		// Split the input line into space-delimited arguments
		std::vector<std::string> args;
		split_str(line, args);
		if(args.empty())
			continue;
		
		
		std::vector<std::string>::iterator iterArg = args.begin();
		for(; iterArg != args.end(); iterArg++){
			if(iterArg->empty()) // Empty argument (I don't think this is possible, but just in case)
				continue;
			if(iterArg->at(0) == '#') // The '#' marks a comment. Ignore the rest of the line
				break;
		}
		if(iterArg != args.end()) // We have parts of the comment to remove
			args.erase(iterArg, args.end());
			
		// Decode the arguments
		decode(args);
	}
	
	// Define the material
	if(!material){
		newMaterial = true;
		material = new G4Material(name, density, nElements);
		for(auto el : elements){ // Add elements to it
			G4Element *element = materials->getElement(el.first);
			if(element)
				material->AddElement(element, el.second);
		}
	}
	
	// Link the properties table to the material
	material->SetMaterialPropertiesTable(mpt);
	
	// Close the file
	ifile.close();
	
	return true;
}

double nDetDynamicMaterial::unitConversion(const double &value, const std::string &unit){
	return (value*G4UnitDefinition::GetValueOf(unit));
}

bool nDetDynamicMaterial::getBoolInput(const std::string &input, bool &flag){
	if(input == "true")
		flag = true;
	else if(input == "false")
		flag = false;
	else{
		std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Invalid argument \"" << input << "\"\n" << Display::ResetStr();
		return false;
	}
	return true;
}

bool nDetDynamicMaterial::decode(const std::vector<std::string> &args){
	size_t nArgs = args.size();
	std::string cmd = args.at(0);

	// Read an array value (and add it to the current array)
	if(valuesToRead > 0){ 
		if(!currArray) // This should never happen
			return false;
		double value = strtod(cmd.c_str(), NULL);
		if(needsConverted) // Use the user-specified unit conversion
			value *= convFactor;
		if(needsScaled) // Use the user-specified scaling factor (for scalable arrays only)
			value *= scalingFactor;
		currArray->push_back(value);
		valuesToRead--;
		return true;
	}

	if(validCommands.find(cmd) == validCommands.end()){ // Check for unknown commands
		std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Unrecognized command (" << args.at(0) << ").\n" << Display::ResetStr();
		return false;
	}

	size_t nArgsExpected = validCommands[cmd];
	if(nArgs < nArgsExpected){ // Check the number of arguments
		std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Invalid number of arguments given to " << cmd << "! Expected " << nArgsExpected-1 << " but received " << nArgs-1 << ".\n" << Display::ResetStr();
		return false;
	}

	if(cmd == "setDebug"){ // setDebug [true|false]
		if(nArgs < 2) // Toggle the state of the flag
			debug = !debug; 
		else if(!getBoolInput(args.at(1), debug)) // User specified the state of the flag
			return false;
	}
	else if(cmd == "setName"){ // setName <name>
		name = args.at(1);
		if(debug)
			std::cout << " debug: Set name to \"" << name << "\"\n";
	}
	else if(cmd == "setDensity"){ // setDensity <density>
		density = strtod(args.at(1).c_str(), NULL)*g/cm3;
		if(debug)
			std::cout << " debug: Set density to " << density/(g/cm3) << " g/cm^3\n";
	}
	else if(cmd == "setMaterial"){ // setMaterial <name>
		G4Material *nistMat = matHandler->getNistDatabase()->searchForMaterial(args.at(1));
		if(!nistMat)
			return false;
		material = nistMat;
		name = args.at(1);
	}
	else if(cmd == "addElement"){ // addElement <name> <weightFraction>
		if(isInElementMap(args.at(1))){
			std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Element \"" << args.at(1) << "\" is already added!\n" << Display::ResetStr();
			return false;
		}
		elements[args.at(1)] = strtod(args.at(2).c_str(), NULL);
		nElements++;
		if(debug)
			std::cout << " debug: Added element \"" << args.at(1) << "\" with fractional weight " << elements[args.at(1)] << std::endl;
	}
	else if(cmd == "addArray" || cmd == "addScalableArray"){ // addArray <name> <N> [units]
		if(isInVariableMap(args.at(1))){
			std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Variable named \"" << args.at(1) << "\" already exists!\n" << Display::ResetStr();
			return false;
		}
		valuesToRead = strtoul(args.at(2).c_str(), NULL, 10);
		variables[args.at(1)] = std::vector<double>();
		currArray = &variables[args.at(1)];
		currArray->reserve(valuesToRead);
		if(nArgs >= 4){ // User specified a unit, we need to do a unit conversion
			convFactor = G4UnitDefinition::GetValueOf(args.at(3));
			needsConverted = true;
			if(debug)
				std::cout << " debug: Added array \"" << args.at(1) << "\" with " << valuesToRead << " elements (units=" << args.at(3) << ", factor=" << convFactor << ")\n";
		}
		else{ // No unit specified, use the value as-is
			needsConverted = false;
			if(debug)
				std::cout << " debug: Added array \"" << args.at(1) << "\" with " << valuesToRead << " elements\n";
		}
		if(cmd == "addArray") // Normal array, do not scale
			needsScaled = false;
		else // Scalable array
			needsScaled = true;
	}
	else if(cmd == "addConstProperty"){ // addConstProperty <name> <value> [units]
		double value = strtod(args.at(2).c_str(), NULL);
		if(nArgs >= 4) // Do we need to do a unit conversion?
			value = unitConversion(value, args.at(3));
		// Add the property
		mpt->AddConstProperty(args.at(1).c_str(), value);
		if(debug)
			std::cout << " debug: Added const property \"" << args.at(1) << "\" to property table\n";
	}
	else if(cmd == "addProperty"){ // addProperty <name> <x> <y>
		std::map<std::string, std::vector<double> >::iterator iterX = variables.find(args.at(2));
		if(iterX == variables.end()){ // Ensure that the array has been defined
			std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Array named \"" << args.at(2) << "\" was not defined in the input file!\n" << Display::ResetStr();
			return false;
		}
		std::map<std::string, std::vector<double> >::iterator iterY = variables.find(args.at(3));
		if(iterY == variables.end()){ // Ensure that the array has been defined
			std::cout << Display::ErrorStr("nDetDynamicMaterial") << "Array named \"" << args.at(3) << "\" was not defined in the input file!\n" << Display::ResetStr();
			return false;
		}
		size_t sizeX = iterX->second.size();
		size_t sizeY = iterY->second.size();
		if(iterX->second.size() != iterY->second.size()){ // Check that the sizes of the two arrays are equal
			std::cout << Display::WarningStr("nDetDynamicMaterial") << "Size of \"" << iterX->first << "\" (" << sizeX << ") does not equal size of \"" << iterY->first << "\" (" << sizeY << ")!\n" << Display::ResetStr();
			sizeX = (sizeX < sizeY) ? sizeX : sizeY;
			std::cout << Display::WarningStr("nDetDynamicMaterial") << " Using lesser of the two sizes (" << sizeX << ")\n" << Display::ResetStr();
		}
		// Add the property
		mpt->AddProperty(args.at(1).c_str(), iterX->second.data(), iterY->second.data(), sizeX);
		if(debug)
			std::cout << " debug: Added property \"" << args.at(1) << "\" to property table\n";
	}
	else if(cmd == "setVisColor"){
		double red = strtod(args.at(1).c_str(), NULL);
		double green = strtod(args.at(2).c_str(), NULL);
		double blue = strtod(args.at(3).c_str(), NULL);
		double alpha = (nArgs >= 5 ? strtod(args.at(4).c_str(), NULL) : 1);
		visatt->SetColor(red, green, blue, alpha);
		if(debug)
			std::cout << " debug: Set material color to (" << red << ", " << green << ", " << blue << ") alpha=" << alpha << std::endl;
	}
	else if(cmd == "setVisLineStyle"){ // Set the line style of the visual attributes
		if(args.at(1) == "unbroken")
			visatt->SetLineStyle(G4VisAttributes::unbroken);
		else if(args.at(1) == "dashed")
			visatt->SetLineStyle(G4VisAttributes::dashed);
		else if(args.at(1) == "dotted")
			visatt->SetLineStyle(G4VisAttributes::dotted);
		else
			std::cout << Display::WarningStr("nDetDynamicMaterial") << "User specified invalid visual line style (" << args.at(1) << ")\n" << Display::ResetStr();
	}
	else if(cmd == "setVisLineWidth"){ // Set the line width of the visual attributes
		double width = strtod(args.at(1).c_str(), NULL);
		visatt->SetLineWidth(width);
		if(debug)
			std::cout << " debug: Set material line width to " << width << std::endl;
	}
	else if(cmd == "setVisWireframe"){
		bool userInput = true;
		if(nArgs > 2 && !getBoolInput(args.at(1), userInput))
			return false;
		visatt->SetForceWireframe(userInput);
	}
	else if(cmd == "setVisSolid"){
		bool userInput = true;
		if(nArgs > 2 && !getBoolInput(args.at(1), userInput))
			return false;
		visatt->SetForceSolid(userInput);
	}
	
	return true;
}

bool nDetDynamicMaterial::isInElementMap(const std::string &mName){
	return (elements.find(mName) != elements.end());
}

bool nDetDynamicMaterial::isInVariableMap(const std::string &mName){
	return (variables.find(mName) != variables.end());
}
