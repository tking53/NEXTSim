//
// $Id: nDetRunAction.cc,v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "time.h"

#include "G4Timer.hh"
#include "G4Run.hh"

#include "nDetRunAction.hh"
#include "nDetEventAction.hh"
#include "nDetStackingAction.hh"
#include "nDetTrackingAction.hh"
#include "nDetSteppingAction.hh"
#include "nDetConstruction.hh"
#include "ParticleSource.hh"
#include "ParticleSourceMessenger.hh"
#include "termColors.hh"

#include "nDetConstructionMessenger.hh"
#include "nDetRunActionMessenger.hh"
#include "nDetMasterOutputFile.hh"

const double KINETIC_ENERGY_THRESHOLD = 0.001; // MeV

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Returns the dot product of two vectors i.e. v1 * v2
double dotProduct(const G4ThreeVector &v1, const G4ThreeVector &v2){
	return (v1.getX()*v2.getX() + v1.getY()*v2.getY() + v1.getZ()*v2.getZ());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

primaryTrackInfo::primaryTrackInfo(const G4Step *step){
	this->setValues(step->GetTrack());
	if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName().find("Scint") != std::string::npos) // Scatter event occured inside a scintillator.
		inScint = true;
	dkE = -1*step->GetDeltaEnergy();
}

primaryTrackInfo::primaryTrackInfo(const G4Track *track){
	this->setValues(track);
}

bool primaryTrackInfo::compare(const G4Track *rhs){
	return (this->pos == rhs->GetPosition());
}

double primaryTrackInfo::getAngle(const G4Track *rhs){
	G4ThreeVector dir2 = rhs->GetMomentumDirection();
	if(this->pos == rhs->GetPosition())
		angle = getAngle(dir2);
	else
		angle = -1;
	return angle;
}

double primaryTrackInfo::getAngle(const G4ThreeVector &dir2){
	angle = std::acos(dotProduct(dir, dir2)/(dir.mag()*dir2.mag())) * 180/3.14159;
	return angle;
}

double primaryTrackInfo::getPathLength(const G4ThreeVector &rhs){
	return (plength = (pos-rhs).mag());
}

void primaryTrackInfo::print(){
	std::cout << "name=" << part->GetParticleName() << ", kE=" << kE << ", dkE=" << dkE << ", pos=(" << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << "), dir=(" << dir.getX() << ", " << dir.getY() << ", " << dir.getZ() << ")\n";
}

