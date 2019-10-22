#ifndef PHOTON_COUNTER_HH
#define PHOTON_COUNTER_HH

#include <vector>

/** @class photonCounter
  * @brief Counts the number of optical photons which are produced for a given primary track ID
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 12, 2019
  */

class photonCounter{
  public:
	/** Default constructor
	  */
	photonCounter() : Ntotal(0), size(0) { }

	/** Add a new track ID to the list
	  *
	  * Will add new IDs to the list until the length of the vector is greater than @a id
	  */
	void push_back(const int &id);

	/** Increment the photon count for a specified track ID
	  * 
	  * If the track ID does not exist in the vector, then the vector will be extended
	  */
	void addPhoton(const int &id);
	
	/** Get the number of photons for a specified track ID
	  */
	unsigned int getPhotonCount(const int &id) const ;

	/** Get the total number of photons
	  */
	unsigned int getTotalPhotonCount() const { return Ntotal; }
	
	/** Print the total number of photon counts for each track ID
	  */
	void print() const ;
	
	/** Clear the counting vector and zero the total number of photons
	  */
	void clear();

  private:
	std::vector<unsigned int> Nphotons; ///< Vector of optical photon counts whose index is equal to the track ID of the parent particle
	
	unsigned int Ntotal; ///< Total number of photons
	
	int size; ///< Size of photon count vector. Equal to the maximum parent track ID plus 1
};

#endif
