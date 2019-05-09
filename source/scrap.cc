#include "centerOfMass.hh"



centerOfMass *GetCenterOfMassPositiveSide(){ return &center[0]; }
  
centerOfMass *GetCenterOfMassNegativeSide(){ return &center[1]; }

void GetSegmentFromCopyNum(const G4int &copyNum, G4int &col, G4int &row) const ;    
    
bool AddDetectedPhoton(const G4Step *step, const double &mass=1);

void GetDetectedPhotons(size_t &numLeft, size_t &numRight);

void Clear();
    
    
    	pmtResponse *getPmtResponseLeft();

	pmtResponse *getPmtResponseRight();

	pmtResponse *getAnodeResponseLeft();

	pmtResponse *getAnodeResponseRight();
    
    

void nDetConstruction::GetDetectedPhotons(size_t &numLeft, size_t &numRight){
	numLeft = center[0].getNumDetected();
	numRight = center[1].getNumDetected();
}











pmtResponse *nDetRunAction::getPmtResponseLeft(){
	return detector->GetCenterOfMassPositiveSide()->getPmtResponse();
}

pmtResponse *nDetRunAction::getPmtResponseRight(){
	return detector->GetCenterOfMassNegativeSide()->getPmtResponse();
}

pmtResponse *nDetRunAction::getAnodeResponseLeft(){
	return detector->GetCenterOfMassPositiveSide()->getAnodeResponse();
}

pmtResponse *nDetRunAction::getAnodeResponseRight(){
	return detector->GetCenterOfMassNegativeSide()->getAnodeResponse();
}


