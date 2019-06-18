#include <vector>
#include <fstream>

#include "G4Run.hh"
#include "G4Timer.hh"
#include "G4MTRunManager.hh"
#include "G4UserRunAction.hh"

#include "TFile.h"
#include "TTree.h"

#include "nDetParticleSource.hh"
#include "nDetParticleSourceMessenger.hh"
#include "nDetRunAction.hh"
#include "nDetConstruction.hh"
#include "nDetConstructionMessenger.hh"
#include "nDetMasterOutputFile.hh"
#include "nDetMasterOutputFileMessenger.hh"
#include "nDetThreadContainer.hh"
#include "photonCounter.hh"
#include "termColors.hh"

nDetMasterOutputFile &nDetMasterOutputFile::getInstance(){
	// The only instance
	// Guaranteed to be lazy initialized
	// Guaranteed that it will be destroyed correctly
	static nDetMasterOutputFile instance;
	return instance;
}

nDetMasterOutputFile::nDetMasterOutputFile(){
	persistentMode = false;
	verbose = false;

	outputEnabled = true;
	outputTraces = false;
	outputDebug = false;
	outputBadEvents = false;
	singleDetectorMode = true;

	runIndex = 1;
	fFile = NULL;
	fTree = NULL;

	runTitle = "NEXT Geant4 output";
	runIndex = 1;

	// Timer initialization.
	timer = new G4Timer();
	previousTime = 0;
	totalTime = 0;
	totalEvents = 0;
	displayTimeInterval = 10;

	// Create a messenger for this class
	fMessenger = new nDetMasterOutputFileMessenger(this); 

	overwriteExistingFile = false;
	
	evtData = new nDetEventStructure();
	outData = new nDetOutputStructure();
	multData = new nDetMultiOutputStructure();
	debugData = new nDetDebugStructure();
	traceData = new nDetTraceStructure();
}

nDetMasterOutputFile::~nDetMasterOutputFile(){
	// Close the root file, if it's still open.
	closeRootFile();
	
	delete evtData;
	delete outData;
	delete multData;
	delete debugData;
	delete traceData;
	
	delete fMessenger;
	delete timer;
}

bool nDetMasterOutputFile::openRootFile(const G4Run* aRun){
	if(!outputEnabled || (persistentMode && aRun->GetRunID() != 0)) return false;
	
	// Close the output file if it is open.
	closeRootFile();	
	
	// Open a new one.
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
			Display::ErrorPrint("Failed to open output file \""+filename+"\"!", "nDetMasterOutputFile");
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
						std::cout << "nDetMasterOutputFile: File \"" << newFilename << "\" already exists.\n";
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
				Display::ErrorPrint("Failed to open output file \""+filename+"\"!", "nDetMasterOutputFile");
				return false;
			}
			break;
		}
	}

	// Add user commands to the output file.
	TDirectory *dir = fFile->mkdir("setup");
	writeMessengerCommands(fMessenger, dir);
	writeMessengerCommands(nDetConstruction::getInstance().GetMessenger(), dir);
	const G4UserRunAction *runAction = NULL;
#ifdef USE_MULTITHREAD
	if(G4MTRunManager::GetMasterRunManager()) // Multithreaded mode.
		runAction = G4MTRunManager::GetMasterRunManager()->GetUserRunAction();
	else if(G4RunManager::GetRunManager()) // Sequential mode.
		runAction = G4RunManager::GetRunManager()->GetUserRunAction();
#else
	if(G4RunManager::GetRunManager())
		runAction = G4RunManager::GetRunManager()->GetUserRunAction();
#endif
	if(runAction){
		writeMessengerCommands(((nDetRunAction*)runAction)->getParticleSource()->GetMessenger(), dir);
	}
	else
		Display::WarningPrint("Failed to find master run manager.", "nDetMasterOutputFile");

	// Create root tree.
	if(treename.empty()) treename = "data"; //"neutronEvent";
	fTree = new TTree(treename.c_str(),"Photons produced by thermal neutrons");

	// Add the branches
	fTree->Branch("event", evtData);
	if(singleDetectorMode){ // Add the single-detector branches
		fTree->Branch("output", outData);
		if(outputDebug) // Add the debug branch
			fTree->Branch("debug", debugData);
	}
	else // Add the multiple-detector branch
		fTree->Branch("output", multData);
	if(outputTraces) // Add the trace branch
		fTree->Branch("trace", traceData);

	std::cout << "nDetMasterOutputFile: File " << fFile->GetName() << " opened." << std::endl;
	
	// Start the timer.
	timer->Start();
	
	return true;
}

