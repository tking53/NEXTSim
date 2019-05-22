#ifndef PHOTON_COUNTER_HH
#define PHOTON_COUNTER_HH

#include <vector>

class photonCounter{
  public:
	photonCounter() : Ntotal(0), size(0) { }

	void push_back(const int &id);

	void addPhoton(const int &id);
	
	unsigned int getPhotonCount(const int &id) const ;

	unsigned int getTotalPhotonCount() const { return Ntotal; }
	
	void print() const ;
	
	void clear();

  private:
	std::vector<unsigned int> Nphotons; ///< Vector of optical photon counts whose index is equal to the track ID of the parent particle
	
	unsigned int Ntotal; ///< Total number of photons
	
	int size; ///< Size of photon count vector. Equal to the maximum parent track ID plus 1
};

#endif
