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

// Removed unused analysis manager
// #include "MiniBooNEBeamlineAnalysis.hh"
// #include "G4AnalysisManager.hh"

#include "PaleoSimPhysicsList.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh" // Optional, depending on use

int main(int argc, char** argv)
{
    //Set the random generator
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    //Set the seeds for the run
    G4long pid = getpid();
    time_t systime = time(NULL);
    G4long seeds[2] = {systime, pid};
    G4Random::setTheSeeds(seeds, 0);
    G4cout<<"Seeds set to: "<<G4Random::getTheSeeds()[0]<<" "<<G4Random::getTheSeeds()[1]<<std::endl;
    
    // Construct the default run manager
    G4RunManager* runManager = new G4RunManager;

    //NOTE: Ordering of these is important!
    // Construction and PrimaryGenerators need to be created
    // before the Messenger. The Messenger needs to be created
    // before the initialization. The PhysicsList needs to be
    // instantiated before the PrimaryGenerator.

    // Detector construction
    G4cout << "Constructing the geometry..." << G4endl;
    auto* detector = new MiniBooNEBeamlineConstruction();
    runManager->SetUserInitialization(detector);
    
    //Physics list
    runManager->SetUserInitialization(new PaleoSimPhysicsList());

    // Generator
    G4cout << "Initializing the generator..." << G4endl;
    auto* generator = new MiniBooNEBeamlinePrimaryGeneratorAction();

    // Messenger
    auto* messenger = new PaleoSimMessenger(detector, generator);
    
    // Run Action
    runManager->SetUserInitialization(new MiniBooNEBeamlineActionInitialization(generator,detector));

    //Load up our macro
    G4UImanager* uiManager = G4UImanager::GetUIpointer();
    if (argc == 2) {
        G4String macroFile = argv[1];
        G4String command = "/control/execute " + macroFile;
        uiManager->ApplyCommand(command);
    }
    else {
        G4Exception("particleProduction", "inputErr001", FatalException,
            "Must supply a macro file!");
    }

    //Initialize the run
    runManager->Initialize();

    //Turn off the annoying "Track stuck" warnings. They appear harmless
    //and occur infrequently
    //G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->SetPushVerbosity(0);
    
    delete runManager;
    delete messenger;
    return 0;
}
