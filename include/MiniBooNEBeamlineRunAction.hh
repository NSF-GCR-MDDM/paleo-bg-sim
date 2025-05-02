#ifndef MiniBooNEBeamlineRunAction_h
#define MiniBooNEBeamlineRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "PaleoSimOutputManager.hh"

class MiniBooNEBeamlineRunAction : public G4UserRunAction
{
  public:
    MiniBooNEBeamlineRunAction(PaleoSimOutputManager& manager);
    virtual ~MiniBooNEBeamlineRunAction() = default;

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
  private:
    PaleoSimOutputManager& fOutputManager;
};

#endif

