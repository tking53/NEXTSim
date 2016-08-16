#ifndef LENSLongPhysicsList_h
#define LENSLongPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

class LENSLongPhysicsList: public G4VUserPhysicsList
{
  public:
    LENSLongPhysicsList();
    ~LENSLongPhysicsList();

  protected:
	virtual void ConstructParticle();
	virtual void ConstructProcess();

    virtual void SetCuts();

    G4int OpVerbLevel;

	void ConstructBosons();
	void ConstructLeptons();
	void ConstructMesons();
	void ConstructBaryons();

	virtual void ConstructGeneral();
	virtual void ConstructEM();
        virtual void ConstructHad();
        virtual void ConstructOp();
};

#endif // LENSLongPhysicsList_h

