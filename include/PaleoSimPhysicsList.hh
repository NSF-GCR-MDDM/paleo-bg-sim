#ifndef PHYSICS_LIST_HH
#define PHYSICS_LIST_HH

#include "G4VModularPhysicsList.hh"

class PaleoSimPhysicsList : public G4VModularPhysicsList {
public:
    PaleoSimPhysicsList();
    ~PaleoSimPhysicsList() override = default;
    
};

#endif

