#ifndef NDET_DATA_PACK_HH
#define NDET_DATA_PACK_HH

#include <vector>

class nDetDataPack{
  public:
	// Run / event information.
	short threadID;
	short runNb;

	// Normal neutron output.
	short nScatters;

	double depEnergy; // energy deposition inside of the EJ200 scintillator
	double initEnergy; // Initial energy of the neutron (CRT)
	
	bool nAbsorbed;
	bool goodEvent;

	// Neutron debug output variables.
	double neutronIncidentPositionX;
	double neutronIncidentPositionY;
	double neutronIncidentPositionZ;
	double neutronExitPositionX;
	double neutronExitPositionY;
	double neutronExitPositionZ;	

	double nTimeToFirstScatter;
	double nLengthToFirstScatter;
	double incidentTime;
	double timeInMaterial;
	
	std::vector<double> nScatterX;
	std::vector<double> nScatterY;
	std::vector<double> nScatterZ;
	std::vector<double> nScatterAngle;
	std::vector<double> nPathLength;
	std::vector<double> impartedE;
	std::vector<double> scatterTime;
	std::vector<short> segmentCol;
	std::vector<short> segmentRow;
	std::vector<short> Nphotons;
	std::vector<short> recoilMass;
	std::vector<bool> nScatterScint;

	// Normal photon output.
	double photonDetEfficiency;
	double photonLightBalance;	
	double barCenterOfMassX;
	double barCenterOfMassY;

	float barTOF;
	float barQDC;	
	float barMaxADC;
	float pulsePhase[2];
	float anodePhase[2][4];

	double photonDetCenterOfMassX[2];
	double photonDetCenterOfMassY[2];
	double reconstructedCenterX[2];
	double reconstructedCenterY[2];

	short centerOfMassColumn[2];
	short centerOfMassRow[2];
	
	// Photon debug output variables.
	unsigned int nPhotonsTot;
	unsigned int nPhotonsDetTot;
	unsigned int nPhotonsDet[2];

	double photonDetCenterOfMassZ[2];
	double photonMinArrivalTime[2];
	double photonAvgArrivalTime[2];
	double pulseWeightedArrival[2];

	float pulseMaxTime[2];
	float pulseMax[2];
	float pulseQDC[2];
	float anodeQDC[2][4];
	
	double neutronCenterOfMass[3];
	double neutronWeight;
	double detSpeedLight;

	std::vector<unsigned short> lightPulseL;
	std::vector<unsigned short> lightPulseR;

	void clear();
};

#endif
