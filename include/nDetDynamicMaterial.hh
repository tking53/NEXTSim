#ifndef NDET_DYNAMIC_MATERIAL_HH
#define NDET_DYNAMIC_MATERIAL_HH

#include <string>
#include <map>
#include <vector>

class G4Material;
class G4MaterialPropertiesTable;

class nDetMaterials;

/** @class nDetDynamicMaterial
  * @brief Build a new G4Material by reading a file
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date August 29, 2019
  *
  * This class may be used to build a new G4Material at runtime by reading material setup
  * parameters from an input file. This allows for the dynamic creation of materials and
  * removes the need for the user to hard-code their materials into NEXTSim. 
  */

class nDetDynamicMaterial{
public:
	/** Default constructor
	  */
	nDetDynamicMaterial();

	/** Destructor
	  */
	~nDetDynamicMaterial();

	/** Return true if the material has been initialized and return false otherwise
	  */
	bool isInitialized() const { return init; }
	
	/** Toggle debug mode
	  */
	bool toggleDebug(){ return (debug = !debug); }

	/** Get the prefix of the input filename
	  */
	std::string getFilePrefix() const { return prefix; }
	
	/** Get the name of the material
	  */
	std::string getName() const { return name; }

	/** Get a pointer to the Geant material object
	  */
	G4Material* getMaterial(){ return material; }
	
	/** Get a pointer to the Geant material properties table
	  */
	G4MaterialPropertiesTable *getMaterialPropertiesTable(){ return mpt; }

	/** Get a pointer to the visual attributes for this material
	  */
	G4VisAttributes *getVisAttributes(){ return visatt; }

	/** Set the value scaling parameter for arrays which are scalable
	  */
	void setScalingFactor(const double &factor){ scalingFactor = factor; }

	/** Read an input file and build the material
	  * @param fname Path to the input ascii file
	  * @param materials Pointer to the NEXTSim material handler
	  * @return True if the file is read successfully and return false otherwise
	  */
	bool read(const std::string &fname, nDetMaterials* materials);

private:
	std::string prefix; ///< The prefix of the input filename
	std::string name; ///< The name of the material

	bool init; ///< Flag indicating that the material has been initialized
	bool debug; ///< Flag indicating whether or not debug information should be printed
	bool needsConverted; ///< Flag indicating that the next value read from the file should be scaled by @a convFactor
	bool needsScaled; ///< Flag indicating that the next value read from the file should be scaled by @a scalingFactor
	bool newMaterial; ///< Flag indicating that the material pointed to by @a material was created locally and should be deleted

	double density; ///< The density of the material (in g/cm^3)
	double convFactor; ///< The current conversion factor to use for converting input values
	double scalingFactor; ///< The current scaling factor to use for scaling input values

	int nElements; ///< The number of elements which have been added to the material

	unsigned int valuesToRead; ///< The remaining number of values to read into the current vector

	G4VisAttributes *visatt; ///< Pointer to the Geant visual attributes for this material

	G4Material* material; ///< Pointer to the Geant material
	
	G4MaterialPropertiesTable* mpt; ///< Pointer to the Geant material properties table

	std::vector<double>* currArray; ///< Pointer to the current vector where values which are read from the file will be stored

	nDetMaterials* matHandler; ///< Pointer to the NEXTSim material handler
	
	std::map<std::string, std::vector<double> > variables; ///< Map which stores all user-defined value arrays

	std::map<std::string, double> elements; ///< Map which stores all elements which the user has added to the material

	std::map<std::string, size_t> validCommands; ///< Map which stores all valid input file commands and their required number of arguments

	/** Convert the input value by taking a unit into account
	  * @note This method uses G4UnitDefinition::GetValueOf() to obtain the conversion factor
	  *       for this value. If Geant does not recognize the input unit name, it will throw an error
	  * @param value The input value
	  * @param unit The name of the unit of the value
	  * @return The value converted to Geant standard units
	  */
	double unitConversion(const double &value, const std::string &unit);

	/** Convert a user input string to a bool value
	  * @param input The input string from the user
	  * @param flag The boolean flag to set
	  * @return True if the user input is either "true" or "false" and return false otherwise
	  */
	bool getBoolInput(const std::string &input, bool &flag);

	/** Decode an line from the user's input file
	  * @param args Vector of arguments obtained from a single input line
	  * @return True if the line is handled successfully, and return false otherwise
	  */
	bool decode(const std::vector<std::string> &args);

	/** Return true if an element with a given @a mName has already been added to the list and return false otherwise
	  */	
	bool isInElementMap(const std::string &mName);

	/** Return true if an array with a given @a mName has already been defined and return false otherwise
	  */	
	bool isInVariableMap(const std::string &mName);
};

#endif
