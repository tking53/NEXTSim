
// $Id: ScintillatorHit.hh 12.16.2016 A. Fijalkowska $
//
/// \file ScintillatorHit.hh
/// \brief Definition of the ScintillatorHitHit class
//
//
#ifndef ScintillatorHit_h
#define ScintillatorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4LogicalVolume.hh"

class ScintillatorHit : public G4VHit
{
  public:
 
    ScintillatorHit(G4int moduleId, G4double enDep, G4double time, 
                    G4ThreeVector &position);
    virtual ~ScintillatorHit();
    ScintillatorHit(const ScintillatorHit &right);

    const ScintillatorHit& operator=(const ScintillatorHit &right);
    G4int operator==(const ScintillatorHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
 
    virtual void Draw();
    virtual void Print();
    
    void AddHit(G4double energyDep, G4double time, 
                G4ThreeVector &position);
    inline G4int GetModuleIndex() { return moduleIndex; }
    G4double GetTime(G4int hitNr);
	G4double GetEnergyDeposit(G4int hitNr);
	G4double GetxPos(G4int hitNr);
	G4double GetyPos(G4int hitNr);
	G4double GetzPos(G4int hitNr);
	inline G4int GetNrOfInteractions() {return nrOfInteract;}
	
	
  private:
    G4int nrOfInteract;
    G4int moduleIndex;
    std::vector<G4double> time;
    std::vector<G4double> energyDep;
    std::vector<G4double> xPos;
    std::vector<G4double> yPos;
    std::vector<G4double> zPos;

};

typedef G4THitsCollection<ScintillatorHit> ScintillatorHitsCollection;

extern G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator;

inline void* ScintillatorHit::operator new(size_t){
  if(!ScintillatorHitAllocator)
      ScintillatorHitAllocator = new G4Allocator<ScintillatorHit>;
  return (void *) ScintillatorHitAllocator->MallocSingle();
}

inline void ScintillatorHit::operator delete(void *aHit){
  ScintillatorHitAllocator->FreeSingle((ScintillatorHit*) aHit);
}

#endif

