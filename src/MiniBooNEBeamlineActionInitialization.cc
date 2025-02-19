//Action Intialization

#include "MiniBooNEBeamlineActionInitialization.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineEventAction.hh"
#include "MiniBooNEBeamlineTrackingAction.hh"


MiniBooNEBeamlineActionInitialization::MiniBooNEBeamlineActionInitialization()
 : G4VUserActionInitialization()
{}

MiniBooNEBeamlineActionInitialization::~MiniBooNEBeamlineActionInitialization()
{}

void MiniBooNEBeamlineActionInitialization::BuildForMaster() const
{
  MiniBooNEBeamlineRunAction* runAction = new MiniBooNEBeamlineRunAction;
  SetUserAction(runAction);
}

void MiniBooNEBeamlineActionInitialization::Build() const
{
  SetUserAction(new MiniBooNEBeamlinePrimaryGeneratorAction);

  MiniBooNEBeamlineRunAction* runAction = new MiniBooNEBeamlineRunAction;
  SetUserAction(runAction);
  
  MiniBooNEBeamlineEventAction* eventAction = new MiniBooNEBeamlineEventAction(runAction);
  SetUserAction(eventAction);

  MiniBooNEBeamlineTrackingAction* trackingAction = new MiniBooNEBeamlineTrackingAction(runAction);
  SetUserAction(trackingAction);
}  

