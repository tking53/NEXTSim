#ifndef NDET_MATERIALS_HH
#define NDET_MATERIALS_HH

#include "globals.hh"

#include "nistDatabase.hh"

class G4Element;
class G4Material;
class G4MaterialPropertiesTable;
class G4OpticalSurface;

class nDetMaterials{
  public:
    // Materials and elements
    G4Element* fH; ///< Hydrogen
    G4Element* fC; ///< Carbon
    G4Element* fO; ///< Oxygen
    G4Element* fF; ///< Flourine
    G4Element* fSi; ///< Silicon
    G4Element* fAl; ///< Aluminium

    G4Material* fAir; ///< Material corresponding to air
    G4Material* fVacuum; ///< Material corresponding to natural vacuum
    G4Material* fTeflon; ///< Material corresponding to teflon
    G4Material* fEJ200; ///< Material corresponding to EJ-200 scintillator
    G4Material* fEJ276; ///< Material corresponding to EJ-276 scintillator
    G4Material* fGrease; ///< Material corresponding to optical grease
    G4Material* fSiO2; ///< Material corresponding to quartz
    G4Material* fSilicon; ///< Material corresponding to silicon
    G4Material* fMylar; ///< Material corresponding to aluminized mylar
    G4Material* fAcrylic; ///< Material corresponding to acrylic
	G4Material* fAluminum; ///< Material corresponding to aluminum

    // Material table properties
    G4MaterialPropertiesTable* fAirMPT; ///< Material properties table for air
    G4MaterialPropertiesTable* fTeflonMPT; ///< Material properties table for teflon
    G4MaterialPropertiesTable* fEJ200MPT; ///< Material properties table for EJ-200 scintillator
    G4MaterialPropertiesTable* fEJ276MPT; ///< Material properties table for EJ-276 scintillator
    G4MaterialPropertiesTable* fGreaseMPT; ///< Material properties table for optical grease
    G4MaterialPropertiesTable* fSiO2MPT; ///< Material properties table for quartz
    G4MaterialPropertiesTable* fSiliconMPT; ///< Material properties table for silicon
	G4MaterialPropertiesTable* fMylarMPT; ///< Material properties table for aluminized mylar
	G4MaterialPropertiesTable* fPerfectMPT; ///< Material properties table for a perfect reflector
	G4MaterialPropertiesTable* fAluminumMPT; ///< Material properties table for aluminum
	G4MaterialPropertiesTable* fEsrMPT; ///< Material properties table for 3M Enhanced Specular Reflector

    // Optical Surfaces
    G4OpticalSurface* fTeflonOpSurf; ///< Optical surface for teflon
    G4OpticalSurface* fSiliconOpSurf; ///< Optical surface for silicon
    G4OpticalSurface* fMylarOpSurf; ///< Optical surface for aluminized mylar
    G4OpticalSurface* fEsrOpSurf; ///< Optical surface for 3M Enhanced Specular Reflector
	G4OpticalSurface* fPerfectOpSurf; ///< Optical surface for a perfect reflector
	G4OpticalSurface* fGreaseOpSurf; ///< Optical surface for optical grease
	
	/** Default constructor
	  */
	nDetMaterials() : isInitialized(false), scintsAreDefined(false), lightYieldScale(1) { }
	
	/** Destructor
	  */
	~nDetMaterials();

	/** Define all materials, scintillators, and optical surfaces
	  */
	void initialize();

	/** Return true if all materials and surfaces are defined and return false otherwise
	  */
	bool materialsAreDefined() const { return isInitialized; }

	/** Get a pointer to the scintillator material corresponding to @a name
	  */
	G4Material* getUserDetectorMaterial(const G4String &name);

	/** Get a pointer to the surface material corresponding to @a name
	  */
	G4Material* getUserSurfaceMaterial(const G4String &name);

	/** Get a pointer to the optical surface corresponding to @a name
	  */
	G4OpticalSurface* getUserOpticalSurface(const G4String &name);

	/** Set the light yield multiplier for scintillator photon production
	  * @param yield The fraction of the light yield to use for optical photon production in scintillators (default is 1)
	  */	
	void setLightYield(const G4double &yield);

	/** Search for a material name in the NIST database
	  */
	G4Material* searchForMaterial(const G4String &name);

  private:
	bool isInitialized; ///< Flag indicating that all materials have been defined
	bool scintsAreDefined; ///< Flag indicating that the scintillator materials have been defined
	
    G4double lightYieldScale; ///< Scaling parameter used for scintillation light yield (default=1)	

	nistDatabase nist; ///< Database for pre-defined NIST element and material lookups

	/** Define materials and optical surfaces
	  */
	void defineMaterials();

	/** Define scintillator materials EJ200 and EJ276
	  */
	void defineScintillators();
};
	
#endif
