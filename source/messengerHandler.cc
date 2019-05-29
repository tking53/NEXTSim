
#include "G4UIcmdWithAString.hh"

#include "TDirectory.h"
#include "TNamed.h"

#include "messengerHandler.hh"

const std::string ansiColorGreen = "\x1b[32m";
const std::string ansiColorReset = "\x1b[0m";

/**Split a string on the delimiter character populating the vector args with 
 * any substrings formed. Returns the number of substrings found.
 *	
 * \param[in] str The string to be parsed.
 * \param[out] args The vector to populate with substrings.
 * \param[in] delimiter The character to split the string on.
 * \return The number of substrings found.
 */
unsigned int split_str(std::string str, std::vector<std::string> &args, char delimiter/*=' '*/){
	args.clear();
	
	//Locate the first non delimiter space.
	size_t strStart = str.find_first_not_of(delimiter);
	//In case of a line with only delimiters we return 0.
	if (strStart == std::string::npos) return 0;

	//Locate the last character that is not a delimiter.
	size_t strStop = str.find_last_not_of(delimiter) + 1;

	//We loop over the string searching for the delimiter keeping track of where 
	// we found the current delimiter and the previous one.
	size_t pos = strStart;
	size_t lastPos = strStart;
	while ((pos = str.find(delimiter, lastPos)) != std::string::npos) {
		//Store the substring from the last non delimiter to the current delimiter.
		args.push_back(str.substr(lastPos, pos - lastPos));
		
		//We update the last position looking for the first character that is not
		// a delimiter.
		lastPos = str.find_first_not_of(delimiter, pos+1);
	}
	
	//Store the last string.
	if(lastPos != std::string::npos)
		args.push_back(str.substr(lastPos, strStop - lastPos));

	return args.size();
}

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

void messengerHandler::write(TDirectory *dir){
	dir->cd();
	for(std::vector<std::string>::iterator iter = fUserCmdList.begin(); iter != fUserCmdList.end(); iter++){ // Write the commands to the output file.
		std::string cmd = (*iter);
		size_t index1 = iter->find_last_of('/');
		if(index1 != std::string::npos){
			size_t index2 = iter->find_first_of(' ');
			if(index2 != std::string::npos)
				cmd = cmd.substr(index1+1, index2-(index1+1));
			else
				cmd = cmd.substr(index1+1);
		}
		TNamed named(cmd.c_str(), iter->c_str());
		named.Write();
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
