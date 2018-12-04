
#include "G4UIcmdWithAString.hh"

#include "TFile.h"

#include "messengerHandler.hh"

messengerHandler::~messengerHandler(){
	for(std::vector<G4UIdirectory*>::iterator iter = fDir.begin(); iter != fDir.end(); iter++){
		delete (*iter);
	}
	for(std::vector<G4UIcommand*>::iterator iter = fCmd.begin(); iter != fCmd.end(); iter++){
		delete (*iter);
	}
}

void messengerHandler::addDirectory(const std::string &str, const std::string &guidance/*=""*/){
	fDir.push_back(new G4UIdirectory(str.c_str()));
	if(!guidance.empty())
		fDir.back()->SetGuidance(guidance.c_str());
}

void messengerHandler::addCommand(G4UIcommand* cmd){
	fCmd.push_back(cmd);
}

void messengerHandler::addGuidance(const std::string &str){
	fCmd.back()->SetGuidance(str.c_str());
}

void messengerHandler::addCandidates(const std::string &str){
	((G4UIcmdWithAString*)fCmd.back())->SetCandidates(str.c_str());
}

bool messengerHandler::findCommand(G4UIcommand *cmd, size_t &index){
	size_t i = 0;
	for(std::vector<G4UIcommand*>::iterator iter = fCmd.begin(); iter != fCmd.end(); iter++){
		if(cmd == (*iter)){
			index = i;
			return true;
		}
		i++;
	}	
	return false;
}
