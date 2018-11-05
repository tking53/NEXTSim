#ifndef PHOTON_COUNTER_HPP
#define PHOTON_COUNTER_HPP

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
	std::vector<unsigned int> Nphotons;
	
	unsigned int Ntotal;
	
	int size;
};

#endif
