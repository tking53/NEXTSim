#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include "nistDatabase.hh"

void help(char * prog_name_){
	std::cout << "  SYNTAX: " << prog_name_ << " <filename> [options] [output]\n";
	std::cout << "   Available options:\n";
	std::cout << "    --help (-h)            | Display this dialogue.\n";
	std::cout << "    --search (-s) <string> | Search for \"string\" in NIST database.\n";
	std::cout << "    --list-elements        | List all elements defined in NIST database.\n";
	std::cout << "    --list-materials       | List all materials defined in NIST database.\n";
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
	bool printElements = false;
	bool printMaterials = false;
	std::string searchString;
	while(index < argc){
		if(strcmp(argv[index], "--search") == 0 || strcmp(argv[index], "-s") == 0){
			if(argc <= index+1){
				std::cout << " Error: Invalid number of arguments to --search.\n";
				help(argv[0]);
				return 1;
			}
			searchString = std::string(argv[++index]);
		}
		else if(strcmp(argv[index], "--list-elements") == 0){
			printElements = true;
		}
		else if(strcmp(argv[index], "--list-materials") == 0){
			printMaterials = true;
		}
		else{ 
			std::cout << " Error! Unrecognized option '" << argv[index] << "'!\n";
			help(argv[0]);
			return 1;
		}
		index++;
	}

	// Get an instance of the NIST manager.
	nistDatabase nist;
	
	if(printElements || !searchString.empty())
		std::cout << "nistList: Found " << nist.getNumberElements() << " elements in NIST database\n";
	
	if(printMaterials || !searchString.empty())
		std::cout << "nistList: Found " << nist.getNumberMaterials() << " materials in NIST database\n";
	
	// Print all elements.
	if(printElements)
		nist.printElements();

	// Print all materials.
	if(printMaterials)
		nist.printMaterials();
	
	// Search the database for a string.
	if(!searchString.empty()){
		std::cout << "nistList: Searching for string \"" << searchString << "\" in database\n";
		std::cout << "\n-G4Element-\n";
		size_t elementCount = nist.searchElementList(searchString);
		std::cout << "\n-G4Material-\n";
		size_t materialCount = nist.searchMaterialList(searchString);
		std::cout << "\nnistList: Identified " << elementCount << " defined element" << (elementCount == 1 ? "" : "s") << " and " << materialCount << " defined material" << (materialCount == 1 ? "" : "s") << "\n"; 
	}

	return 0;
}
