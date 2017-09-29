//
// Created by David Pérez Loureiro on 5/19/17.
//

#ifndef NEXTMULTIPLESCATTERING_NDETPHYSICSLIST_HH
#define NEXTMULTIPLESCATTERING_NDETPHYSICSLIST_HH

#endif //NEXTMULTIPLESCATTERING_NDETPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"

#include "QGSP_BERT_HP.hh"

class nDetPhysicsList: public G4VModularPhysicsList {

public:
    nDetPhysicsList();
    ~nDetPhysicsList();
};