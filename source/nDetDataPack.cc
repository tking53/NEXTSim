#include "nDetDataPack.hh"

void nDetDataPack::clear(){
	multiplicity = 0;
	nScatters = 0;
	nPhotonsTot = 0;
	nPhotonsDet[0] = 0;
	nPhotonsDet[1] = 0;
	nTimeToFirstScatter = 0;
	nLengthToFirstScatter = 0;
	incidentTime = 0;
	depEnergy = 0;
	nAbsorbed = false;
	goodEvent = false;

	neutronCenterOfMass[0] = 0;
	neutronCenterOfMass[1] = 0;
	neutronCenterOfMass[2] = 0;
	neutronWeight = 0;

	nScatterX.clear();
	nScatterY.clear();
	nScatterZ.clear();
	nScatterAngle.clear();
	nPathLength.clear();
	impartedE.clear();
	scatterTime.clear();
	segmentCol.clear();
	segmentRow.clear();
	Nphotons.clear();
	recoilMass.clear();
	nScatterScint.clear();

	lightPulseL.clear();
	lightPulseR.clear();
}
