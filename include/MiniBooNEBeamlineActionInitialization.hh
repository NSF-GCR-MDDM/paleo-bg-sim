#ifndef MiniBooNEBeamlineActionInitialization_h
#define MiniBooNEBeamlineActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class PaleoSimMessenger;
class PaleoSimOutputManager;
class MiniBooNEBeamlinePrimaryGeneratorAction;

class MiniBooNEBeamlineActionInitialization : public G4VUserActionInitialization
{
  public:
    MiniBooNEBeamlineActionInitialization(PaleoSimMessenger& messenger,
                                          PaleoSimOutputManager& manager,
                                          MiniBooNEBeamlinePrimaryGeneratorAction& generator);
    virtual ~MiniBooNEBeamlineActionInitialization() = default;

    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fOutputManager;
    MiniBooNEBeamlinePrimaryGeneratorAction& fGenerator;
};

#endif

    
