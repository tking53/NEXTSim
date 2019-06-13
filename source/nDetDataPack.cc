#include <iostream>

#include "TBranch.h"

#include "nDetDataPack.hh"

void nDetDataPack::setDataAddresses(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug, nDetTraceStructure *trace){
	evtData = evt;
	outData = out;
	multData = mult;
	debugData = debug;
	traceData = trace;
}

void nDetDataPack::copyData(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug, nDetTraceStructure *trace) const {
	(*evt) = (*evtData);
	(*out) = (*outData);
	(*mult) = (*multData);
	(*debug) = (*debugData);
	(*trace) = (*traceData);
}

bool nDetDataPack::goodEvent() const {
	return (evtData->goodEvent || (debugData->nPhotons[0] > 0 || debugData->nPhotons[1] > 0));
}

int nDetDataPack::getEventID() const {
	return evtData->eventID;
}

void nDetDataPack::clear(){
	evtData->Zero();
	outData->Zero();
	multData->Zero();
	debugData->Zero();
	traceData->Zero();
}