void primaryTrackInfo::setValues(const G4Track *track){
	pos = track->GetPosition();
	dir = track->GetMomentumDirection();
	kE = track->GetKineticEnergy();
	dkE = -1;
	gtime = track->GetGlobalTime();
	plength = track->GetTrackLength();
	part = track->GetParticleDefinition();
	
	copyNum = track->GetTouchable()->GetCopyNumber();
	trackID = track->GetTrackID();
	atomicMass = part->GetAtomicMass();
	inScint = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetRunAction::nDetRunAction(){
	timer = new G4Timer;
	
	outputDebug = false;
	verbose = false;
	printTrace = false;
	
	eventAction = NULL;
	stacking = NULL;
	tracking = NULL;
	stepping = NULL;
	
	// Setup the particle source.
	source = new ParticleSource(this);

	baselineFraction = 0;
	baselineJitterFraction = 0;
	polyCfdFraction = 0.5;

	pulseIntegralLow = 5;
	pulseIntegralHigh = 10;
	
	numPhotonsTotal = 0;
	numPhotonsDetTotal = 0;
  
	// Create a messenger for this class
	fActionMessenger = new nDetRunActionMessenger(this); 
	
	// Get a pointer to the detector.
	detector = &nDetConstruction::getInstance(); // The detector builder is a singleton class.
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetRunAction::~nDetRunAction(){
	delete timer;
	delete fActionMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetRunAction::BeginOfRunAction(const G4Run* aRun)
{
	// Update the debug output flag (may not be thread safe CRT)
	outputDebug = nDetMasterOutputFile::getInstance().getOutputDebug();

	numPhotonsTotal = 0;
	numPhotonsDetTotal = 0;

	//if(!IsMaster()) return; // Master thread only. THIS DOESN'T WORK CRT
	if(G4Threading::G4GetThreadId() >= 0) return; // Master thread only.
	
	G4cout << "nDetRunAction::BeginOfRunAction()->"<< G4endl;
	G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl; 
	timer->Start();

	// Open a root file.
	nDetMasterOutputFile::getInstance().openRootFile(aRun); // The master output file is a singleton class.

	// Set the total number of events
	nDetMasterOutputFile::getInstance().setTotalEvents(aRun->GetNumberOfEventToBeProcessed());

	// Get RunId and threadID
	data.runNb = aRun->GetRunID();
	data.threadID = G4Threading::G4GetThreadId();
	
	// Update the source.
	source->SetDetector(detector);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetRunAction::EndOfRunAction(const G4Run* aRun)
{
	if(!IsMaster()) return; // Master thread only.
	
	timer->Stop();
	G4cout << "number of event = " << aRun->GetNumberOfEvent() << " " << *timer << G4endl;
}

void nDetRunAction::process(){
	while(this->scatterEvent()){
	}
	
	if(counter){ 
		for(short i = data.nScatters+1; i >= 2; i--) // The first recoil particle ID is equal to 2
			data.Nphotons.push_back(counter->getPhotonCount(i));
	}

	// Get the number of detected photons
	data.nPhotonsDet[0] = cmL.getNumDetected();
	data.nPhotonsDet[1] = cmR.getNumDetected();
	data.nPhotonsDetTot = data.nPhotonsDet[0]+data.nPhotonsDet[1];

	// Compute the photon detection efficiency
	data.nPhotonsTot = stacking->GetNumPhotonsProduced();
	if(data.nPhotonsTot > 0)
		data.photonDetEfficiency = data.nPhotonsDetTot/(double)data.nPhotonsTot;
	else
		data.photonDetEfficiency = -1;

	// Check for valid bar detection
	if(data.nPhotonsDet[0] > 0 && data.nPhotonsDet[1] > 0)
		data.goodEvent = true;

	// Get the photon center-of-mass positions
	G4ThreeVector centerL = cmL.getCenter();
	G4ThreeVector centerR = cmR.getCenter();
	data.photonDetCenterOfMassX[0] = centerL.getX(); data.photonDetCenterOfMassX[1] = centerR.getX(); 
	data.photonDetCenterOfMassY[0] = centerL.getY(); data.photonDetCenterOfMassY[1] = centerR.getY(); 
	data.photonDetCenterOfMassZ[0] = centerL.getZ(); data.photonDetCenterOfMassZ[1] = centerR.getZ(); 

	// Get photon arrival times at the PMTs
	data.photonMinArrivalTime[0] = cmL.getMinArrivalTime();
	data.photonAvgArrivalTime[0] = cmL.getAvgArrivalTime();

	data.photonMinArrivalTime[1] = cmR.getMinArrivalTime();
	data.photonAvgArrivalTime[1] = cmR.getAvgArrivalTime();

	pmtResponse *pmtL = cmL.getPmtResponse();
	pmtResponse *pmtR = cmR.getPmtResponse();

	// "Digitize" the light pulses.
	pmtL->digitize(baselineFraction, baselineJitterFraction);
	pmtR->digitize(baselineFraction, baselineJitterFraction);

	// Check for saturated pulse.
	if(pmtL->getPulseIsSaturated() || pmtR->getPulseIsSaturated()){
		Display::WarningPrint("One or more PMT's traces have saturated! Recommend lowering the gain.", "nDetRunAction");
	}

	// Copy the trace into the trace vector.
	/*if(outputTraces){
		pmtL->copyTrace(data.lightPulseL);
		pmtR->copyTrace(data.lightPulseR);
	}*/

	double targetTimeOffset = source->GetTargetTimeOffset();

	// Do some light pulse analysis
	data.pulsePhase[0] = pmtL->analyzePolyCFD(polyCfdFraction) + targetTimeOffset;
	data.pulseQDC[0] = pmtL->integratePulseFromMaximum(pulseIntegralLow, pulseIntegralHigh);
	data.pulseMax[0] = pmtL->getMaximum();
	data.pulseMaxTime[0] = pmtL->getMaximumTime();
	data.pulseWeightedArrival[0] = pmtL->getWeightedPhotonArrivalTime();

	data.pulsePhase[1] = pmtR->analyzePolyCFD(polyCfdFraction) + targetTimeOffset;
	data.pulseQDC[1] = pmtR->integratePulseFromMaximum(pulseIntegralLow, pulseIntegralHigh);
	data.pulseMax[1] = pmtR->getMaximum();
	data.pulseMaxTime[1] = pmtR->getMaximumTime();
	data.pulseWeightedArrival[1] = pmtR->getWeightedPhotonArrivalTime();

	// Print the digitized traces.
	if(printTrace){
		size_t traceLength = pmtL->getPulseLength();
		unsigned short *traceL = pmtL->getDigitizedPulse();
		unsigned short *traceR = pmtR->getDigitizedPulse();
		std::cout << "***********************************************************\n";
		std::cout << "* PhotonsTot	 : " << data.nPhotonsTot << std::endl;
		std::cout << "* PhotonsDet	 : " << data.nPhotonsDet[0] << "\t" << data.nPhotonsDet[1] << std::endl;
		std::cout << "* MaxIndex	   : " << pmtL->getMaximumIndex() << "\t" << pmtR->getMaximumIndex() << std::endl;
		std::cout << "* Baseline	   : " << pmtL->getBaseline() << "\t" << pmtR->getBaseline() << std::endl;	
		std::cout << "* Maximum		: " << pmtL->getMaximum() << "\t" << pmtR->getMaximum() << std::endl;
		std::cout << "* MaxTime		: " << pmtL->getMaximumTime() << "\t" << pmtR->getMaximumTime() << std::endl;
		std::cout << "* WeightedArrival: " << pmtL->getWeightedPhotonArrivalTime() << "\t" << pmtR->getWeightedPhotonArrivalTime() << std::endl;
		std::cout << "* MinimumArrival : " << pmtL->getMinimumPhotonArrivalTime() << "\t" << pmtR->getMinimumPhotonArrivalTime() << std::endl;
		std::cout << "***********************************************************\n";

		int adcClockTick = pmtL->getAdcClockTick();
		for(size_t i = 0; i < traceLength; i++){
			std::cout << i*adcClockTick << "\t" << traceL[i] << "\t" << traceR[i] << std::endl;
		}
	}

	// Get the digitizer response of the anodes.
	pmtResponse *anodeResponseL = cmL.getAnodeResponse();
	pmtResponse *anodeResponseR = cmR.getAnodeResponse();

	// Digitize anode waveforms and integrate.
	float anodeQDC[2][4];
	for(size_t i = 0; i < 4; i++){
		anodeResponseL[i].digitize(baselineFraction, baselineJitterFraction);
		anodeResponseR[i].digitize(baselineFraction, baselineJitterFraction);
		data.anodeQDC[0][i] = anodeResponseL[i].integratePulseFromMaximum(pulseIntegralLow, pulseIntegralHigh);
		data.anodeQDC[1][i] = anodeResponseR[i].integratePulseFromMaximum(pulseIntegralLow, pulseIntegralHigh);
	}

	// Compute the anode positions.
	for(size_t i = 0; i < 2; i++){
		data.reconstructedCenterX[i] = -((anodeQDC[i][0]+anodeQDC[i][1])-(anodeQDC[i][2]+anodeQDC[i][3]))/(anodeQDC[i][0]+anodeQDC[i][1]+anodeQDC[i][2]+anodeQDC[i][3]);
		data.reconstructedCenterY[i] = ((anodeQDC[i][1]+anodeQDC[i][2])-(anodeQDC[i][3]+anodeQDC[i][0]))/(anodeQDC[i][0]+anodeQDC[i][1]+anodeQDC[i][2]+anodeQDC[i][3]);
	}

	if(outputDebug){
		// Perform CFD on digitized anode waveforms.
		for(size_t i = 0; i < 4; i++){
			data.anodePhase[0][i] = anodeResponseL[i].analyzePolyCFD(polyCfdFraction) + targetTimeOffset; // left
			data.anodePhase[1][i] = anodeResponseR[i].analyzePolyCFD(polyCfdFraction) + targetTimeOffset; // right
		}

		// Compute the bar speed-of-light.
		double barTimeDiff = data.pulsePhase[1] - data.pulsePhase[0];
		data.detSpeedLight = 2*data.neutronCenterOfMass[2]/barTimeDiff;

		G4ThreeVector nCenterMass(data.neutronCenterOfMass[0], data.neutronCenterOfMass[1], data.neutronCenterOfMass[2]);
		G4ThreeVector nIncidentPos(data.neutronIncidentPositionX, data.neutronIncidentPositionY, data.neutronIncidentPositionZ);
		G4ThreeVector nExitPos(data.neutronExitPositionX, data.neutronExitPositionY, data.neutronExitPositionZ);

		// Compute the neutron scatter center-of-mass.
		nCenterMass = (1/data.neutronWeight)*nCenterMass;
		
		// Convert the neutron incident/exit positions to the frame of the detector.
		nIncidentPos = nIncidentPos;
		nExitPos = nExitPos;

		// Now in the rotated frame of the detector.
		data.neutronCenterOfMass[0] = nCenterMass.getX();
		data.neutronCenterOfMass[1] = nCenterMass.getY();
		data.neutronCenterOfMass[2] = nCenterMass.getZ();
		data.neutronIncidentPositionX = nIncidentPos.getX();
		data.neutronIncidentPositionY = nIncidentPos.getY();
		data.neutronIncidentPositionZ = nIncidentPos.getZ();
		data.neutronExitPositionX = nExitPos.getX();
		data.neutronExitPositionY = nExitPos.getY();
		data.neutronExitPositionZ = nExitPos.getZ();
	}

	// Compute the light balance (Z).
	data.photonLightBalance = (data.pulseQDC[0]-data.pulseQDC[1])/(data.pulseQDC[0]+data.pulseQDC[1]);

	// Compute "bar" variables.
	data.barCenterOfMassX = (data.photonDetCenterOfMassX[0]+data.photonDetCenterOfMassX[1])/2;
	data.barCenterOfMassY = (data.photonDetCenterOfMassY[0]+data.photonDetCenterOfMassY[1])/2;
	data.barTOF = (data.pulsePhase[0]+data.pulsePhase[1])/2;
	data.barQDC = std::sqrt(data.pulseQDC[0]*data.pulseQDC[1]);
	data.barMaxADC = std::sqrt(data.pulseMax[0]*data.pulseMax[1]);

	// Get the segment of the detector where the photon CoM occurs.
	cmL.getCenterSegment(data.centerOfMassColumn[0], data.centerOfMassRow[0]);
	cmR.getCenterSegment(data.centerOfMassColumn[1], data.centerOfMassRow[1]);
	
	// Write the data (mutex protected, thread safe).
	nDetMasterOutputFile::getInstance().fillBranch(data); // The master output file is a singleton class.

	// Update photon statistics.
	numPhotonsTotal += data.nPhotonsTot;
	numPhotonsDetTotal += data.nPhotonsDet[0]+data.nPhotonsDet[1];
	
	// Clear all statistics.
	cmL.clear();
	cmR.clear();
	data.clear();
	
	if(stacking) stacking->Reset();
	if(tracking) tracking->Reset();
	if(stepping) stepping->Reset();
}

void nDetRunAction::setActions(nDetEventAction *event_, nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_){
	eventAction = event_;
	stacking = stacking_;
	tracking = tracking_;
	stepping = stepping_;

	// Get the photon counter
	counter = stacking->GetCounter();
}

bool nDetRunAction::AddDetectedPhoton(const G4Step *step, const double &mass/*=1*/){
	if(!step->GetPostStepPoint()){
		Display::WarningPrint("INVALID POST POINT!", "nDetRunAction");
		return false;
	}
	
	double energy = step->GetTrack()->GetTotalEnergy();
	double time = step->GetPostStepPoint()->GetGlobalTime();
	G4ThreeVector position = step->GetPostStepPoint()->GetPosition() - detector->GetDetectorPos();
	position = (*detector->GetDetectorRot())*position;
	
	if(position.z() > 0){
		if(cmL.addPoint(energy, time, position, mass))
			return true;
	}
	if(position.z() < 0){
		if(cmR.addPoint(energy, time, position, mass))
			return true;
	}
	return false;
}


bool nDetRunAction::scatterEvent(){
	if(primaryTracks.size() <= 1)
		return false;

	// Get a primary particle step off the stack.
	primaryTrackInfo *priTrack = &primaryTracks.back();

	if(data.nScatters++ == 0){ // First scatter event (CAREFUL! These are in reverse order).
		if(primaryTracks.size() < 2){
			if(verbose){
				Display::WarningPrint("Expected at least two events in primaryTracks.", "nDetRunAction");
				priTrack->print();
			}
			return false;
		}
		if(outputDebug){
			G4ThreeVector firstScatter = primaryTracks.at(0).pos + primaryTracks.at(1).pos;
			data.nTimeToFirstScatter = primaryTracks.at(1).gtime;
			data.nLengthToFirstScatter = firstScatter.mag();
		}
	}
	
	// Add the deposited energy to the total.
	data.depEnergy += priTrack->dkE;

	G4ThreeVector vertex = priTrack->pos;

	if(verbose){
		std::cout << " id=" << priTrack->trackID << " "; priTrack->print();	
		std::cout << "	pos=(" << vertex.getX() << ", " << vertex.getY() << ", " << vertex.getZ() << ")\n";
		std::cout << "	dE=" << priTrack->dkE << ", dE2=" << priTrack->kE << ", size=" << primaryTracks.size() << std::endl;
	}

	if(outputDebug){
		data.nScatterX.push_back(vertex.getX());
		data.nScatterY.push_back(vertex.getY());
		data.nScatterZ.push_back(vertex.getZ());

		data.neutronCenterOfMass[0] += priTrack->dkE*vertex.getX();
		data.neutronCenterOfMass[1] += priTrack->dkE*vertex.getY();
		data.neutronCenterOfMass[2] += priTrack->dkE*vertex.getZ();
		data.neutronWeight += priTrack->dkE;

		data.nScatterAngle.push_back(priTrack->angle);
		data.impartedE.push_back(priTrack->dkE);

		data.nPathLength.push_back(priTrack->plength);
		data.scatterTime.push_back(priTrack->gtime - data.incidentTime);
		data.recoilMass.push_back(priTrack->atomicMass); 
		data.nScatterScint.push_back(priTrack->inScint);

		G4int segCol, segRow;
		detector->GetSegmentFromCopyNum(priTrack->copyNum, segCol, segRow);
		data.segmentCol.push_back(segCol);
		data.segmentRow.push_back(segRow);
	}
	
	primaryTracks.pop_back();
	
	return true;
}

void nDetRunAction::initializeNeutron(const G4Step *step){
	data.initEnergy = step->GetPreStepPoint()->GetKineticEnergy();
	if(outputDebug){
		data.incidentTime = step->GetTrack()->GetGlobalTime();
		data.neutronIncidentPositionX = step->GetPostStepPoint()->GetPosition().getX();
		data.neutronIncidentPositionY = step->GetPostStepPoint()->GetPosition().getY();
		data.neutronIncidentPositionZ = step->GetPostStepPoint()->GetPosition().getZ();
		data.timeInMaterial = 0;
		data.neutronExitPositionX = 0;
		data.neutronExitPositionY = 0;
		data.neutronExitPositionZ = 0;
		primaryTracks.clear();
		primaryTracks.push_back(step);
		prevDirection = primaryTracks.back().dir;
		prevPosition = primaryTracks.back().pos;
	}
	if(verbose){ 
		std::cout << "IN: "; primaryTracks.back().print();
	}
}

void nDetRunAction::scatterNeutron(const G4Step *step){
	double dE = -1*step->GetDeltaEnergy();
	G4Track *track = step->GetTrack();
	if(dE > 0){
		primaryTracks.push_back(step);
		if(outputDebug){
			primaryTracks.back().getAngle(prevDirection);
			primaryTracks.back().getPathLength(prevPosition);
			prevDirection = primaryTracks.back().dir;
			prevPosition = primaryTracks.back().pos;
		}
		if(verbose){
			std::cout << "SC: "; primaryTracks.back().print();
		}
		if(track->GetKineticEnergy() < KINETIC_ENERGY_THRESHOLD){ // Check particle kinematic energy.
			data.nAbsorbed = true;
			track->SetTrackStatus(fStopAndKill); // Kill the neutron.
			if(verbose) 
				std::cout << "OT: final kE=0 (absorbed)" << std::endl;
		}
	}
}

void nDetRunAction::finalizeNeutron(const G4Step *step){
	data.nAbsorbed = false;
	G4Track *track = step->GetTrack();
	if(outputDebug){
		data.timeInMaterial = track->GetGlobalTime() - data.incidentTime;
		data.neutronExitPositionX = step->GetPreStepPoint()->GetPosition().getX();
		data.neutronExitPositionY = step->GetPreStepPoint()->GetPosition().getY();
		data.neutronExitPositionZ = step->GetPreStepPoint()->GetPosition().getZ();
	}
	if(verbose) 
		std::cout << "OT: final kE=" << track->GetKineticEnergy() << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
