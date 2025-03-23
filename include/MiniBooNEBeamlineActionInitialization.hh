//Action Initialization definition

#ifndef MiniBooNEBeamlineActionInitialization_h
#define MiniBooNEBeamlineActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class MiniBooNEBeamlinePrimaryGeneratorAction;

/// Action initialization class.

class MiniBooNEBeamlineActionInitialization : public G4VUserActionInitialization
{
  public:
    MiniBooNEBeamlineActionInitialization(MiniBooNEBeamlinePrimaryGeneratorAction* generator);
    virtual ~MiniBooNEBeamlineActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
      MiniBooNEBeamlinePrimaryGeneratorAction* fGenerator = nullptr;
};

#endif

    