bool nDetMasterOutputFile::closeRootFile(){
	// Close the root file.
	if(fFile){
		fFile->cd();
		fTree->Write();
		fFile->Close();
		delete fFile;
		fFile = NULL;
	}
	return true;
}

bool nDetMasterOutputFile::fillBranch(const nDetDataPack &pack){
	if(!outputEnabled) return false;

	// Enable the mutex lock to protect file access.
	fileLock.lock();

	// Copy the data
	pack.copyData(evtData, outData, multData, debugData, traceData);

	if(outputBadEvents || pack.goodEvent())
		fTree->Fill(); // Fill the tree

	double avgTimePerEvent;
	double avgTimePerPhoton;
	double avgTimePerDetection;

	if(pack.getEventID() != 0){ // Status output.
		timer->Stop();
		nDetThreadContainer *container = &nDetThreadContainer::getInstance();
		unsigned long long numPhotons = 0;
		unsigned long long numPhotonsDet = 0;
		for(size_t index = 0; index < container->size(); index++){
			numPhotons += container->getActionManager(index)->getRunAction()->getNumPhotons();
			numPhotonsDet += container->getActionManager(index)->getRunAction()->getNumPhotonsDet();
		}
		totalTime += timer->GetRealElapsed();
		if(displayTimeInterval > 0 && (totalTime - previousTime) >= displayTimeInterval){ // Display every 10 seconds.
			std::cout << "Event ID: " << pack.getEventID() << ", TIME=" << totalTime << " s";
			avgTimePerEvent = totalTime/pack.getEventID();
			avgTimePerPhoton = totalTime/numPhotons;
			avgTimePerDetection = totalTime/numPhotonsDet;
			if(totalEvents > 0){
				std::cout << ", REMAINING=" << (totalEvents-pack.getEventID())*avgTimePerEvent << " s";
			}
			std::cout << ", RATE=" << 1/avgTimePerEvent << " evt/s (" << 1/avgTimePerPhoton << " phot/s & " << 1/avgTimePerDetection << " det/s)\n";
			previousTime = totalTime;
		}
	}

	// Start the timer.
	timer->Start();

	// Disable the mutex lock to open access to the file.
	fileLock.unlock();

	return true;
}

void nDetMasterOutputFile::setOutputFilename(const std::string &fname){
	size_t index = fname.find('.');
	std::string prefix = fname;
	if(index != std::string::npos) 
		prefix = fname.substr(0, index);
	filename = prefix + ".root";
	filenamePrefix = filename;
	filenameSuffix = "";
	index = filename.find_last_of('.');
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

bool nDetMasterOutputFile::writeInfoToFile(const std::string &name, const std::string &value){
	if(fFile && fFile->IsOpen()){
		fFile->cd();
		TNamed named(name.c_str(), value.c_str());
		named.Write();
	}
	return false;
}

void nDetMasterOutputFile::writeMessengerCommands(messengerHandler* messenger, TDirectory* directory){
	directory->cd();
	std::vector<std::string> userCommands = messenger->getAllUserCommands();
	for(std::vector<std::string>::iterator iter = userCommands.begin(); iter != userCommands.end(); iter++){ // Write the commands to the output file.
		std::string cmd = (*iter);
		size_t index1 = iter->find_last_of('/');
		if(index1 != std::string::npos){
			size_t index2 = iter->find_first_of(' ');
			if(index2 != std::string::npos)
				cmd = cmd.substr(index1+1, index2-(index1+1));
			else
				cmd = cmd.substr(index1+1);
		}
		TNamed named(cmd.c_str(), iter->c_str());
		named.Write();
	}
}
