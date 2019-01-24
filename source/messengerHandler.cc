
#include "G4UIcmdWithAString.hh"

#include "TDirectory.h"
#include "TNamed.h"

#include "messengerHandler.hh"

messengerHandler::~messengerHandler(){
	for(std::vector<G4UIdirectory*>::iterator iter = fDir.begin(); iter != fDir.end(); iter++){
		delete (*iter);
	}
	for(std::vector<G4UIcommand*>::iterator iter = fCmd.begin(); iter != fCmd.end(); iter++){
		delete (*iter);
	}
}

void messengerHandler::write(TDirectory *dir){
	dir->cd();
	for(size_t i = 0; i < size; i++){
		if(fCmdCalled[i]){ // Write the command to the output file.
			std::string output = fCmd[i]->GetCommandPath() + " " + fCmdString[i];
			std::string cmd = fCmd[i]->GetCommandPath();
			size_t index = cmd.find_last_of('/');
			if(index != std::string::npos){
				cmd = cmd.substr(index+1);
			}
			TNamed named(cmd.c_str(), output.c_str());
			named.Write();
		}
	}		
}

void messengerHandler::addDirectory(const std::string &str, const std::string &guidance/*=""*/){
	fDir.push_back(new G4UIdirectory(str.c_str()));
	if(!guidance.empty())
		fDir.back()->SetGuidance(guidance.c_str());
}

void messengerHandler::addCommand(G4UIcommand* cmd){
	fCmd.push_back(cmd);
	fCmdCalled.push_back(false);
	fCmdString.push_back(std::string());
	size++;
}

void messengerHandler::addGuidance(const std::string &str){
	fCmd.back()->SetGuidance(str.c_str());
}

void messengerHandler::addCandidates(const std::string &str){
	((G4UIcmdWithAString*)fCmd.back())->SetCandidates(str.c_str());
}

bool messengerHandler::findCommand(G4UIcommand *cmd, const std::string &str, size_t &index, const bool &verbose/*=false*/){
	for(size_t i = 0; i < size; i++){
		if(cmd == fCmd[i]){
			index = i;
			fCmdCalled[i] = true;
			fCmdString[i] = str;
			if(verbose)
				std::cout << "User Command: " << fCmd[i]->GetCommandPath() << " \"" << fCmdString[i] << "\"\n";
			return true;
		}
	}	
	return false;
}
