#ifndef NDET_CONSTRUCTION_HH
#define NDET_CONSTRUCTION_HH

#include <vector>

#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "centerOfMass.hh"
#include "nDetDetector.hh"
#include "nDetMaterials.hh"

// Class declarations
class nDetConstructionMessenger;
class nDetWorld;

class G4Material;
class G4VPhysicalVolume;

/** @class nDetConstruction
  * @brief Handles construction of NEXTSim detector setups
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 10, 2019
  */

class nDetConstruction : public G4VUserDetectorConstruction{
  public:
	/** Destructor
	  */
	~nDetConstruction();

	/** Copy constructor. Not implemented for singleton class
	  */
	nDetConstruction(nDetConstruction const &copy);

	/** Assignment operator. Not implemented for singleton class
	  */	
	nDetConstruction &operator=(nDetConstruction const &copy);

	/** Return an instance of this class
	  */
	static nDetConstruction &getInstance();

	/** Construct the detector setup
	  * @note This method does nothing on its own, all the work is done by ConstructDetector()
	  * @return A pointer to the world physical volume
	  */
	G4VPhysicalVolume* Construct();

	/** Build the world volume and place all detectors defined in the detector list
	  * @return A pointer to the world physical volume
	  */
	G4VPhysicalVolume* ConstructDetector();

	/** Add a detector geometry to the list of detectors
	  * @note See nDetDetector::setGeometry() for accepted geometry names
	  * @return True if the specified type is recognized and return false otherwise
	  */
	bool AddGeometry(const G4String &geom);

	/** Setup segmented PMTs
	  */
	void setSegmentedPmt();

	/** Load PMT quantum efficiency spectrum from a file
	  * @note See centerOfMass::loadSpectralResponse() for information on required file contents
	  * @param fname Path to file containing anode quantum efficiency spectrum
	  */
	void setPmtSpectralResponse(const std::string &fname){ spectralResponseFilename = fname; }

	/** Load segmented PMT anode gain matrix from a file
	  * @note See centerOfMass::loadGainMatrix() for information on required file contents
	  * @param fname Path to file containing a matrix of anode gain percentages
	  */
	void setPmtGainMatrix(const std::string &fname){ gainMatrixFilename = fname; }

	/** Get a pointer to the NEXTSim materials handler
	  */
	nDetMaterials *GetMaterials(){ return &materials; }

	/** Get a pointer to the messenger used for this class
	  */
	nDetConstructionMessenger *GetMessenger(){ return fDetectorMessenger; }

	/** Return a pointer to the optical photon center-of-mass calculator for the left PMT
	  */
	centerOfMass *GetCenterOfMassL(){ return &center[0]; }

	/** Return a pointer to the optical photon center-of-mass calculator for the right PMT
	  */
	centerOfMass *GetCenterOfMassR(){ return &center[1]; }

	/** Return a pointer to the PMT response for the left PMT
	  */
	pmtResponse *GetPmtResponseL(){ return center[0].getPmtResponse(); }

	/** Return a pointer to the PMT response for the right PMT
	  */
	pmtResponse *GetPmtResponseR(){ return center[1].getPmtResponse(); }

	/** Get a copy of the current vector of detectors
	  */
	std::vector<nDetDetector> GetUserDetectors() const { return userDetectors; }

	/** Get the current user defined detector parameters
	  */
	nDetDetectorParams GetDetectorParameters() const { return params; }

	/** Get a pointer to the current detector
	  */
	nDetDetector *getCurrentDetector(){ return currentDetector; }

	/** Clear all volumes, surfaces, and detectors
	  */
	void ClearGeometry();

	/** Place all detectors into the world and copy the list of detectors to all user run actions
	  */	
	void UpdateGeometry();

	/** Load a light guide model from a file using parameters from a space-delimited input string and place it into the current detector assembly
	  * @note See nDetDetector::addLightGuideGDML(const G4String &) for input string syntax
	  */
	void AddLightGuideGDML(const G4String &input);

	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note See nDetDetector::addGreaseLayer(const G4String &) for input string syntax
	  */
	void AddGrease(const G4String &input);

	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See nDetDetector::addDiffuserLayer(const G4String &) for input string syntax
	  */
	  void AddDiffuser(const G4String &input);

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note See nDetDetector::addLightGuideLayer(const G4String &) for input string syntax
	  */
	void AddLightGuide(const G4String &input);
	
	/** Add an array of detectors in a cylindrical setup
	  * @note String syntax: <geom> <r0> <startTheta> <stopTheta> <Ndet> <br>
	  * | Parameter  | Description |
	  * |------------|-------------|
	  * | geom       | Name of detector geometry (see nDetDetector::setGeometry() for valid types)
	  * | r0         | Radius of the center of the detectors (in cm)
	  * | startTheta | The angle of the first detector (in degrees)
	  * | stopTheta  | The angle of the final detector (in degrees)
	  * | Ndet       | Number of detectors in the array
	  */
	void AddDetectorArray(const G4String &input);

	/** Set the light yield multiplier for scintillator photon production
	  * @param yield The fraction of the light yield to use for optical photon production in scintillators (default is 1)
	  */	
	void SetLightYieldMultiplier(const G4double &yield);
	
	/** Print detailed information about all defined detectors
	  */
	void PrintAllDetectors() const ;
	
  private:
	nDetConstructionMessenger *fDetectorMessenger; ///< Geant messenger to use for this class
	
	nDetDetectorParams params; ///< Physical attributes of the current detector assembly

	G4bool fCheckOverlaps; ///< Flag indicating that Geant should check for overlaps between all placed objects

	std::vector<nDetDetector> userDetectors; ///< Vector of all detectors added by the user
	
	nDetDetector *currentDetector; ///< Pointer to the current detector added by the user

	centerOfMass center[2]; ///< Objects used to compute the detected optical photon center-of-mass position for the left and right PMT

	nDetMaterials materials; ///< Object containing all Geant materials and elements which will be used in detector construction

	std::string gainMatrixFilename; ///< Path to the anode gain matrix
	std::string spectralResponseFilename; ///< Path to the anode quantum efficiency

	nDetWorld *expHall; ///< Pointer to the experimental hall setup area

	/** Default constructor. Private for singleton class
	  */
	nDetConstruction();

	/** Load PMT quantum efficiency spectrum from a file
	  * @return True if the quantum efficiency is loaded correctly and return false otherwise
	  */
	bool loadPmtSpectralResponse();

	/** Load segmented PMT anode gain matrix from a file
	  * @return True if the anode gain matrix is loaded correctly and return false otherwise
	  */
	bool loadPmtGainMatrix();
};

#endif
