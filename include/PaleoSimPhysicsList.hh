#ifndef PALEOSIMPHYSICSLIST_HH
#define PALEOSIMPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"

class PaleoSimPhysicsList : public G4VModularPhysicsList {
public:
    PaleoSimPhysicsList();
    ~PaleoSimPhysicsList() override = default;
    
};

#endif

