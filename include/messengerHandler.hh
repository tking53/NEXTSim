#ifndef MESSENGER_HANDLER_HH
#define MESSENGER_HANDLER_HH

#include <vector>
#include <string>
#include <iostream>

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class TDirectory;

/** Split a string on the delimiter character populating the vector args with 
  * any substrings formed. Returns the number of substrings found.
  *	
  * @param str The string to be parsed
  * @param args The vector to populate with substrings
  * @param delimiter The character to split the string on
  * @return The number of substrings found
  */
unsigned int split_str(std::string str, std::vector<std::string> &args, char delimiter=' ');

/*! \class cmdSearchPair
 *  \brief Class used link a formatted command path to a G4UIcommand object
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class cmdSearchPair{
  public:
	G4UIcommand *command; ///< Pointer to the geant UI command
	std::string unformatted; ///< Unformatted command path
	std::string formatted; ///< Formatted command path

	/** Default constructor
	  */
	cmdSearchPair() : command(NULL) { }
	
	/** Constructor taking a G4UIcommand
	  * @param cmd Pointer to the G4UIcommand
	  * @param str1 The unformatted command path
	  * @param str2 The output-formatted command path
	  */
	cmdSearchPair(G4UIcommand *cmd, const std::string &str1, const std::string &str2) : command(cmd), unformatted(str1), formatted(str2) { }

	/** Equal to operator
	  * @return True if this pair points to the same G4UIcommand as another and return false otherwise.
	  */
	bool operator == (const cmdSearchPair &other) const { return (this->command == other.command); }
};

/*! \class messengerHandler
 *  \brief Class used to manage all commands needed for the user to interface with NEXTSim
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class messengerHandler : public G4UImessenger {
  public:
	/** Default constructor
	  */
	messengerHandler();

	/** Named constructor
	  * @param name_ The class name of this messenger
	  */	
	messengerHandler(const std::string &name_);

	/** Virtual destructor
	  */
	virtual ~messengerHandler();

	/** Write all user-called commands to an output TFile
	  * @param dir Pointer to the current directory of an output TFile
	  */
	virtual void write(TDirectory* dir);

	/** Add a command to the list of commands the user has called. Called from G4UIcommand::DoIt()
	  * @param command Pointer to the G4UIcommand called by the user
	  * @param newValue String containing the argument passed by the user
	  */
	virtual void SetNewValue(G4UIcommand* command, G4String newValue);

	/** Virtual method used by derived classes to handle command calls. Called from SetNewValue()
	  */
	virtual void SetNewChildValue(G4UIcommand*, G4String){ }

	/** Get the number of commands defined for this messenger
	  */
	size_t getSize() const { return size; }

	/** Get the class name of this messenger
	  */	
	std::string getName() const { return name; }

	/** Search for a substring in the paths of all defined commands
	  * @param str Search string
	  * @param matches Vector of search pairs containing any matches
	  * @return True if at least one exact match was found and return false otherwise
	  */
	bool searchForString(const std::string &str, std::vector<cmdSearchPair> &matches) const ;

	/** Get search pairs for all defined commands
	  * @param commands Vector to fill with search pairs of all commands
	  */
	void getAllCommands(std::vector<cmdSearchPair> &commands) const ;

	/** Print all defined command paths to stdout
	  */
	void printAllCommands() const ;

  protected:
	/** Create a new command directory
	  * @param str The full command path
	  * @param guidance Command guidance to aid the user
	  */
	void addDirectory(const std::string &str, const std::string &guidance="");
  
	/** Add a new command to the vector
	  * @param cmd Pointer to a command object inheriting from G4UIcommand
	  */
	void addCommand(G4UIcommand* cmd);
	
	/** Add guidance to the most recently added command
	  * @param str Guidance string to add to the most recently defined command
	  */
	void addGuidance(const std::string &str);
	
	/** Add candidates to the most recently added command
	  * @param str Space delimited list of candidates to add to the most recently defined command
	  */
	void addCandidates(const std::string &str);

	/** Find a command pointed to by cmd and return its index in the vector
	  * @param cmd Pointer to a G4UIcommand called by the user
	  * @param index The index of the command in the list of defined commands, if it is found
	  * @param verbose Flag indicating that the user input will be echoed to stdout
	  * @return True if the command was found in the command list and return false otherwise
	  */
	bool findCommand(G4UIcommand *cmd, const std::string &str, size_t &index, const bool &verbose=false);

	/** Get a pointer to the most recently added command
	  */
	G4UIcommand* getBack(){ return fCmd.back(); }

  private:
	std::vector<G4UIdirectory*> fDir; ///< Vector of UI directories which are added
  
	std::vector<G4UIcommand*> fCmd; ///< Vector of UI commands which are added
	
	std::vector<bool> fCmdCalled; /*!< Vector of bool flags for all commands
									   These flags are set to true if a command is called by the user */
	
	std::vector<std::string> fCmdArg; /*!< Vector of argument strings for all commands
										   The strings are empty unless the command is called by the user 
										   in which case the string is set to the argument supplied by the user */
	
	std::vector<std::string> fUserCmdList; ///< Vector of all macro commands called by the user
	
	std::string name; ///< Name of the class for which this messenger is used
	
	size_t size; ///< Number of defined commands
};

#endif
