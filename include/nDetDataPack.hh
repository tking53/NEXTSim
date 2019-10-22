#ifndef NDET_DATA_PACK_HH
#define NDET_DATA_PACK_HH

#include <vector>

#include "nDetStructures.hpp"

class TBranch;

/** @class nDetDataPack
  * @brief Stores simulated data and output variables
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date May 9, 2019
  */

class nDetDataPack{
  public:
	/** Default constructor
	  */
	nDetDataPack() : evtData(NULL), outData(NULL), multData(NULL), debugData(NULL), traceData(NULL) { }

	/** Data structure constructor
	  */
	nDetDataPack(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug, nDetTraceStructure *trace) : 
	  evtData(evt), outData(out), multData(mult), debugData(debug), traceData(trace) { }

	void setDataAddresses(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug, nDetTraceStructure *trace);

	void copyData(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug, nDetTraceStructure *trace) const ;

	/** Return true if the current event is a good detection event, meaning that optical photons
	  * were detected at the photo-sensitive surfaces of the detector, and return false otherwise
	  */
	bool goodEvent() const ;

	/** Get the ID of the current event
	  */
	int getEventID() const ;

	/** Clear all variables and vectors
	  */
	void clear();
	
  private:
	nDetEventStructure *evtData;
	nDetOutputStructure *outData;
	nDetMultiOutputStructure *multData;
	nDetDebugStructure *debugData;
	nDetTraceStructure *traceData;
};

#endif
