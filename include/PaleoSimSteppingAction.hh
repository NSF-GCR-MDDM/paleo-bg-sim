#ifndef PALEOSIMSTEPPINGACTION_HH
#define PALEOSIMSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

#include "PaleoSimMessenger.hh" 
#include "PaleoSimOutputManager.hh" 

class PaleoSimSteppingAction : public G4UserSteppingAction {
public:
    PaleoSimSteppingAction(PaleoSimMessenger& messenger, 
                           PaleoSimOutputManager& manager);
    virtual ~PaleoSimSteppingAction();

    virtual void UserSteppingAction(const G4Step* step) override;
    
private:
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fOutputManager;
    
    G4int MapProcessToMT(G4int processType, G4int processSubType);
};

#endif
