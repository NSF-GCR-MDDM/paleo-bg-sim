//Action Initialization definition

#ifndef MiniBooNEBeamlineActionInitialization_h
#define MiniBooNEBeamlineActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class MiniBooNEBeamlineActionInitialization : public G4VUserActionInitialization
{
  public:
    MiniBooNEBeamlineActionInitialization();
    virtual ~MiniBooNEBeamlineActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif

    
