#ifndef PALEOSIMRUNACTION_HH
#define PALEOSIMRUNACTION_HH

#include <memory>

#include "G4UserRunAction.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"

class G4Run;

class PaleoSimRunAction : public G4UserRunAction {
public:
    PaleoSimRunAction(PaleoSimMessenger& messenger);
    ~PaleoSimRunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    PaleoSimOutputManager& GetOutputManager() { return *fOutputManager; }

private:
    std::unique_ptr<PaleoSimOutputManager> fOutputManager;
};

#endif