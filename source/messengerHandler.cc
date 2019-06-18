
#include "G4UIcmdWithAString.hh"

#include "messengerHandler.hh"

const std::string ansiColorGreen = "\x1b[32m";
const std::string ansiColorReset = "\x1b[0m";

///////////////////////////////////////////////////////////////////////////////
// class messengerHandler
///////////////////////////////////////////////////////////////////////////////

messengerHandler::messengerHandler() : G4UImessenger(), name(), size(0) { 
}

messengerHandler::messengerHandler(const std::string &name_) : G4UImessenger(), name(name_), size(0) { 
}

messengerHandler::~messengerHandler(){
	for(std::vector<G4UIdirectory*>::iterator iter = fDir.begin(); iter != fDir.end(); iter++){
		delete (*iter);
	}
	for(std::vector<G4UIcommand*>::iterator iter = fCmd.begin(); iter != fCmd.end(); iter++){
		delete (*iter);
	}
}

void messengerHandler::SetNewValue(G4UIcommand* command, G4String newValue){
	fUserCmdList.push_back(command->GetCommandPath() + " " + newValue);
	this->SetNewChildValue(command, newValue); 
}

bool messengerHandler::searchForString(const std::string &str, std::vector<cmdSearchPair> &matches) const {
	bool retval = false;
	for(size_t i = 0; i < size; i++){
		std::string path = fCmd[i]->GetCommandPath();
		if(path.find(str) != std::string::npos){
			size_t findIndex = path.find(str);
			if(findIndex != std::string::npos)
				path.replace(findIndex, str.length(), (ansiColorGreen+str+ansiColorReset));
			bool matchInList = false;
			cmdSearchPair currMatch(fCmd[i], fCmd[i]->GetCommandPath(), path);
			for(std::vector<cmdSearchPair>::iterator iter = matches.begin(); iter != matches.end(); iter++){ // Check for duplicates.
				if((*iter) == currMatch){
					matchInList = true;
					break;
				}
			}
			if(!matchInList)
				matches.push_back(currMatch);
			retval = true;
		}
	}
	
	return retval;
}

void messengerHandler::getAllCommands(std::vector<cmdSearchPair> &commands) const {
	commands.clear();
	for(size_t i = 0; i < size; i++){
		commands.push_back(cmdSearchPair(fCmd[i], fCmd[i]->GetCommandPath(), ""));
	}	
}

void messengerHandler::printAllCommands() const {
	for(size_t i = 0; i < size; i++){
		std::cout << " " << fCmd[i]->GetCommandPath() << std::endl;
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
	fCmdArg.push_back("");
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
			fCmdArg[i] = str;
			if(verbose)
				std::cout << "User Command: " << fCmd[i]->GetCommandPath() << " \"" << str << "\"\n";
			return true;
		}
	}	
	return false;
}
