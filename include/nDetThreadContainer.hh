#ifndef NDET_THREAD_CONTAINER_HH
#define NDET_THREAD_CONTAINER_HH

#include <vector>
#include <utility>

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

	void addAction(nDetRunAction* ptr, const int &threadID){ actions.push_back(std::pair<nDetRunAction*, int>(ptr, threadID)); }
	
	size_t size() const { return actions.size(); }
	
	std::vector<std::pair<nDetRunAction*, int> > *getAction(){ return &actions; }
	
	nDetRunAction *getAction(const size_t &index){ return actions.at(index).first; }
	
	int getThreadID(const size_t &index){ return actions.at(index).second; }
	
  private:
	std::vector<std::pair<nDetRunAction*, int> > actions; ///< Vector of pairs of all thread-local user run actions and their thread IDs

	// Private constructor.
	nDetThreadContainer(){ }
};

#endif
