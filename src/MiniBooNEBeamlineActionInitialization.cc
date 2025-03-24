#include "MiniBooNEBeamlineActionInitialization.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineEventAction.hh"
#include "PaleoSimSteppingAction.hh"

// Constructor now accepts a pointer to the already-created generator and detector
MiniBooNEBeamlineActionInitialization::MiniBooNEBeamlineActionInitialization(
    PaleoSimMessenger& messenger,
    PaleoSimOutputManager* manager, 
    MiniBooNEBeamlinePrimaryGeneratorAction& generator)
 : G4VUserActionInitialization(),
   fMessenger(messenger),
   fOutputManager(manager),
   fGenerator(generator) 
{}
MiniBooNEBeamlineActionInitialization::~MiniBooNEBeamlineActionInitialization() {}

void MiniBooNEBeamlineActionInitialization::BuildForMaster() const
{
  auto* runAction = new MiniBooNEBeamlineRunAction;
  SetUserAction(runAction);
}

void MiniBooNEBeamlineActionInitialization::Build() const
{
  G4cout << "Registering Primary Generator Action..." << G4endl;
  SetUserAction(fGenerator);  // Use the generator passed in from main()
  
  auto* runAction = new MiniBooNEBeamlineRunAction(fOutputManager);
  SetUserAction(runAction);

  // Pass SteppingAction to EventAction
  auto* steppingAction = new PaleoSimSteppingAction(fMessenger, fOutputManager);  // Pass detector and generator
  SetUserAction(steppingAction);

  // Now pass the SteppingAction into EventAction
  auto* eventAction = new MiniBooNEBeamlineEventAction(fOutputManager);
  SetUserAction(eventAction);
}