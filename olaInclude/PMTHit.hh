
// $Id: PMTHit.hh 12.08.2016 A. Fijalkowska $
//
/// \file PMTHit.hh
/// \brief Definition of the PMTHit class
//
//
#ifndef PMTHit_h
#define PMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4LogicalVolume.hh"

class PMTHit : public G4VHit
{
  public:
 
    PMTHit(G4int moduleIndexVal, G4int pmtIndexVal, 
           G4double energyDepVal, G4double hitTimeVal);
    virtual ~PMTHit();
    PMTHit(const PMTHit &right);

    const PMTHit& operator=(const PMTHit &right);
    G4int operator==(const PMTHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
 
    virtual void Draw();
    virtual void Print();

    inline void SetDrawIt(G4bool b){drawIt=b;}
    inline G4bool GetDrawIt(){return drawIt;}
    
    inline void SetLogVol(G4LogicalVolume* val) { logVol = val; }
    
    void AddHit(G4double hitEnergyDep, G4double hitTime);
    inline G4int GetPMTIndex() { return pmtIndex; }
    inline G4int GetModuleIndex() { return moduleIndex; }
    G4double GetTime(G4int hitNr);
	G4double GetEnergyDeposit(G4int hitNr);
	inline G4int GetPhotonsCounts() {return hitsNr;}
	
	
  private:
    G4int hitsNr;
    G4int pmtIndex;
    G4int moduleIndex;
    std::vector<G4double> time;
    std::vector<G4double> energyDep;
    G4bool drawIt;
    G4LogicalVolume* logVol;

};

typedef G4THitsCollection<PMTHit> PMTHitsCollection;

extern G4ThreadLocal G4Allocator<PMTHit>* PMTHitAllocator;

inline void* PMTHit::operator new(size_t){
  if(!PMTHitAllocator)
      PMTHitAllocator = new G4Allocator<PMTHit>;
  return (void *) PMTHitAllocator->MallocSingle();
}

inline void PMTHit::operator delete(void *aHit){
  PMTHitAllocator->FreeSingle((PMTHit*) aHit);
}

#endif

