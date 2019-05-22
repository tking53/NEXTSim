#ifndef NDET_DATA_PACK_HH
#define NDET_DATA_PACK_HH

#include <vector>

///////////////////////////////////////////////////////////////////////////////
// class nDetDataPack
///////////////////////////////////////////////////////////////////////////////

class nDetDataPack{
  public:
	// Run & event information.
	int eventID; ///< Geant event ID number
	short threadID; ///< Geant thread ID number for multithreading
	short runNb; ///< Geant run number

	// Normal neutron output.
	short nScatters; ///< Number of primary particle scatters

	double depEnergy; ///< Energy deposition inside of the detector (in MeV)
	double initEnergy; ///< Initial energy of the neutron (in MeV)
	
	bool nAbsorbed; ///< Flag indicating whether or not the neutron was captured inside the detector
	bool goodEvent; ///< Flag indicating a good detection event i.e. where both PMTs detect at least one scintillation photon

	// Neutron debug output variables.
	double neutronIncidentPositionX; ///< X-axis position where the primary particle enters the detector
	double neutronIncidentPositionY; ///< Y-axis position where the primary particle enters the detector
	double neutronIncidentPositionZ; ///< Z-axis position where the primary particle enters the detector
	double neutronExitPositionX; ///< X-axis position where the primary particle exits the detector
	double neutronExitPositionY; ///< Y-axis position where the primary particle exits the detector
	double neutronExitPositionZ; ///< Z-axis position where the primary particle exits the detector

	double nTimeToFirstScatter; ///< Global time of the first primary particle scatter event (in ns)
	double nLengthToFirstScatter; ///< Flight path to the first primary particle scatter event (n mm)
	double incidentTime; ///< Global time when the primary particle enters the detector (in ns)
	double timeInMaterial; ///< Total time the primary particle spends in the material (in ns)
	
	std::vector<double> nScatterX; ///< Vector of X-axis position of all primary particle scatter positions (in mm)
	std::vector<double> nScatterY; ///< Vector of Y-axis position of all primary particle scatter positions (in mm)
	std::vector<double> nScatterZ; ///< Vector of Z-axis position of all primary particle scatter positions (in mm)
	std::vector<double> nScatterAngle; ///< Angle between successive scatters of the primary particle (in degrees)
	std::vector<double> nPathLength; ///< Path length of primary particle between successive scatter events (in mm)
	std::vector<double> impartedE; ///< Energy imparted by the primary particle for each scatter event (in MeV)
	std::vector<double> scatterTime; ///< Global time of each primary particle scatter event (in ns)
	std::vector<short> segmentCol; ///< Scintillator segment column for each primary particle scatter event (for modular detectors)
	std::vector<short> segmentRow; ///< Scintillator segment row for each primary particle scatter event (for modular detectors)
	std::vector<short> Nphotons; ///< Number of scintillation photons produced for each primary particle scatter event
	std::vector<short> recoilMass; ///< Mass of the recoil particle for each primary particle scatter event
	std::vector<bool> nScatterScint; ///< Flag indicating whether or not the scatter event occured in a scintillator material

	// Normal photon output.
	double photonDetEfficiency; ///< Ratio of optical photons detected by a PMT to the total number of photons generated
	double photonLightBalance; ///< Ratio of the difference of left and right TQDC to the sum of left and right TQDC
	double barCenterOfMassX; ///< Average of the left and right photon center-of-mass X position (in mm)
	double barCenterOfMassY; ///< Average of the left and right photon center-of-mass Y position (in mm)

	float barTOF; ///< Average of the left and right dynode light pulse phases computed using PolyCFD (in ns)
	float barQDC; ///< Average of the left and right dynode light pulse integrals
	float barMaxADC; ///< Average of the left and right dynode light pulse maxima (in ADC channels)
	float pulsePhase[2]; ///< Phases of the left and right dynode light pulses computed using PolyCFD (in ns)
	float anodePhase[2][4]; ///< Phases of the four anger logic anode readout pulses for the left and right PMT computed using PolyCFD (in ns)

	double photonDetCenterOfMassX[2]; ///< Left and right PMT photon center-of-mass along the X-axis weighted by the anode gain and quantum efficiency (in mm)
	double photonDetCenterOfMassY[2]; ///< Left and right PMT photon center-of-mass along the Y-axis weighted by the anode gain and quantum efficiency (in mm)
	double photonDetCenterOfMassZ[2]; ///< Left and right PMT photon center-of-mass along the Z-axis weighted by the anode gain and quantum efficiency (in mm)
	double reconstructedCenterX[2]; ///< Left and right PMT photon center-of-mass along the X-axis computed using Anger Logic reconstruction
	double reconstructedCenterY[2]; ///< Left and right PMT photon center-of-mass along the Y-axis computed using Anger Logic reconstruction

	short centerOfMassColumn[2]; ///< Segmented PMT anode column corresponding to the photon center-of-mass for the left and right PMT
	short centerOfMassRow[2]; ///< Segmented PMT anode row corresponding to the photon center-of-mass for the left and right PMT
	
	// Photon debug output variables.
	unsigned int nPhotonsTot; ///< Total number of scintillation photons produced
	unsigned int nPhotonsDetTot; ///< Total number of optical photons detected by both PMTs
	unsigned int nPhotonsDet[2]; ///< Number of optical photons produced by each PMT

	double photonMinArrivalTime[2]; ///< Minimum optical photon arrival time at each PMT (in ns)
	double photonAvgArrivalTime[2]; ///< Average optical photon arrival time at each PMT (in ns)
	double pulseWeightedArrival[2]; ///< Average optical photon arrival time at each PMT weighted by the PMT anode gain and quantum efficiency (in ns)

	float pulseMaxTime[2]; ///< Time of arrival of the maximum of the left and right light pulses (in ns)
	float pulseMax[2]; ///< Maximum of the left and right light pulses (in ADC channels)
	float pulseQDC[2]; ///< Integral of the left and right light pulses
	float anodeQDC[2][4]; ///< Anger logic currents for the four readouts of the left and right PSPmts
	
	double neutronCenterOfMass[3]; ///< X, Y, and Z, center-of-mass positions for all neutron scatters weighted by the energy imparted to the detector (in mm)
	double neutronWeight; ///< Sum of all energy imparted to the detector by all neutron scatters (in MeV)
	double detSpeedLight; ///< Detector speed of light (USED FOR DEBUGGING)

	std::vector<unsigned short> lightPulseL; ///< Copy of the psuedo-digitized left PMT light response pulse
	std::vector<unsigned short> lightPulseR; ///< Copy of the psuedo-digitized right PMT light response pulse

	void clear();
};

#endif
