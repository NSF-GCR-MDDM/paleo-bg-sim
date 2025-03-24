//Event Action

#ifndef MiniBooNEBeamlineEventAction_h
#define MiniBooNEBeamlineEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "PaleoSimSteppingAction.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"

class MiniBooNEBeamlineRunAction;
class MiniBooNEBeamlinePrimaryGeneratorAction;
class PaleoSimSteppingAction;

/// Event action class

class MiniBooNEBeamlineEventAction : public G4UserEventAction
{
  public:
    MiniBooNEBeamlineEventAction(MiniBooNEBeamlineRunAction* runAction,
                               MiniBooNEBeamlinePrimaryGeneratorAction* generator,
                               PaleoSimSteppingAction* steppingAction);
    virtual ~MiniBooNEBeamlineEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

  private:
    MiniBooNEBeamlineRunAction* fRunAction;
    MiniBooNEBeamlinePrimaryGeneratorAction* fGenerator; 
    PaleoSimSteppingAction* fSteppingAction; 
};

#endif

    
