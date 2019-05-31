#ifndef NIST_DATABASE_HH
#define NIST_DATABASE_HH

#include <vector>

#include "globals.hh"

class G4Element;
class G4Material;

/*! \class nistDatabase
 *  \brief Contains lists of all elements and materials defined in the Geant4 NIST database
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class nistDatabase{
  public:
	/** Default constructor
	  */
	nistDatabase();

	/** Get the number of elements defined in the database
	  */
	size_t getNumberElements() const ;

	/** Get the number of materials defined in the database
	  */
	size_t getNumberMaterials() const ;

	/** Print all database elements to stdout
	  */
	void printElements() const ;

	/** Print all database materials to stdout
	  */	
	void printMaterials() const ;

	/** Search for elements containing a search substring
	  * @param name Search string
	  * @return The number of elements whose name contain the search string
	  */
	size_t searchElementList(const G4String &name) const ;
	
	/** Search for materials containing a search substring
	  * @param name Search string
	  * @return The number of elements whose name contain the search string
	  */
	size_t searchMaterialList(const G4String &name) const ;

	/** Search for an element in the database
	  * @param name Name of the NIST element
	  * @return A pointer to the Geant4 element if it was found and return 0x0 otherwise
	  */
	G4Element *searchForElement(const G4String &name) const ;
	
	/** Search for a material in the database
	  * @param name Name of the NIST material
	  * @return A pointer to the Geant4 material if it was found and return 0x0 otherwise
	  */
	G4Material *searchForMaterial(const G4String &name) const ;

  private:
	std::vector<G4String> elements; ///< Vector of NIST element names
	std::vector<G4String> materials; ///< Vector of NISt material names
};

#endif
