#ifndef MiniBooNEBeamlineEventAction_h
#define MiniBooNEBeamlineEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimMessenger.hh"

class MiniBooNEBeamlineEventAction : public G4UserEventAction
{
  public:
    MiniBooNEBeamlineEventAction(PaleoSimMessenger& messenger,PaleoSimOutputManager& manager);
    virtual ~MiniBooNEBeamlineEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

  private:
  PaleoSimMessenger& fMessenger;
  PaleoSimOutputManager& fOutputManager;
  
};

#endif