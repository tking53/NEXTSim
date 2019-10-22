#ifndef NDET_THREAD_CONTAINER_HH
#define NDET_THREAD_CONTAINER_HH

#include <vector>
#include <utility>

#include "nDetActionInitialization.hh"
#include "nDetRunAction.hh"

/** @class nDetThreadContainer
  * @brief Collection of userActionManager objects for master and worker threads
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date August 9, 2019
  */

class nDetThreadContainer{
  public:
	/** Destructor
	  */
	~nDetThreadContainer(){ }

	/** Copy constructor. Not implemented for singleton class
	  */
	nDetThreadContainer(nDetThreadContainer const &copy);

	/** Assignment operator. Not implemented for singleton class
	  */
	nDetThreadContainer &operator=(nDetThreadContainer const &copy);

	/** Get an instance of this singleton
	  */
	static nDetThreadContainer &getInstance(){
		// The only instance
		// Guaranteed to be lazy initialized
		// Guaranteed that it will be destroyed correctly
		static nDetThreadContainer instance;
		return instance;
	}

	/** Set the user run action pointer for the master thread and enable the multithreading flag
	  */
	void setMaster(nDetRunAction* ptr){ master = ptr; multithreading = true; }

	/** Add a thread-local user action manager to the vector of all threads
	  */
	void addAction(const userActionManager &manager){ actions.push_back(manager); }
	
	/** Get the size of the vector of user action managers for all threads
	  */
	size_t size() const { return actions.size(); }
	
	/** Return a pointer to the vector of all thread-local user action managers for all threads
	  */
	std::vector<userActionManager> *getAction(){ return &actions; }
	
	/** Get a pointer to the run action of the thread at a specified index
	  * @param index The ID of the thread whose run action will be returned
	  * @return A pointer to the thread's run action if it exists in the vector and return NULL otherwise
	  */
	userActionManager *getActionManager(const size_t &index){ return (index < actions.size() ? &actions.at(index) : NULL); }

	/** Get a pointer to the run action of the master thread
	  */
	nDetRunAction *getMasterRunAction(){ return master; }
	
	/** Return true if NEXTSim is running in multithreaded mode and return false otherwise
	  */
	bool getMultithreadingMode() const { return multithreading; }
	
  private:
	std::vector<userActionManager> actions; ///< Vector of all thread-local user action managers for all threads

	nDetRunAction* master; ///< User run action for the master thread

	bool multithreading; ///< Flag indicating that the program is running in multithreading mode

	/** Private constructor (for singleton class)
	  */
	nDetThreadContainer() : master(NULL), multithreading(false) { }
};

#endif
