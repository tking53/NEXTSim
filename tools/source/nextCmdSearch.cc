#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include "nDetConstructionMessenger.hh"
#include "nDetRunActionMessenger.hh"
#include "ParticleSourceMessenger.hh"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void help(char * prog_name_){
	std::cout << "  SYNTAX: " << prog_name_ << " [str1 str2 ...]\n";
	std::cout << "   Available options:\n";
	std::cout << "    --help (-h) | Display this dialogue.\n";
}

int main(int argc, char *argv[]){
	if(argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)){
		help(argv[0]);
		return 0;
	}

	int index = 1;
	std::vector<std::string> searchStrings;
	while(index < argc){
		searchStrings.push_back(std::string(argv[index]));
		index++;
	}

	nDetConstructionMessenger constructionMessenger;
	nDetRunActionMessenger runActionMessenger;
	ParticleSourceMessenger sourceMessenger;

	messengerHandler *handlers[3] = {&constructionMessenger, &runActionMessenger, &sourceMessenger};

	std::string msgNames[3];
	size_t msgNumCmds[3];

	for(size_t i = 0; i < 3; i++){
		msgNames[i] = handlers[i]->getName();
		msgNumCmds[i] = handlers[i]->getSize();
	}

	if(searchStrings.empty()){
		for(size_t i = 0; i < 3; i++){
			std::cout << msgNames[i] << ": (" << msgNumCmds[i] << " commands)\n";
			handlers[i]->printAllCommands();
			std::cout << std::endl;
		}
	}
	else{
		std::vector<std::string> matches[3];
		for(size_t i = 0; i < 3; i++){
			for(std::vector<std::string>::iterator iter = searchStrings.begin(); iter != searchStrings.end(); iter++){
				handlers[i]->searchForString(*iter, matches[i], true);
			}
		}
		for(size_t i = 0; i < 3; i++){
			if(matches[i].empty()) continue;
			std::cout << msgNames[i] << ": (" << msgNumCmds[i] << " commands, " << matches[i].size() << " matches)\n";
			for(std::vector<std::string>::iterator iter = matches[i].begin(); iter != matches[i].end(); iter++){
				std::cout << " " << *iter << std::endl;
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
