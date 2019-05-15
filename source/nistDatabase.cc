
#include <iostream>

#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"

#include "nistDatabase.hh"
#include "termColors.hh"

nistDatabase::nistDatabase(){
	G4NistManager *manNist = G4NistManager::Instance();
	elements = manNist->GetNistElementNames();
	materials = manNist->GetNistMaterialNames();
}

size_t nistDatabase::getNumberElements() const {
	return elements.size();
}

size_t nistDatabase::getNumberMaterials() const {
	return materials.size();
}

void nistDatabase::printElements() const {
	for(size_t i = 0; i < elements.size(); i++){
		std::cout << i << "\t" << elements.at(i) << std::endl;
	}
}

void nistDatabase::printMaterials() const {
	for(size_t i = 0; i < materials.size(); i++){
		std::cout << i << "\t" << materials.at(i) << std::endl;
	}
}

size_t nistDatabase::searchElementList(const G4String &name) const {
	size_t count = 0;
	for(size_t i = 0; i < elements.size(); i++){
		if(elements.at(i).find(name) != std::string::npos){
			std::cout << "  " << elements.at(i) << std::endl;
			count++;
		}
	}
	return count;
}

size_t nistDatabase::searchMaterialList(const G4String &name) const {
	size_t count = 0;
	for(size_t i = 0; i < materials.size(); i++){
		if(materials.at(i).find(name) != std::string::npos){
			std::cout << "  " << materials.at(i) << std::endl;
			count++;
		}
	}
	return count;
}

G4Element *nistDatabase::searchForElement(const G4String &name) const {
	G4Element *retval = G4NistManager::Instance()->FindOrBuildElement(name);
	if(!retval)
		Display::ErrorPrint("Failed to find element named \""+name+" in NIST database.", "nistDatabase");
	else
		std::cout << "nistDatabase: Successfully found element named \"" << name << "\" in NIST database.\n";
	return retval;
}

G4Material *nistDatabase::searchForMaterial(const G4String &name) const {
	G4Material *retval = G4NistManager::Instance()->FindOrBuildMaterial(name);
	if(!retval)
		Display::ErrorPrint("Failed to find material named \""+name+" in NIST database.", "nistDatabase");
	else
		std::cout << "nistDatabase: Successfully found material named \"" << name << "\" in NIST database.\n";
	return retval;
}
