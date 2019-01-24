
#include <vector>

#include "globals.hh"

class G4Element;
class G4Material;

class nistDatabase{
  public:
	nistDatabase();

	size_t getNumberElements() const ;
	
	size_t getNumberMaterials() const ;

	void printElements() const ;
	
	void printMaterials() const ;

	size_t searchElementList(const G4String &name) const ;
	
	size_t searchMaterialList(const G4String &name) const ;
	
	G4Element *searchForElement(const G4String &name) const ;
	
	G4Material *searchForMaterial(const G4String &name) const ;

  private:
	std::vector<G4String> elements;
	std::vector<G4String> materials;
};
