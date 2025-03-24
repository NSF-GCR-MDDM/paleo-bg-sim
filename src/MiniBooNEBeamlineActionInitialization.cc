#include "MiniBooNEBeamlineActionInitialization.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineEventAction.hh"
#include "MiniBooNEBeamlineConstruction.hh"

// Constructor now accepts a pointer to the already-created generator and detector
MiniBooNEBeamlineActionInitialization::MiniBooNEBeamlineActionInitialization(
    MiniBooNEBeamlinePrimaryGeneratorAction* generator, 
    MiniBooNEBeamlineConstruction* detector)
 : G4VUserActionInitialization(),
   fGenerator(generator),
   fDetector(detector) 
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
  
  auto* runAction = new MiniBooNEBeamlineRunAction;
  SetUserAction(runAction);

  // Pass SteppingAction to EventAction
  auto* steppingAction = new PaleoSimSteppingAction(fDetector, fGenerator);  // Pass detector and generator
  SetUserAction(steppingAction);

  // Now pass the SteppingAction into EventAction
  auto* eventAction = new MiniBooNEBeamlineEventAction(runAction, fGenerator, steppingAction);
  SetUserAction(eventAction);
}