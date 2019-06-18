#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include "nDetMasterOutputFileMessenger.hh"
#include "nDetConstructionMessenger.hh"
#include "nDetParticleSourceMessenger.hh"

void help(char * prog_name_){
	std::cout << "  SYNTAX: " << prog_name_ << " [str1 str2 ...]\n";
	std::cout << "   Available options:\n";
	std::cout << "    --help (-h)    | Display this dialogue.\n";
	std::cout << "    --verbose (-V) | Display command syntax information.\n";
}

int main(int argc, char *argv[]){
	if(argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)){
		help(argv[0]);
		return 0;
	}

	bool verbose = false;
	int index = 1;
	std::vector<std::string> searchStrings;
	while(index < argc){
		if(strcmp(argv[index], "--verbose") == 0 || strcmp(argv[index], "-V") == 0)
			verbose = true;
		else
			searchStrings.push_back(std::string(argv[index]));
		index++;
	}

	nDetMasterOutputFileMessenger outputMessenger;
	nDetConstructionMessenger constructionMessenger;
	nDetParticleSourceMessenger sourceMessenger;

	const size_t numMessengers = 3;
	messengerHandler *handlers[numMessengers] = {&outputMessenger, &constructionMessenger, &sourceMessenger};

	std::string msgNames[numMessengers];
	size_t msgNumCmds[numMessengers];

	for(size_t i = 0; i < numMessengers; i++){
		msgNames[i] = handlers[i]->getName();
		msgNumCmds[i] = handlers[i]->getSize();
	}

	if(searchStrings.empty()){
		for(size_t i = 0; i < numMessengers; i++){
			std::cout << msgNames[i] << ": (" << msgNumCmds[i] << " commands)\n";
			std::vector<cmdSearchPair> commands;
			handlers[i]->getAllCommands(commands);
			size_t maxLength = 0;
			for(std::vector<cmdSearchPair>::iterator iter = commands.begin(); iter != commands.end(); iter++){
				if(iter->unformatted.length() > maxLength)
					maxLength = iter->unformatted.length();
			}
			for(std::vector<cmdSearchPair>::iterator iter = commands.begin(); iter != commands.end(); iter++){
				std::cout << " " << iter->unformatted;
				if(verbose)
					std::cout << std::string(maxLength-iter->unformatted.length(), ' ') << " | " << iter->command->GetTitle();
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
	else{
		std::vector<cmdSearchPair> matches[numMessengers];
		for(size_t i = 0; i < numMessengers; i++){
			for(std::vector<std::string>::iterator iter = searchStrings.begin(); iter != searchStrings.end(); iter++){
				handlers[i]->searchForString(*iter, matches[i]);
			}
		}
		for(size_t i = 0; i < numMessengers; i++){
			if(matches[i].empty()) continue;
			std::cout << msgNames[i] << ": (" << msgNumCmds[i] << " commands, " << matches[i].size() << " matches)\n";
			size_t maxLength = 0;
			for(std::vector<cmdSearchPair>::iterator iter = matches[i].begin(); iter != matches[i].end(); iter++){
				if(iter->formatted.length() > maxLength)
					maxLength = iter->formatted.length();
			}
			for(std::vector<cmdSearchPair>::iterator iter = matches[i].begin(); iter != matches[i].end(); iter++){
				std::cout << " " << iter->formatted;
				if(verbose)
					std::cout << std::string(maxLength-iter->formatted.length(), ' ') << " | " << iter->command->GetTitle();
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
