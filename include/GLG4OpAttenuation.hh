#ifndef GLG4OpAttenuation_h
#define GLG4OpAttenuation_h 1


#include "G4OpAbsorption.hh"


class GLG4OpAttenuation : public G4OpAbsorption
{

private:


public: 

        GLG4OpAttenuation(const G4String& processName = "Attenuation");


	~GLG4OpAttenuation();


public: 

	G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
 				        const G4Step&  aStep);


};

#endif /* GLG4OpAttenuation_h */
