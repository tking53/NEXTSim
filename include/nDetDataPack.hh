#ifndef NDET_DATA_PACK_HH
#define NDET_DATA_PACK_HH

#include <vector>

#include "Structures.hpp"

class TBranch;

/*! \class nDetDataPack
 *  \brief Class used to store simulated data and output variables
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 23, 2019
*/

class nDetDataPack{
  public:
	nDetDataPack() : evtData(NULL), outData(NULL), multData(NULL), debugData(NULL) { }

	nDetDataPack(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug) : evtData(evt), outData(out), multData(mult), debugData(debug) { }

	void setDataAddresses(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug);

	void setBranchAddresses(TBranch *evt_b, TBranch *out_b, TBranch *mult_b, TBranch *debug_b) const ;

	void copyData(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug) const ;

	bool goodEvent() const ;

	int getEventID() const ;

	/** Clear all variables and vectors
	  */
	void clear();
	
  private:
	nDetEventStructure *evtData;
	nDetOutputStructure *outData;
	nDetMultiOutputStructure *multData;
	nDetDebugStructure *debugData;
};

#endif
