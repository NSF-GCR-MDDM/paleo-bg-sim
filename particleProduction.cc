//Main Program
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4TransportationManager.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineActionInitialization.hh"
#include "Randomize.hh"
#include "time.h"
#include <unistd.h>

#include "PaleoSimPhysicsList.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"

int main(int argc, char** argv) {
    // 1. RNG seeding
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4long pid = getpid();
    time_t systime = time(nullptr);
    G4long seeds[2] = {systime, pid};
    G4Random::setTheSeeds(seeds, 0);

    // 2. Construct run manager
    auto* runManager = new G4RunManager();

    // 3. Messenger FIRST — must exist before reading macro
    auto* messenger = new PaleoSimMessenger();

    // 4. Load macro BEFORE creating detector, output manager
    G4UImanager* ui = G4UImanager::GetUIpointer();
    if (argc != 2) {
      G4Exception("main", "NoMacro", FatalException, "Macro file is required.");
    }    
    G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + G4String(argv[1]));

    // 5. Create detector, register, create output manager
    auto* detector       = new MiniBooNEBeamlineConstruction(*messenger);
    runManager->SetUserInitialization(detector);
    auto* outputManager  = new PaleoSimOutputManager(*messenger);

    // 6. Physics list
    runManager->SetUserInitialization(new PaleoSimPhysicsList());

    // 7. Register actions (via ActionInitialization)
    runManager->SetUserInitialization(new MiniBooNEBeamlineActionInitialization(*messenger, *outputManager));

    // 8. Initialize run manager AFTER all setup is complete
    runManager->Initialize();

    // 9. BeamOn loop (manual, from messenger)
    G4int nps = messenger->GetNPS();
    runManager->BeamOn(nps);

    // 10. Clean up
    delete runManager;
    delete messenger;
    return 0;
}
