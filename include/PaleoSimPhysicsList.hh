#ifndef PALEOSIMPHYSICSLIST_HH
#define PALEOSIMPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"
#include "PaleoSimMessenger.hh"

class PaleoSimPhysicsList : public G4VModularPhysicsList {
    public:
        PaleoSimPhysicsList(PaleoSimMessenger& messenger);
        ~PaleoSimPhysicsList() override = default;
        
        void SetCuts() override;
    private:
        PaleoSimMessenger& fMessenger;
};

#endif

