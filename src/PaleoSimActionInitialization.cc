#include "PaleoSimActionInitialization.hh"
#include "PaleoSimPrimaryGeneratorAction.hh"
#include "PaleoSimRunAction.hh"
#include "PaleoSimEventAction.hh"
#include "PaleoSimSteppingAction.hh"

// Constructor now accepts a pointer to the already-created generator and detector
PaleoSimActionInitialization::PaleoSimActionInitialization(
    PaleoSimMessenger& messenger,
    PaleoSimOutputManager& manager)
 : G4VUserActionInitialization(),
   fMessenger(messenger),
   fOutputManager(manager)
{}

void PaleoSimActionInitialization::BuildForMaster() const
{
  auto* runAction = new PaleoSimRunAction(fOutputManager);
  SetUserAction(runAction);
}

void PaleoSimActionInitialization::Build() const
{
  G4cout << "Registering Primary Generator Action..." << G4endl;

  auto* generator = new PaleoSimPrimaryGeneratorAction(fMessenger, fOutputManager);
  SetUserAction(generator);  // Use the generator passed in from main()
  
  auto* runAction = new PaleoSimRunAction(fOutputManager);
  SetUserAction(runAction);

  // Pass SteppingAction to EventAction
  auto* steppingAction = new PaleoSimSteppingAction(fMessenger, fOutputManager);  // Pass detector and generator
  SetUserAction(steppingAction);

  // Now pass the SteppingAction into EventAction
  auto* eventAction = new PaleoSimEventAction(fMessenger, fOutputManager);
  SetUserAction(eventAction);
}