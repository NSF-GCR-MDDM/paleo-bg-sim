//Event Action

#ifndef MiniBooNEBeamlineEventAction_h
#define MiniBooNEBeamlineEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class MiniBooNEBeamlineRunAction;

/// Event action class

class MiniBooNEBeamlineEventAction : public G4UserEventAction
{
  public:
    MiniBooNEBeamlineEventAction(MiniBooNEBeamlineRunAction* runAction);
    virtual ~MiniBooNEBeamlineEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

  private:
    MiniBooNEBeamlineRunAction* fRunAction;
};

#endif

    
