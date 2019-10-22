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

/** @class nDetMasterOutputFile
  * @brief Handles NEXTSim simulation output files
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date May 9, 2019
  */

class nDetMasterOutputFile{
  public:
	/** Destructor
	  */
	~nDetMasterOutputFile();

	/** Copy constructor. Not implemented for singleton class
	  */
	nDetMasterOutputFile(nDetMasterOutputFile const &copy);

	/** Assignment operator. Not implemented for singleton class
	  */
	nDetMasterOutputFile &operator=(nDetMasterOutputFile const &copy);

	/** Return an instance of this class
	  */
	static nDetMasterOutputFile &getInstance();

	/** Safely fill the output branches (thread safe)
	  * @param pack Data structure to copy simulation data from
	  * @return True if file output is enabled and return false otherwise
	  */
	bool fillBranch(const nDetDataPack &pack);

	/** Return true if writing of debug scattering data is enabled
	  */
	bool getOutputDebug() const { return outputDebug; }

	/** Return true if writing of simulated light pulses is enabled
	  */
	bool getOutputTraces() const { return outputTraces; }

	/** Set the output filename
	  */
	void setOutputFilename(const std::string &fname);

	/** Set the name of the output TTree
	  */
	void setOutputTreeName(const std::string &tname){ treename = tname; }

	/** Enable or disable persistent file mode (disabled by default). If persistent mode is
	  * enabled, the output file will remain open even when the Geant run number is changed
	  */	
	void setPersistentMode(const bool &enabled){ persistentMode = enabled; }
	
	/** Enable or disable file output
	  */
	void setOutputEnabled(const bool &enabled){ outputEnabled = enabled; }

	/** Set the title of the output file
	  */	
	void setOutputFileTitle(const std::string &title){ runTitle = title; }	

	/** Set the output filename index e.g. filename-xxx.root where @a xxx is the index
	  */
	void setOutputFileIndex(const int &index){ runIndex = index; }

	/** Enable or disable writing of simulated light pulses to the output file
	  */	
	void setOutputTraces(const bool &enabled){ outputTraces = enabled; }

	/** Enable or disable writing of debug scattering data to the output file
	  */
	void setOutputDebug(const bool &enabled){ outputDebug = enabled; }

	/** Enable or disable writing of non-detection events to the output file
	  */	
	void setOutputBadEvents(const bool &enabled){ outputBadEvents = enabled; }

	/** Enable or disable over-writing of the output file
	  */
	void setOverwriteOutputFile(const bool &overwrite){ overwriteExistingFile = overwrite; }
	
	/** Set the total number of events to be simulated
	  */
    void setTotalEvents(const G4long &events){ totalEvents = events; }

	/** Set the time to wait between successive status updates (in seconds)
	  */
	void setDisplayTimeInterval(const int &interval){ displayTimeInterval = interval; }

	/** Set output mode for multiple detectors
	  * @param enabled Flag indicating that there is more than one detector in the setup
	  */
	void setMultiDetectorMode(const bool &enabled){ singleDetectorMode = !enabled; }

	/** Write a TNamed object, which contains a name and a value string, to the output file
	  * @param name The name of the data member to write to the file
	  * @param value The title string of the data member
	  * @return True if the output file is open and return false otherwise
	  */
	bool writeInfoToFile(const std::string &name, const std::string &value);

	/** Write all user-called commands to an output TFile
	  * @param messenger Pointer to a messengerHandler object whose commands will be written to file
	  * @param directory Pointer to the current directory of an output TFile where the commands will be written
	  */
	void writeMessengerCommands(messengerHandler* messenger, TDirectory* directory);

	/** Toggle the verbosity flag
	  * @return The state of the verbosity flag
	  */
	bool toggleVerboseMode(){ return (verbose = !verbose); }

	/** Open a new output file. If the user has not set the output filename, the output file will
	  * be given a filename containing the date and time the file is created
	  * @param aRun Pointer to a G4Run object which is used to obtain the run number
	  * @return True if file output is enabled and a new output file is opened successfully and return false otherwise
	  */
	bool openRootFile(const G4Run* aRun);

	/** Close the output file, if one is open
	  * @return True
	  */
	bool closeRootFile();

	/** Print a status message to stdout
	  */
	void printMessage(const G4String &msg) const ;

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

	/** Default constructor (private because we must only have one instance of the output file)
	  */
	nDetMasterOutputFile();	
};

#endif
