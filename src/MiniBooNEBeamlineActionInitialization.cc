//Action Intialization

#include "MiniBooNEBeamlineActionInitialization.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineEventAction.hh"
#include "MiniBooNEBeamlineTrackingAction.hh"


// Constructor now accepts a pointer to the already-created generator
MiniBooNEBeamlineActionInitialization::MiniBooNEBeamlineActionInitialization(
    MiniBooNEBeamlinePrimaryGeneratorAction* generator)
 : G4VUserActionInitialization(),
   fGenerator(generator)
{}
MiniBooNEBeamlineActionInitialization::~MiniBooNEBeamlineActionInitialization()
{}

void MiniBooNEBeamlineActionInitialization::BuildForMaster() const
{
  auto* runAction = new MiniBooNEBeamlineRunAction;
  SetUserAction(runAction);
}

void MiniBooNEBeamlineActionInitialization::Build() const
{
  G4cout << "Registering Primary Generator Action..." << G4endl;
  SetUserAction(fGenerator);  // Use the generator passed in from main()

  auto* runAction = new MiniBooNEBeamlineRunAction;
  SetUserAction(runAction);

  auto* eventAction = new MiniBooNEBeamlineEventAction(runAction);
  SetUserAction(eventAction);

  auto* trackingAction = new MiniBooNEBeamlineTrackingAction(runAction);
  SetUserAction(trackingAction);
}
