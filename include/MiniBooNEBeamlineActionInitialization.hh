#ifndef MiniBooNEBeamlineActionInitialization_h
#define MiniBooNEBeamlineActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"

class MiniBooNEBeamlineActionInitialization : public G4VUserActionInitialization
{
  public:
    MiniBooNEBeamlineActionInitialization(PaleoSimMessenger& messenger,PaleoSimOutputManager& manager);
    virtual ~MiniBooNEBeamlineActionInitialization() = default;

    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fOutputManager;
};

#endif

    
