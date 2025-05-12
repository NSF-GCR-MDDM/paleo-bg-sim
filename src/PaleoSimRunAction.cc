#include "PaleoSimRunAction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

PaleoSimRunAction::PaleoSimRunAction(PaleoSimOutputManager& manager)
 : G4UserRunAction(), fOutputManager(manager)
{ }

void PaleoSimRunAction::BeginOfRunAction(const G4Run*)
{
  // Disable random seed saving
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Initialize output trees
  fOutputManager.CreateOutputFileAndTrees();
}

void PaleoSimRunAction::EndOfRunAction(const G4Run* run)
{
  if (run->GetNumberOfEvent() == 0) return;

  // Write and close ROOT file
  fOutputManager.WriteAndClose();
}
