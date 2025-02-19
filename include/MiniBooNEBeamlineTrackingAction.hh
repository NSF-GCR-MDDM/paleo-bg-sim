//Tracking Action

#ifndef MiniBooNEBeamlineTrackingAction_h
#define MiniBooNEBeamlineTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class MiniBooNEBeamlineRunAction;

/// Tracking action class

class MiniBooNEBeamlineTrackingAction : public G4UserTrackingAction
{
  public:
    MiniBooNEBeamlineTrackingAction(MiniBooNEBeamlineRunAction* runAction);
    virtual ~MiniBooNEBeamlineTrackingAction();

    virtual void PreUserTrackingAction(const G4Track* track);
    virtual void PostUserTrackingAction(const G4Track* track);

  private:
    MiniBooNEBeamlineRunAction* fRunAction;
    G4ThreeVector fMgInitialPosition;
    G4ThreeVector fFeInitialPosition;
    G4ThreeVector fSiInitialPosition;
    G4ThreeVector fOInitialPosition;
};

#endif

    
