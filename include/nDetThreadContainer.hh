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

	void addAction(const nDetRunAction* ptr, const int &threadID){ actions.push_back(std::pair<const nDetRunAction*, int>(ptr, threadID)); }
	
	size_t size() const { return actions.size(); }
	
	std::vector<std::pair<const nDetRunAction*, int> > *getAction(){ return &actions; }
	
	std::pair<const nDetRunAction*, int> *getAction(const size_t &index){ return &actions.at(index); }
	
  private:
	std::vector<std::pair<const nDetRunAction*, int> > actions;

	// Private constructor.
	nDetThreadContainer(){ }
};

#endif
