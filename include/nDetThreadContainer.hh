#ifndef NDET_THREAD_CONTAINER_HH
#define NDET_THREAD_CONTAINER_HH

#include <vector>
#include <utility>

#include "nDetActionInitialization.hh"
#include "nDetRunAction.hh"

class nDetThreadContainer{
  public:
	~nDetThreadContainer(){ }

	nDetThreadContainer(nDetThreadContainer const &copy); // Not Implemented
	
	nDetThreadContainer &operator=(nDetThreadContainer const &copy); // Not Implemented

	static nDetThreadContainer &getInstance(){
		// The only instance
		// Guaranteed to be lazy initialized
		// Guaranteed that it will be destroyed correctly
		static nDetThreadContainer instance;
		return instance;
	}

	void setMaster(nDetRunAction* ptr){ master = ptr; multithreading = true; }

	void addAction(const userActionManager &manager){ actions.push_back(manager); }
	
	size_t size() const { return actions.size(); }
	
	std::vector<userActionManager> *getAction(){ return &actions; }
	
	userActionManager *getActionManager(const size_t &index){ return &actions.at(index); }
	
	nDetRunAction *getMasterRunAction(){ return master; }
	
	bool getMultithreadingMode() const { return multithreading; }
	
  private:
	std::vector<userActionManager> actions; ///< Vector of pairs of all thread-local user run actions and their thread IDs

	nDetRunAction* master; ///< User run action for the master thread

	bool multithreading; ///< Flag indicating that the program is running in multithreaded mode

	// Private constructor.
	nDetThreadContainer() : master(NULL), multithreading(false) { }
};

#endif
