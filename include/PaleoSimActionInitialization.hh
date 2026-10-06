#ifndef PALEOSIMACTIONINITIALIZATION_HH
#define PALEOSIMACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"
#include "PaleoSimMessenger.hh"

class PaleoSimActionInitialization : public G4VUserActionInitialization
{
public:
    PaleoSimActionInitialization(PaleoSimMessenger& messenger);
    ~PaleoSimActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;

private:
    PaleoSimMessenger& fMessenger;
};

#endif