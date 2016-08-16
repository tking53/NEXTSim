#ifndef LENSLongLeptonPhysics_h
#define LENSLongLeptonPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4LowEnergyCompton.hh"
#include "G4LowEnergyPhotoElectric.hh"

#include "G4GammaConversion.hh"
#include "G4MultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuIonisation.hh"
#include "G4hIonisation.hh"

#include "G4MuonMinusCaptureAtRest.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpWLS.hh" 


class LENSLongLeptonPhysics : public G4VPhysicsConstructor
{
  public: 
    LENSLongLeptonPhysics(const G4String& name="Lepton");
    virtual ~LENSLongLeptonPhysics();

  public: 

    virtual void ConstructParticle();

    virtual void ConstructProcess();

  protected:


  G4LowEnergyPhotoElectric*	m_PhotoEffect;
  G4LowEnergyCompton*				m_ComptonEffect;
  G4GammaConversion*				m_PairProduction;


	G4Cerenkov* m_CerenkovRadiation;
	G4Scintillation* m_Scintillation;
	

  G4OpAbsorption* m_OpticalAbsorption;
  G4OpRayleigh* m_OpticalRayleighScattering;
  G4OpBoundaryProcess* m_OpticalBoundary;
  G4OpWLS* m_WLSProcess;

  G4MultipleScattering*	m_ElectronMultipleScattering;
  G4eIonisation*				m_ElectronIonisation;
  G4eBremsstrahlung*		m_ElectronBremsStrahlung;
  

  G4MultipleScattering*	m_PositronMultipleScattering;
  G4eIonisation*				m_PositronIonisation; 
  G4eBremsstrahlung*		m_PositronBremsStrahlung;  
  G4eplusAnnihilation*	m_PositronAnnihilation;


   G4MuIonisation*				m_MuPlusIonisation;
   G4MultipleScattering*	m_MuPlusMultipleScattering;
   G4MuBremsstrahlung*		m_MuPlusBremsstrahlung ;
   G4MuPairProduction*		m_MuPlusPairProduction;

   G4MuIonisation*				m_MuMinusIonisation;
   G4MultipleScattering*	m_MuMinusMultipleScattering;
   G4MuBremsstrahlung*		m_MuMinusBremsstrahlung ;
   G4MuPairProduction*		m_MuMinusPairProduction;

   G4MuonMinusCaptureAtRest* m_MuMinusCaptureAtRest;

   G4MultipleScattering*	m_TauPlusMultipleScattering;
   G4hIonisation*					m_TauPlusIonisation;

   G4MultipleScattering*	m_TauMinusMultipleScattering;
   G4hIonisation*					m_TauMinusIonisation;
};


#endif // LENSLongLeptonPhysics_h

