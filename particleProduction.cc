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

/*
General to-do:
  - Check compatibility with geant4
  - Update CMakeLists - I added some flags for GDML but I don't think those are needed any more
  - We should comment in the macro some commands which should NOT be included (beamOn, initialize)
  - I removed the commands allowing this to run in interactive mode, we should decide if we want to re-add 
  - Get external geometry viewer working
  - MiniBooNE -> PaleoSim naming
  - General testing, general clean up if we have includes that are no longer needed
  - Meta data stored output?
*/

int main(int argc, char** argv) {

	std::vector<std::string> vars = {
			"G4ENSDFSTATEDATA",
			"G4LEVELGAMMADATA",
			"G4RADIOACTIVEDATA",
			"G4NEUTRONHPDATA",
			"G4LEDATA",
			"G4PARTICLEXSDATA",
			"G4PIIDATA",
			"G4SAIDXSDATA",
			"G4ABLADATA",
			"G4REALSURFACEDATA",
		};
		for (const auto& var : vars) {
		const char* val = std::getenv(var.c_str());
		std::cout << var << ": " << (val ? val : "NOT SET") << std::endl;
	}
  
    // 1. RNG seeding
    // TODO: Check is this is the approach we want vs. seeds set in macro
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4long pid = getpid();
    time_t systime = time(nullptr);
    G4long seeds[2] = {systime, pid};
    G4Random::setTheSeeds(seeds, 0);

    // 2. Construct run manager
    auto* runManager = new G4RunManager();

    // 3. Messenger FIRST — must exist before reading macro
    // TODO: Add error checking function that runs after reading the macro
    // TODO: Consistency with variable names?
    // TODO: Do we want to generate outputTree enabled flags here (right now they're in the OutputManager)
    // TODO: Figure out how to automatically document maco based on the PaleoSimMessenger .cc and .hh. We 
    //       may need to add default arguments to the command objects in the .cc for this to work properly,
    //       which should already exist in the .hh
    // TODO: I'm not sure the optional/required flag for the commands is set right for every argument
    // TODO: Maybe we want a flag for exporting the geometry
    auto* messenger = new PaleoSimMessenger();

    // 4. Load macro BEFORE creating detector, output manager
    G4UImanager* ui = G4UImanager::GetUIpointer();
    if (argc != 2) {
      G4Exception("main", "NoMacro", FatalException, "Macro file is required.");
    }    
    G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + G4String(argv[1]));

    // 5. Create detector, register, create output manager
    // TODO: Change some names to make this more general. I still think there are generally only
    //       ~4 volumes we might want (1. world, which could be rock or air, 2. a cavity, which 
    //       is probably air but maybe we want flexibility, 3. a shielding volume, and 4. a target
    //       or crystal cell.
    // TODO: Add 4th volume--shielding
    // TODO: We might want to be able to position the target/shielding rather than generate at the origin
    // TODO: Add relevant materials definitions to MaterialsManager. "Standard rock" for example
    // TODO: Output geometry as VRML, as it doesn't require GEANT4 to be built with any visualization flags
    auto* detector       = new MiniBooNEBeamlineConstruction(*messenger);
    runManager->SetUserInitialization(detector);  

    // 6. Create output manager
    // TODO: Maybe we don't need the tree status flags here if we move them to messenger?
    // TODO: Do we want to add tracking of secondaries in the rock volume--bring back in code from main branch?
    // TODO: We should decide on standard units for all branches and stick to them
    // TODO: Fix neutron branches entering cavity (theta, multiplicity, origin, distance)
    auto* outputManager  = new PaleoSimOutputManager(*messenger);

    // 7. Physics list
    // TODO: Try different lists.
    // TODO: Set volume-specific tracking cuts (higher-grained tracking in the crystal/target). Determine
    //       where we hand off to SRIM
    runManager->SetUserInitialization(new PaleoSimPhysicsList());

    // 8. Register actions (via ActionInitialization). Generator is built in here.
    // TODO: Test cosmic muon sources speed
    // TODO: Document how to add in a custom source
    // TODO: MUTE generator
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
