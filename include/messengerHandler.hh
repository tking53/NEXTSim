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

class messengerHandler : public G4UImessenger {
  public:
	messengerHandler();

	messengerHandler(const std::string &name_);
	
	virtual ~messengerHandler();
	
	virtual void write(TDirectory*);

    virtual void SetNewValue(G4UIcommand*, G4String){ }

	size_t getSize() const { return size; }
	
	std::string getName() const { return name; }
    
	bool searchForString(const std::string &str, std::vector<std::string> &matches, bool color=false) const ;
	
	void printAllCommands() const ;

  protected:
	/// Create a new directory.
	void addDirectory(const std::string &str, const std::string &guidance="");
  
	/// Add a new command to the vector.
	void addCommand(G4UIcommand* cmd);
	
	/// Add guidance to the most recently added command.
	void addGuidance(const std::string &str);
	
	/// Add candidates to the most recently added command.
	void addCandidates(const std::string &str);

	/// Find a command pointed to by cmd and return the index in the vector.
	bool findCommand(G4UIcommand *cmd, const std::string &str, size_t &index, const bool &verbose=false);

	/// Get the most recently added command.
	G4UIcommand* getBack(){ return fCmd.back(); }

  private:
	std::vector<G4UIdirectory*> fDir;
  
	std::vector<G4UIcommand*> fCmd;
	
	std::vector<bool> fCmdCalled;
	
	std::vector<std::string> fCmdArg;
	
	std::string name;
	
	size_t size;
};

#endif
