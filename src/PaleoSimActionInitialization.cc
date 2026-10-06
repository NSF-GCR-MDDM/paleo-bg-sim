#include "PaleoSimActionInitialization.hh"
#include "PaleoSimPrimaryGeneratorAction.hh"
#include "PaleoSimRunAction.hh"
#include "PaleoSimEventAction.hh"
#include "PaleoSimSteppingAction.hh"


PaleoSimActionInitialization::PaleoSimActionInitialization(
    PaleoSimMessenger& messenger)
 : G4VUserActionInitialization(),
   fMessenger(messenger)
{}


void PaleoSimActionInitialization::BuildForMaster() const
{
  auto* runAction = new PaleoSimRunAction(fMessenger);
  SetUserAction(runAction);
}


void PaleoSimActionInitialization::Build() const
{
  G4cout << "Registering Primary Generator Action..." << G4endl;

  //Each worker RunAction owns its own OutputManager
  auto* runAction = new PaleoSimRunAction(fMessenger);
  SetUserAction(runAction);

  auto& outputManager = runAction->GetOutputManager();

  auto* generator = new PaleoSimPrimaryGeneratorAction(fMessenger, outputManager);
  SetUserAction(generator);

  auto* steppingAction = new PaleoSimSteppingAction(fMessenger, outputManager);
  SetUserAction(steppingAction);

  auto* eventAction = new PaleoSimEventAction(fMessenger, outputManager);
  SetUserAction(eventAction);
}