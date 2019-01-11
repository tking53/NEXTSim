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

#include "nDetConstructionMessenger.hh"
#include "nDetRunActionMessenger.hh"

const double KINETIC_ENERGY_THRESHOLD = 0.001; // MeV

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Returns the dot product of two vectors i.e. v1 * v2
double dotProduct(const G4ThreeVector &v1, const G4ThreeVector &v2){
	return (v1.getX()*v2.getX() + v1.getY()*v2.getY() + v1.getZ()*v2.getZ());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

primaryTrackInfo::primaryTrackInfo(const G4Step *step){
	G4Track *track = step->GetTrack();
	pos = track->GetPosition();
	dir = track->GetMomentumDirection();
	kE = track->GetKineticEnergy();
	dkE = -1*step->GetDeltaEnergy();
	gtime = track->GetGlobalTime();
	plength = track->GetTrackLength();
	part = track->GetParticleDefinition();
}

primaryTrackInfo::primaryTrackInfo(const G4Track *track){
	pos = track->GetPosition();
	dir = track->GetMomentumDirection();
	kE = track->GetKineticEnergy();
	dkE = -1;
	gtime = track->GetGlobalTime();
	plength = track->GetTrackLength();
	part = track->GetParticleDefinition();
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetRunAction::nDetRunAction(nDetConstruction *det){
	timer = new G4Timer;
	
	persistentMode = false;
	verbose = false;
	
	outputEnabled = true;
	outputTraces = false;
	outputDebug = false;
	outputBadEvents = false;
	
	fFile = NULL;
	eventAction = NULL;
	stacking = NULL;
	tracking = NULL;
	stepping = NULL;
	detector = det;
	
	// Setup the particle source.
	source = new ParticleSource(this, detector);

	baselineFraction = 0;
	baselineJitterFraction = 0;
	polyCfdFraction = 0.5;

	pulseIntegralLow = 5;
	pulseIntegralHigh = 10;
		
	//create a messenger for this class
	fActionMessenger = new nDetRunActionMessenger(this); 

	runTitle = "NExT Geant4 output";
	runIndex = 1;
	
	overwriteExistingFile = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetRunAction::~nDetRunAction()
{
	delete timer;
	delete fActionMessenger;
		
	// Close the root file, if it's still open.
	closeRootFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetRunAction::BeginOfRunAction(const G4Run* aRun)
{
	G4cout << "nDetRunAction::BeginOfRunAction()->"<< G4endl;
	G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl; 
	timer->Start();

	if(outputEnabled){ // Open a root file.
		if(!persistentMode || aRun->GetRunID() == 0){ // Close the file and open a new one.
			if(openRootFile(aRun))
				G4cout << "### File " << fFile->GetName() << " opened." << G4endl;
			else
				G4cout << "### FAILED TO OPEN OUTPUT FILE!\n";
		}
	}

	// Set the total number of events
	eventAction->SetTotalEvents(aRun->GetNumberOfEventToBeProcessed());

	// get RunId
	runNb = aRun->GetRunID();
	
	// Update the source.
	source->SetDetector(detector);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void nDetRunAction::EndOfRunAction(const G4Run* aRun)
{	 
	timer->Stop();
	G4cout << "number of event = " << aRun->GetNumberOfEvent() << " " << *timer << G4endl;
}

pmtResponse *nDetRunAction::getPmtResponseLeft(){
	return detector->GetCenterOfMassPositiveSide()->getPmtResponse();
}

pmtResponse *nDetRunAction::getPmtResponseRight(){
	return detector->GetCenterOfMassNegativeSide()->getPmtResponse();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool nDetRunAction::openRootFile(const G4Run* aRun)
{
	// Close the output file if it is open.
	closeRootFile();
	
	if(filename.empty()){ // Get the system time, and use it to create the filename of root file.
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[180];

		time ( &rawtime );
		timeinfo = localtime ( &rawtime );

		strftime (buffer,180,"_%H:%M:%S_%a_%b_%d_%Y",timeinfo);

		// Create a root file for the current run
		char defaultFilename[300];
		sprintf(defaultFilename, "run_%03d%s.root",aRun->GetRunID(), buffer);
		filename = std::string(defaultFilename);
		
		// Create a ROOT file
		fFile = new TFile(filename.c_str(), "RECREATE", runTitle.c_str());
		if(!fFile->IsOpen()) {
			G4cout << " nDetRunAction: ERROR! Failed to open file \"" << filename << "\"!\n";
			return false;
		}
	}
	else{ // Load new output file.
		while(true){
			std::stringstream stream; stream << runIndex++;
			std::string runID = stream.str();
		
			std::string newFilename = filenamePrefix + "-" + std::string(3-runID.length(), '0') + runID + filenameSuffix;
			if(!overwriteExistingFile){ // Do not overwrite output
				std::ifstream fCheck(newFilename.c_str());
				if(fCheck.good()){ // File exists. Start over.
					if(verbose)
						std::cout << " nDetRunAction: File \"" << newFilename << "\" already exists.\n";
					fCheck.close();
					continue;
				}
				fCheck.close();
				fFile = new TFile(newFilename.c_str(), "CREATE", runTitle.c_str());
			}
			else{ // Overwrite output
				fFile = new TFile(newFilename.c_str(), "RECREATE", runTitle.c_str());
			}
			if(!fFile->IsOpen()) {
				G4cout << " nDetRunAction: ERROR! Failed to open file \"" << newFilename << "\"!\n";
				return false;
			}
			break;
		}
	}

	// Add user commands to the output file.
	TDirectory *dir = fFile->mkdir("setup");
	detector->GetMessenger()->write(dir);
	source->GetMessenger()->write(dir);
	fActionMessenger->write(dir);

	// Create root tree.
	if(treename.empty()) treename = "data"; //"neutronEvent";
	fTree = new TTree(treename.c_str(),"Photons produced by thermal neutrons");

	if(persistentMode) // Run number
		fTree->Branch("runNb", &runNb);
	fTree->Branch("nScatters", &nScatters);
	fTree->Branch("nDepEnergy", &depEnergy);
	fTree->Branch("nInitEnergy", &initEnergy);
	fTree->Branch("nAbsorbed", &nAbsorbed);
	fTree->Branch("nPhotonsTot", &nPhotonsTot);
	fTree->Branch("nPhotonsDet", &nPhotonsDetTot);

	if(outputDebug){ // Output extra neutron scattering information (off by default).
		fTree->Branch("nEnterPosX", &neutronIncidentPositionX);
		fTree->Branch("nEnterPosY", &neutronIncidentPositionY);
		fTree->Branch("nEnterPosZ", &neutronIncidentPositionZ);
		fTree->Branch("nExitPosX", &neutronExitPositionX);
		fTree->Branch("nExitPosY", &neutronExitPositionY);
		fTree->Branch("nExitPosZ", &neutronExitPositionZ);
		fTree->Branch("nComX", &neutronCenterOfMass[0]);
		fTree->Branch("nComY", &neutronCenterOfMass[1]);
		fTree->Branch("nComZ", &neutronCenterOfMass[2]);		
		fTree->Branch("nFirstScatterTime", &nTimeToFirstScatter);
		fTree->Branch("nFirstScatterLen", &nLengthToFirstScatter);		
		fTree->Branch("nEnterTime", &incidentTime);
		fTree->Branch("nTimeInMat", &timeInMaterial);		
		fTree->Branch("nScatterX", &nScatterX);
		fTree->Branch("nScatterY", &nScatterY);
		fTree->Branch("nScatterZ", &nScatterZ);
		fTree->Branch("nScatterAngle", &nScatterAngle);
		fTree->Branch("nPathLength", &nPathLength);
		fTree->Branch("nScatterTime", &scatterTime);
		fTree->Branch("impartedE", &impartedE);
		fTree->Branch("segmentCol", &segmentCol);
		fTree->Branch("segmentRow", &segmentRow);
		fTree->Branch("photonsProd", &Nphotons);
		fTree->Branch("recoilMass", &recoilMass);
	}
	
	fTree->Branch("lightBalance", &photonLightBalance);
	fTree->Branch("photonDetEff", &photonDetEfficiency);
	fTree->Branch("barTOF", &barTOF);
	fTree->Branch("barQDC", &barQDC);
	fTree->Branch("barMaxADC", &barMaxADC);
	if(outputBadEvents)
		fTree->Branch("goodEvent", &goodEvent);
	fTree->Branch("pulsePhase[2]", pulsePhase);
	fTree->Branch("photonComX[2]", photonDetCenterOfMassX);
	fTree->Branch("photonComY[2]", photonDetCenterOfMassY);
	fTree->Branch("reconComX[2]", reconstructedCenterX);
	fTree->Branch("reconComY[2]", reconstructedCenterY);
	fTree->Branch("photonComCol[2]", centerOfMassColumn);
	fTree->Branch("photonComRow[2]", centerOfMassRow);

	if(outputDebug){ // Output extra photon information (off by default).
		fTree->Branch("nPhotons[2]", nPhotonsDet);
		fTree->Branch("photonMinTime[2]", photonMinArrivalTime);
		fTree->Branch("photonAvgTime[2]", photonAvgArrivalTime);		
		fTree->Branch("pulseQDC[2]", pulseQDC);
		fTree->Branch("pulseMax[2]", pulseMax);
		fTree->Branch("detSpdLight", &detSpeedLight);
		fTree->Branch("anode", anodeCurrent, "anode[2][4]/D");
	}

	if(outputTraces){ // Add the lilght pulses to the output tree.
			fTree->Branch("lightPulseL", &lightPulseL);
			fTree->Branch("lightPulseR", &lightPulseR);
	}

	defineRootBranch = true;
		
	return true;
}//end of open root file...

bool nDetRunAction::closeRootFile(){
	// Close the root file.
	if(fFile && IsMaster()){
		fFile->cd();
		fTree->Write();
		fFile->Close();
		delete fFile;
		fFile = NULL;
	}
	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool nDetRunAction::fillBranch()
{
	if(!outputEnabled) return false;

	// The first recoil particle ID is equal to 2
	for(short i = nScatters+1; i >= 2; i--)
		Nphotons.push_back(counter->getPhotonCount(i));

	centerOfMass *cmL = detector->GetCenterOfMassPositiveSide();
	centerOfMass *cmR = detector->GetCenterOfMassNegativeSide();

	// Get the number of detected photons
	nPhotonsDet[0] = cmL->getNumDetected();
	nPhotonsDet[1] = cmR->getNumDetected();
	nPhotonsDetTot = nPhotonsDet[0]+nPhotonsDet[1];

	// Compute the photon detection efficiency
	nPhotonsTot = stacking->GetNumPhotonsProduced();
	if(nPhotonsTot > 0)
		photonDetEfficiency = nPhotonsDetTot/(double)nPhotonsTot;
	else
		photonDetEfficiency = -1;

	// Check for valid bar detection
	if(nPhotonsDet[0]*nPhotonsDet[1] > 0)
		goodEvent = true;

	// Get the photon center-of-mass positions
	G4ThreeVector centerL = cmL->getCenter();
	G4ThreeVector centerR = cmR->getCenter();
	photonDetCenterOfMassX[0] = centerL.getX(); photonDetCenterOfMassX[1] = centerR.getX(); 
	photonDetCenterOfMassY[0] = centerL.getY(); photonDetCenterOfMassY[1] = centerR.getY(); 
	photonDetCenterOfMassZ[0] = centerL.getZ(); photonDetCenterOfMassZ[1] = centerR.getZ(); 

	// Get photon arrival times at the PMTs
	photonMinArrivalTime[0] = cmL->getMinArrivalTime();
	photonAvgArrivalTime[0] = cmL->getAvgArrivalTime();

	photonMinArrivalTime[1] = cmR->getMinArrivalTime();
	photonAvgArrivalTime[1] = cmR->getAvgArrivalTime();

	pmtResponse *pmtL = getPmtResponseLeft();
	pmtResponse *pmtR = getPmtResponseRight();

	// "Digitize" the light pulses.
	pmtL->digitize(baselineFraction, baselineJitterFraction);
	pmtR->digitize(baselineFraction, baselineJitterFraction);

	// Check for saturated pulse.
	if(pmtL->getPulseIsSaturated() || pmtR->getPulseIsSaturated()){
		std::cout << " Warning: One or more PMT's traces have saturated! Recommend lowering the gain.\n";
	}

	// Copy the trace into the trace vector.
	if(outputTraces){
		pmtL->copyTrace(lightPulseL);
		pmtR->copyTrace(lightPulseR);
	}

	// Do some light pulse analysis
	pulsePhase[0] = pmtL->analyzePolyCFD(polyCfdFraction);
	pulseQDC[0] = pmtL->integratePulseFromMaximum(pulseIntegralLow, pulseIntegralHigh);
	pulseMax[0] = pmtL->getMaximum();

	pulsePhase[1] = pmtR->analyzePolyCFD(polyCfdFraction);
	pulseQDC[1] = pmtR->integratePulseFromMaximum(pulseIntegralLow, pulseIntegralHigh);
	pulseMax[1] = pmtR->getMaximum();

	if(outputDebug){
		// Compute the bar speed-of-light.
		double barTimeDiff = pulsePhase[1] - pulsePhase[0];
		detSpeedLight = 2*neutronCenterOfMass[2]/barTimeDiff;
	
		// Compute the neutron scatter center-of-mass.
		neutronCenterOfMass[0] = neutronCenterOfMass[0]/neutronWeight;
		neutronCenterOfMass[1] = neutronCenterOfMass[1]/neutronWeight;
		neutronCenterOfMass[2] = neutronCenterOfMass[2]/neutronWeight;
	}

	// Compute the light balance (Z).
	photonLightBalance = (pulseQDC[0]-pulseQDC[1])/(pulseQDC[0]+pulseQDC[1]);

	// Compute "bar" variables.
	barCenterOfMassX = (photonDetCenterOfMassX[0]+photonDetCenterOfMassX[1])/2;
	barCenterOfMassY = (photonDetCenterOfMassY[0]+photonDetCenterOfMassY[1])/2;
	barTOF = (pulsePhase[0]+pulsePhase[1])/2;
	barQDC = std::sqrt(pulseQDC[0]*pulseQDC[1]);
	barMaxADC = std::sqrt(pulseMax[0]*pulseMax[1]);

	// Retrieve the reconstructed center of mass position.
	reconstructedCenterX[0] = -cmL->getReconstructedX();
	reconstructedCenterY[0] = cmL->getReconstructedY();
	reconstructedCenterX[1] = -cmR->getReconstructedX();
	reconstructedCenterY[1] = cmR->getReconstructedY();

	// Get the segment of the detector where the photon CoM occurs.
	cmL->getCenterSegment(centerOfMassColumn[0], centerOfMassRow[0]);
	cmR->getCenterSegment(centerOfMassColumn[1], centerOfMassRow[1]);
	
	if(outputDebug){ // Retrieve the anode currents.
		cmL->getAnodeCurrents(anodeCurrent[0]);
		cmR->getAnodeCurrents(anodeCurrent[1]);
	}
	
	// Clear all photon statistics from the detector.
	detector->Clear();

	if(outputBadEvents || goodEvent)
		fTree->Fill(); // Fill the tree

	return true;
}

void nDetRunAction::vectorClear(){
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

	lightPulseL.clear();
	lightPulseR.clear();

	stacking->Reset();
	tracking->Reset();
	stepping->Reset();
}

void nDetRunAction::setOutputFilename(const std::string &fname){
	filename = fname;
	filenamePrefix = filename;
	filenameSuffix = "";
	size_t index = filename.find_last_of('.');
	if(index != std::string::npos){
		filenamePrefix = filename.substr(0, index);
		filenameSuffix = filename.substr(index);
	}
	runIndex = 1;
	if(verbose){
		std::cout << " Output filename prefix=" << filenamePrefix << ", suffix=" << filenameSuffix << std::endl;
		std::cout << " Reset output filename run counter to 1\n";
	}
}

void nDetRunAction::setActions(nDetEventAction *event_, nDetStackingAction *stacking_, nDetTrackingAction *tracking_, nDetSteppingAction *stepping_){
	eventAction = event_;
	stacking = stacking_;
	tracking = tracking_;
	stepping = stepping_;

	// Get the photon counter
	counter = stacking->GetCounter();
}

void nDetRunAction::scatterEvent(const G4Track *track){
	if(primaryTracks.empty()){
		if(verbose) 
			std::cout << " Warning: primaryTracks is empty!\n";
		return;
	}

	// Get a primary particle step off the stack.
	primaryTrackInfo *priTrack = &primaryTracks.back();

	if(!priTrack->compare(track)){
		G4ThreeVector vec = track->GetPosition();
		if(verbose) 
			std::cout << " Warning: Scatter pos=(" << priTrack->pos.getX() << ", " << priTrack->pos.getY() << ", " << priTrack->pos.getZ() << ") does not match recoil pos=(" << vec.getX() << ", " << vec.getY() << ", " << vec.getZ() << "), primaryTracks.size()=" << primaryTracks.size() << std::endl;
		return;
	}

	if(nScatters++ == 0){ // First scatter event (CAREFUL! These are in reverse order).
		if(primaryTracks.size() < 2){
			if(verbose){
				std::cout << " Warning: Expected at least two events in primaryTracks, but primaryTracks.size()=" << primaryTracks.size() << std::endl;
				priTrack->print();
			}
			return;
		}
		if(outputDebug){
			G4ThreeVector firstScatter = primaryTracks.at(0).pos + primaryTracks.at(1).pos;
			nTimeToFirstScatter = primaryTracks.at(1).gtime;
			nLengthToFirstScatter = firstScatter.mag();
		}
		counter->push_back(track->GetTrackID());
	}
	
	// Add the deposited energy to the total.
	depEnergy += priTrack->dkE;

	G4ThreeVector vertex = priTrack->pos;

	if(verbose){
		primaryTrackInfo pizza(track);
		std::cout << " id=" << track->GetTrackID() << " "; pizza.print();	
		std::cout << "	pos=(" << vertex.getX() << ", " << vertex.getY() << ", " << vertex.getZ() << ")\n";
		std::cout << "	dE=" << priTrack->dkE << ", dE2=" << track->GetKineticEnergy() << ", size=" << primaryTracks.size() << std::endl;
	}

	if(outputDebug){
		nScatterX.push_back(vertex.getX());
		nScatterY.push_back(vertex.getY());
		nScatterZ.push_back(vertex.getZ());

		neutronCenterOfMass[0] += priTrack->dkE*vertex.getX();
		neutronCenterOfMass[1] += priTrack->dkE*vertex.getY();
		neutronCenterOfMass[2] += priTrack->dkE*vertex.getZ();
		neutronWeight += priTrack->dkE;

		nScatterAngle.push_back(priTrack->angle);
		impartedE.push_back(priTrack->dkE);

		nPathLength.push_back(priTrack->plength);
		scatterTime.push_back(priTrack->gtime - incidentTime);
		recoilMass.push_back(track->GetParticleDefinition()->GetAtomicMass()); 

		G4int segCol, segRow;
		detector->GetSegmentFromCopyNum(track->GetTouchable()->GetCopyNumber(), segCol, segRow);
		segmentCol.push_back(segCol);
		segmentRow.push_back(segRow);
	}
	
	primaryTracks.pop_back();
}

void nDetRunAction::initializeNeutron(const G4Step *step){
	if(outputDebug){
		incidentTime = step->GetTrack()->GetGlobalTime();
		neutronIncidentPositionX = step->GetPostStepPoint()->GetPosition().getX();
		neutronIncidentPositionY = step->GetPostStepPoint()->GetPosition().getY();
		neutronIncidentPositionZ = step->GetPostStepPoint()->GetPosition().getZ();
		timeInMaterial = 0;
		neutronExitPositionX = 0;
		neutronExitPositionY = 0;
		neutronExitPositionZ = 0;
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
			nAbsorbed = true;
			track->SetTrackStatus(fStopAndKill); // Kill the neutron.
			if(verbose) 
				std::cout << "OT: final kE=0 (absorbed)" << std::endl;
		}
	}
}

void nDetRunAction::finalizeNeutron(const G4Step *step){
	nAbsorbed = false;
	G4Track *track = step->GetTrack();
	if(outputDebug){
		timeInMaterial = track->GetGlobalTime() - incidentTime;
		neutronExitPositionX = track->GetPosition().getX();
		neutronExitPositionY = track->GetPosition().getY();
		neutronExitPositionZ = track->GetPosition().getZ();
	}
	if(verbose) 
		std::cout << "OT: final kE=" << track->GetKineticEnergy() << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
