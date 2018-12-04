#ifndef MESSENGER_HANDLER_HH
#define MESSENGER_HANDLER_HH

#include <vector>
#include <string>

#include "G4UImessenger.hh"

class G4UIcommand;
class G4UIdirectory;
class TDirectory;

class messengerHandler : public G4UImessenger {
  public:
	messengerHandler() : G4UImessenger(), size(0) { }
	
	virtual ~messengerHandler();
	
	virtual void write(TDirectory*);

    virtual void SetNewValue(G4UIcommand*, G4String){ }

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
	
	std::vector<std::string> fCmdString;
	
	size_t size;
};

#endif
