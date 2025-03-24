//Action Initialization definition

#ifndef MiniBooNEBeamlineActionInitialization_h
#define MiniBooNEBeamlineActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class MiniBooNEBeamlinePrimaryGeneratorAction;
class MiniBooNEBeamlineConstruction;

/// Action initialization class.

class MiniBooNEBeamlineActionInitialization : public G4VUserActionInitialization
{
  public:
    MiniBooNEBeamlineActionInitialization(MiniBooNEBeamlinePrimaryGeneratorAction* generator,
                                          MiniBooNEBeamlineConstruction* detector);
    virtual ~MiniBooNEBeamlineActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
  MiniBooNEBeamlinePrimaryGeneratorAction* fGenerator = nullptr;
  MiniBooNEBeamlineConstruction* fDetector = nullptr;
};

#endif

    
