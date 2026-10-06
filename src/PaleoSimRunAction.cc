#include "PaleoSimRunAction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"


PaleoSimRunAction::PaleoSimRunAction(PaleoSimMessenger& messenger)
    : G4UserRunAction(),
      fOutputManager(std::make_unique<PaleoSimOutputManager>(messenger))
{
  fOutputManager->Book();
}


void PaleoSimRunAction::BeginOfRunAction(const G4Run*)
{
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  fOutputManager->BeginOfRun();
}


void PaleoSimRunAction::EndOfRunAction(const G4Run*)
{
  fOutputManager->EndOfRun();
}