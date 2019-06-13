#include "Structures.hpp"

nDetTraceStructure::nDetTraceStructure(const std::string &name_/*=""*/){
	name = name_;
	mult = 0;
}

void nDetTraceStructure::Zero(){
	left.clear();
	right.clear();
	mult = 0;
}

void nDetTraceStructure::Append(unsigned short *arrL_, unsigned short *arrR_, const size_t &size_){
	left.reserve(left.size()+size_);
	right.reserve(right.size()+size_);
	for(size_t i = 0; i < size_; i++){
		left.push_back(arrL_[i]);
		right.push_back(arrR_[i]);
	}
	mult++;
}

///////////////////////////////////////////////////////////
// nDetEventStructure
///////////////////////////////////////////////////////////

nDetEventStructure::nDetEventStructure(){
	eventID = 0;
	threadID = 0;
	runNb = 0;
	nScatters = 0;
	nDepEnergy = 0;
	nInitEnergy = 0;
	nAbsorbed = 0;
	goodEvent = 0;
}

void nDetEventStructure::SetValues(const int &eventID_, const short &threadID_, const short &runNb_, const short &nScatters_, const double &nDepEnergy_, const double &nInitEnergy_, const bool &nAbsorbed_, const bool &goodEvent_){
	eventID = eventID_;
	threadID = threadID_;
	runNb = runNb_;
	nScatters = nScatters_;
	nDepEnergy = nDepEnergy_;
	nInitEnergy = nInitEnergy_;
	nAbsorbed = nAbsorbed_;
	goodEvent = goodEvent_;
}

void nDetEventStructure::Append(){
}

void nDetEventStructure::Zero(){
	eventID = 0;
	threadID = 0;
	runNb = 0;
	nScatters = 0;
	nDepEnergy = 0;
	nInitEnergy = 0;
	nAbsorbed = 0;
	goodEvent = 0;
}


///////////////////////////////////////////////////////////
// nDetOutputStructure
///////////////////////////////////////////////////////////

nDetOutputStructure::nDetOutputStructure(){
	nPhotonsTot = 0;
	nPhotonsDet = 0;
	lightBalance = 0;
	photonDetEff = 0;
	barTOF = 0;
	barQDC = 0;
	barMaxADC = 0;
	photonComX = 0;
	photonComY = 0;
	reconComX = 0;
	reconComY = 0;
	photonComCol = 0;
	photonComRow = 0;
}

void nDetOutputStructure::SetValues(const unsigned int &nPhotonsTot_, const unsigned int &nPhotonsDet_, const double &lightBalance_, const double &photonDetEff_, const float &barTOF_, const float &barQDC_, const float &barMaxADC_, const double &photonComX_, const double &photonComY_, const double &reconComX_, const double &reconComY_, const short &photonComCol_, const short &photonComRow_){
	nPhotonsTot = nPhotonsTot_;
	nPhotonsDet = nPhotonsDet_;
	lightBalance = lightBalance_;
	photonDetEff = photonDetEff_;
	barTOF = barTOF_;
	barQDC = barQDC_;
	barMaxADC = barMaxADC_;
	photonComX = photonComX_;
	photonComY = photonComY_;
	reconComX = reconComX_;
	reconComY = reconComY_;
	photonComCol = photonComCol_;
	photonComRow = photonComRow_;
}

void nDetOutputStructure::Append(){
}

void nDetOutputStructure::Zero(){
	nPhotonsTot = 0;
	nPhotonsDet = 0;
	lightBalance = 0;
	photonDetEff = 0;
	barTOF = 0;
	barQDC = 0;
	barMaxADC = 0;
	photonComX = 0;
	photonComY = 0;
	reconComX = 0;
	reconComY = 0;
	photonComCol = 0;
	photonComRow = 0;
}


///////////////////////////////////////////////////////////
// nDetMultiOutputStructure
///////////////////////////////////////////////////////////

nDetMultiOutputStructure::nDetMultiOutputStructure(){
	multiplicity = 0;
}

void nDetMultiOutputStructure::SetValues(){
}

void nDetMultiOutputStructure::Append(const unsigned int &nPhotonsTot_, const unsigned int &nPhotonsDet_, const double &lightBalance_, const double &photonDetEff_, const float &barTOF_, const float &barQDC_, const float &barMaxADC_, const double &photonComX_, const double &photonComY_, const double &reconComX_, const double &reconComY_, const short &photonComCol_, const short &photonComRow_, const short &detID_){
	nPhotonsTot.push_back(nPhotonsTot_);
	nPhotonsDet.push_back(nPhotonsDet_);
	lightBalance.push_back(lightBalance_);
	photonDetEff.push_back(photonDetEff_);
	barTOF.push_back(barTOF_);
	barQDC.push_back(barQDC_);
	barMaxADC.push_back(barMaxADC_);
	photonComX.push_back(photonComX_);
	photonComY.push_back(photonComY_);
	reconComX.push_back(reconComX_);
	reconComY.push_back(reconComY_);
	photonComCol.push_back(photonComCol_);
	photonComRow.push_back(photonComRow_);
	detID.push_back(detID_);
	multiplicity++;
}

