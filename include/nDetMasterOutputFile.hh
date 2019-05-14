#ifndef NDET_MASTER_OUTPUT_FILE_HH
#define NDET_MASTER_OUTPUT_FILE_HH

#include <mutex>

#include "centerOfMass.hh"
#include "nDetDataPack.hh"

class G4Run;
class G4Timer;

class TFile;
class TTree;
class TBranch;

class photonCounter;
class nDetMasterOutputFileMessenger;

class nDetMasterOutputFile{
  public:
	~nDetMasterOutputFile();

	nDetMasterOutputFile(nDetMasterOutputFile const &copy); // Not Implemented
	
	nDetMasterOutputFile &operator=(nDetMasterOutputFile const &copy); // Not Implemented

	static nDetMasterOutputFile &getInstance();

	bool fillBranch(const nDetDataPack &pack);

	bool getOutputDebug() const { return outputDebug; }

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

	bool toggleVerboseMode(){ return (verbose = !verbose); }

	bool openRootFile(const G4Run* aRun);

	bool closeRootFile();

  private:
	std::mutex fileLock;

	std::string filename;
	std::string treename;
	
	std::string filenamePrefix;
	std::string filenameSuffix;

	std::string runTitle;
	int runIndex;
	
	TFile *fFile;
	TTree *fTree;
	TBranch *fBranch;

	bool defineRootBranch;
	bool persistentMode;
	bool verbose;
	
	bool outputEnabled;
	bool outputTraces;
	bool outputDebug;
	bool outputBadEvents;
	bool overwriteExistingFile;	

	nDetDataPack data;

    G4Timer *timer;
    
    G4double previousTime;
    G4double totalTime;
    
    G4long totalEvents;
    
    int displayTimeInterval; // In seconds.

	nDetMasterOutputFileMessenger *fMessenger;

	nDetMasterOutputFile();	
};

#endif
