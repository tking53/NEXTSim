#ifndef MESSENGER_HANDLER_HH
#define MESSENGER_HANDLER_HH

#include <vector>
#include <string>
#include <iostream>

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class TDirectory;

/**Split a string on the delimiter character populating the vector args with 
 * any substrings formed. Returns the number of substrings found.
 *	
 * \param[in] str The string to be parsed.
 * \param[out] args The vector to populate with substrings.
 * \param[in] delimiter The character to split the string on.
 * \return The number of substrings found.
 */
unsigned int split_str(std::string str, std::vector<std::string> &args, char delimiter=' ');

///////////////////////////////////////////////////////////////////////////////
// class messengerHandler
///////////////////////////////////////////////////////////////////////////////

class cmdSearchPair{
  public:
	G4UIcommand *command; ///< Pointer to the geant UI command
	std::string unformatted; ///< 
	std::string formatted; ///< 
  
	cmdSearchPair() : command(NULL) { }
	
	cmdSearchPair(G4UIcommand *cmd, const std::string &str1, const std::string &str2) : command(cmd), unformatted(str1), formatted(str2) { }
	
	bool operator == (const cmdSearchPair &other) const { return (this->command == other.command); }
};

///////////////////////////////////////////////////////////////////////////////
// class messengerHandler
///////////////////////////////////////////////////////////////////////////////

class messengerHandler : public G4UImessenger {
  public:
	messengerHandler();

	messengerHandler(const std::string &name_);
	
	virtual ~messengerHandler();
	
	virtual void write(TDirectory*);

    virtual void SetNewValue(G4UIcommand*, G4String){ }

	size_t getSize() const { return size; }
	
	std::string getName() const { return name; }
    
	bool searchForString(const std::string &str, std::vector<cmdSearchPair> &matches) const ;
	
	void getAllCommands(std::vector<cmdSearchPair> &commands) const ;
	
	void printAllCommands() const ;

  protected:
	///< Create a new directory.
	void addDirectory(const std::string &str, const std::string &guidance="");
  
	///< Add a new command to the vector.
	void addCommand(G4UIcommand* cmd);
	
	///< Add guidance to the most recently added command.
	void addGuidance(const std::string &str);
	
	///< Add candidates to the most recently added command.
	void addCandidates(const std::string &str);

	///< Find a command pointed to by cmd and return the index in the vector.
	bool findCommand(G4UIcommand *cmd, const std::string &str, size_t &index, const bool &verbose=false);

	///< Get the most recently added command.
	G4UIcommand* getBack(){ return fCmd.back(); }

  private:
	std::vector<G4UIdirectory*> fDir; ///< Vector of UI directories which are added
  
	std::vector<G4UIcommand*> fCmd; ///< Vector of UI commands which are added
	
	std::vector<bool> fCmdCalled; /*!< Vector of bool flags for all commands
	                                   These flags are set to true if a command is called by the user */
	
	std::vector<std::string> fCmdArg; /*!< Vector of argument strings for all commands
	                                       The strings are empty unless the command is called by the user 
	                                       in which case the string is set to the argument supplied by the user */
	
	std::string name; ///< Name of the class for which this messenger is used
	
	size_t size; ///< Number of defined commands
};

#endif