void nDetMultiOutputStructure::Append(const nDetOutputStructure &output, const short &detID_){
	nPhotonsTot.push_back(output.nPhotonsTot);
	nPhotonsDet.push_back(output.nPhotonsDet);
	lightBalance.push_back(output.lightBalance);
	photonDetEff.push_back(output.photonDetEff);
	barTOF.push_back(output.barTOF);
	barQDC.push_back(output.barQDC);
	barMaxADC.push_back(output.barMaxADC);
	photonComX.push_back(output.photonComX);
	photonComY.push_back(output.photonComY);
	reconComX.push_back(output.reconComX);
	reconComY.push_back(output.reconComY);
	photonComCol.push_back(output.photonComCol);
	photonComRow.push_back(output.photonComRow);
	detID.push_back(detID_);
	multiplicity++;
}

void nDetMultiOutputStructure::Zero(){
	if(multiplicity == 0){ return; } // Structure is already empty
	nPhotonsTot.clear();
	nPhotonsDet.clear();
	lightBalance.clear();
	photonDetEff.clear();
	barTOF.clear();
	barQDC.clear();
	barMaxADC.clear();
	photonComX.clear();
	photonComY.clear();
	reconComX.clear();
	reconComY.clear();
	photonComCol.clear();
	photonComRow.clear();
	detID.clear();
	multiplicity = 0;
}


///////////////////////////////////////////////////////////
// nDetDebugStructure
///////////////////////////////////////////////////////////

nDetDebugStructure::nDetDebugStructure(){
	nEnterPosX = 0;
	nEnterPosY = 0;
	nEnterPosZ = 0;
	nExitPosX = 0;
	nExitPosY = 0;
	nExitPosZ = 0;
	nComX = 0;
	nComY = 0;
	nComZ = 0;
	neutronWeight = 0;
	nFirstScatterTime = 0;
	nFirstScatterLen = 0;
	nEnterTime = 0;
	nTimeInMat = 0;
	mult = 0;
}

void nDetDebugStructure::SetValues(const double &nEnterPosX_, const double &nEnterPosY_, const double &nEnterPosZ_, const double &nExitPosX_, const double &nExitPosY_, const double &nExitPosZ_, const double &nComX_, const double &nComY_, const double &nComZ_, const double &neutronWeight_, const double &nFirstScatterTime_, const double &nFirstScatterLen_, const double &nEnterTime_, const double &nTimeInMat_){
	nEnterPosX = nEnterPosX_;
	nEnterPosY = nEnterPosY_;
	nEnterPosZ = nEnterPosZ_;
	nExitPosX = nExitPosX_;
	nExitPosY = nExitPosY_;
	nExitPosZ = nExitPosZ_;
	nComX = nComX_;
	nComY = nComY_;
	nComZ = nComZ_;
	neutronWeight = neutronWeight_;
	nFirstScatterTime = nFirstScatterTime_;
	nFirstScatterLen = nFirstScatterLen_;
	nEnterTime = nEnterTime_;
	nTimeInMat = nTimeInMat_;
}

void nDetDebugStructure::Append(const double &nScatterX_, const double &nScatterY_, const double &nScatterZ_, const double &nScatterAngle_, const double &nPathLength_, const double &nScatterTime_, const double &impartedE_, const short &segmentCol_, const short &segmentRow_, const short &photonsProd_, const short &recoilMass_, const bool &nScatterScint_){
	nScatterX.push_back(nScatterX_);
	nScatterY.push_back(nScatterY_);
	nScatterZ.push_back(nScatterZ_);
	nScatterAngle.push_back(nScatterAngle_);
	nPathLength.push_back(nPathLength_);
	nScatterTime.push_back(nScatterTime_);
	impartedE.push_back(impartedE_);
	segmentCol.push_back(segmentCol_);
	segmentRow.push_back(segmentRow_);
	photonsProd.push_back(photonsProd_);
	recoilMass.push_back(recoilMass_);
	nScatterScint.push_back(nScatterScint_);
	mult++;
}

void nDetDebugStructure::Zero(){
	if(mult == 0){ return; } // Structure is already empty
	nEnterPosX = 0;
	nEnterPosY = 0;
	nEnterPosZ = 0;
	nExitPosX = 0;
	nExitPosY = 0;
	nExitPosZ = 0;
	nComX = 0;
	nComY = 0;
	nComZ = 0;
	neutronWeight = 0;
	nFirstScatterTime = 0;
	nFirstScatterLen = 0;
	nEnterTime = 0;
	nTimeInMat = 0;
	nScatterX.clear();
	nScatterY.clear();
	nScatterZ.clear();
	nScatterAngle.clear();
	nPathLength.clear();
	nScatterTime.clear();
	impartedE.clear();
	segmentCol.clear();
	segmentRow.clear();
	photonsProd.clear();
	recoilMass.clear();
	nScatterScint.clear();
	mult = 0;
}

