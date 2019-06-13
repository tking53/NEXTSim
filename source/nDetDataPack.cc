#include <iostream>

#include "TBranch.h"

#include "nDetDataPack.hh"

void nDetDataPack::setDataAddresses(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug){
	evtData = evt;
	outData = out;
	multData = mult;
	debugData = debug;
}

void nDetDataPack::setBranchAddresses(TBranch *evt_b, TBranch *out_b, TBranch *mult_b, TBranch *debug_b) const {
	if(evt_b) evt_b->SetAddress(evtData);
	if(out_b) out_b->SetAddress(outData);
	if(mult_b) mult_b->SetAddress(multData);
	if(debug_b) debug_b->SetAddress(debugData);
}

void nDetDataPack::copyData(nDetEventStructure *evt, nDetOutputStructure *out, nDetMultiOutputStructure *mult, nDetDebugStructure *debug) const {
	(*evt) = (*evtData);
	(*out) = (*outData);
	(*mult) = (*multData);
	(*debug) = (*debugData);
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
}
