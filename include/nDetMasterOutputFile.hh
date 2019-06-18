#ifndef NDET_MASTER_OUTPUT_FILE_HH
#define NDET_MASTER_OUTPUT_FILE_HH

#include <mutex>

#include "centerOfMass.hh"
#include "nDetDataPack.hh"

class G4Run;
class G4Timer;

class TFile;
class TTree;

class photonCounter;
class messengerHandler;
class nDetMasterOutputFileMessenger;

class nDetMasterOutputFile{
  public:
	~nDetMasterOutputFile();

	nDetMasterOutputFile(nDetMasterOutputFile const &copy); // Not Implemented
	
	nDetMasterOutputFile &operator=(nDetMasterOutputFile const &copy); // Not Implemented

	static nDetMasterOutputFile &getInstance();

	bool fillBranch(const nDetDataPack &pack);

	bool getOutputDebug() const { return outputDebug; }

	bool getOutputTraces() const { return outputTraces; }

	void setOutputFilename(const std::string &fname);

	void setOutputTreeName(const std::string &tname){ treename = tname; }
	
	void setPersistentMode(const bool &enabled){ persistentMode = enabled; }
	
	void setOutputEnabled(const bool &enabled){ outputEnabled = enabled; }
	
	void setOutputFileTitle(const std::string &title){ runTitle = title; }	

	void setOutputFileIndex(const int &index){ runIndex = index; }
	
	void setOutputTraces(const bool &enabled){ outputTraces = enabled; }
	
	void setOutputDebug(const bool &enabled){ outputDebug = enabled; }
	
	void setOutputBadEvents(const bool &enabled){ outputBadEvents = enabled; }

	void setOverwriteOutputFile(const bool &overwrite){ overwriteExistingFile = overwrite; }

    void setTotalEvents(const G4long &events){ totalEvents = events; }

	void setDisplayTimeInterval(const int &interval){ displayTimeInterval = interval; }

	/** Set output mode for multiple detectors
	  * @param enabled Flag indicating that there is more than one detector in the setup
	  */
	void setMultiDetectorMode(const bool &enabled){ singleDetectorMode = !enabled; }

	bool writeInfoToFile(const std::string &name, const std::string &value);

	/** Write all user-called commands to an output TFile
	  * @param messenger Pointer to a messengerHandler object whose commands will be written to file
	  * @param directory Pointer to the current directory of an output TFile where the commands will be written
	  */
	void writeMessengerCommands(messengerHandler* messenger, TDirectory* directory);

	bool toggleVerboseMode(){ return (verbose = !verbose); }

	bool openRootFile(const G4Run* aRun);

	bool closeRootFile();

  private:
	std::mutex fileLock; ///< Mutex lock for thread-safe TTree filling

	std::string filename; ///< Default time & date filename when output filename unspecified by user
	std::string treename; ///< Name of the output TTree
	
	std::string filenamePrefix; ///< Prefix of output filename i.e. [PREFIX]-xxx.SUFFIX
	std::string filenameSuffix; ///< Suffix of output filename i.e. PREFIX-xxx.[SUFFIX]

	std::string runTitle; ///< Title of the output root file
	int runIndex; ///< Geant run ID number
	
	TFile *fFile; ///< Pointer to the output root file
	TTree *fTree; ///< Pointer to the output TTree

	bool persistentMode; ///< Flag indicating whether or not the output file should remain open for subsequent runs
	bool verbose; ///< Verbosity flag
	
	bool outputEnabled; ///< Flag indicating that root file output is enabled
	bool outputTraces; ///< Flag indicating that traces will be written to the output tree
	bool outputDebug; ///< Flag indicating that low-level scattering information will be written to the output tree
	bool outputBadEvents; ///< Flag indicating that invalid detection events will be written to the output tree
	bool overwriteExistingFile; ///< Flag indicating that files with matching filenames will be overwritten
	bool singleDetectorMode; ///< Flag indicating that there is only one detector in the setup

	nDetEventStructure *evtData; ///< Pointer to data structure containing Geant4 event information
	nDetOutputStructure *outData; ///< Pointer to data structure containing normal (single-detector) output variables
	nDetMultiOutputStructure *multData; ///< Pointer to data structure containing multi-detector output variables
	nDetDebugStructure *debugData; ///< Pointer to data structure containing debug output variables
	nDetTraceStructure *traceData; ///< Pointer to data structure containing PMT response light pulses

    G4Timer *timer; ///< Geant timer used to measure time between successive status updates
    
    G4double previousTime; ///< Time of previous status output (in seconds)
    G4double totalTime; ///< Total time of current run (in seconds)
    
    G4long totalEvents; ///< Total number of events for the current run
    
    int displayTimeInterval; ///< Time to wait between successive status updates (in seconds)

	nDetMasterOutputFileMessenger *fMessenger; ///< Pointer to the messenger object used for this class

	nDetMasterOutputFile();	
};

#endif
