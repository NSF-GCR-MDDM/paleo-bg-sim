#ifndef MiniBooNEBeamlineEventAction_h
#define MiniBooNEBeamlineEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "PaleoSimOutputManager.hh"

class MiniBooNEBeamlineEventAction : public G4UserEventAction
{
  public:
    MiniBooNEBeamlineEventAction(PaleoSimOutputManager& manager);
    virtual ~MiniBooNEBeamlineEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

  private:
    PaleoSimOutputManager& fOutputManager;
  
};

#endif