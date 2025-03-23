#include "MiniBooNEBeamlineRunAction.hh"
#include "PaleoSimOutputManager.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

MiniBooNEBeamlineRunAction::MiniBooNEBeamlineRunAction()
 : G4UserRunAction()
{
  // No histogram or G4AnalysisManager setup needed anymore
}

MiniBooNEBeamlineRunAction::~MiniBooNEBeamlineRunAction()
{
  // No deletion needed — PaleoSimOutputManager handles itself
}

void MiniBooNEBeamlineRunAction::BeginOfRunAction(const G4Run*)
{
  // Disable random seed saving
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Initialize output trees (optionally you can detect geometry if needed)
  PaleoSimOutputManager::Get().Initialize(/* hasTallyVolume = */ true, 
                                          /* hasDepositionsVolume = */ true);
}

void MiniBooNEBeamlineRunAction::EndOfRunAction(const G4Run* run)
{
  if (run->GetNumberOfEvent() == 0) return;

  // Write and close ROOT file
  PaleoSimOutputManager::Get().WriteAndClose();
}
