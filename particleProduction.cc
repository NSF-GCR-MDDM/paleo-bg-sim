//Main Program
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4TransportationManager.hh"
#include "Randomize.hh"
#include "QGSP_BERT_HP.hh"
#include "QGSP_BIC_HP.hh"
#include "PaleoSimDetectorConstruction.hh"
#include "PaleoSimPrimaryGeneratorAction.hh"
#include "PaleoSimActionInitialization.hh"
#include "PaleoSimPhysicsList.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimGeometryMessenger.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimCmdLineParser.hh"

#include "time.h"
#include <unistd.h>

/*
To-do:
  - Consistency with variable names needs some cleaning up
  - Very minimal testing has been done. 
  - Maybe we should append units to output branches? Or be consistent with units
  - Implement CRY generator
  - Long-term goal: Generators should be in their own pieces of code, in a generators folder or something
  - Do we care about mu- vs. mu+ in meiHime & mute
  - Error checking on parameters for RGBA in geometry file
  - Check G4 MT codes
  - Try different physics lists
  - headerTree needs updating
  - We could store the geometry file in the tree--is that with GDML only? nEXO does this
  - Could have a 'cellNum' argument for volumes, and then potentially track multiple cells with recoils, tallies, etc.,
    or store cellNum -> volumeName map in header tree
  - Compare with Geant4 Mei & hime simulation paper from--different settings?
  - Mute generator should have an argument for whether passed in file is energy theta or energy theta phi, and needs sampling adjusted as well
  - Volumetric sampling
  - AmBe source defined
  - Energy deposition tree
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

  //0. Parse command line
  auto parsedArgs = PaleoSimCommandLine::Parse(argc, argv);

  //Macro file
  std::string macroFilename;
  if (parsedArgs.find("macro") != parsedArgs.end()) {
      macroFilename = parsedArgs["macro"];
  } else {
      G4Exception("main", "NoMacro", FatalException, "Macro file is required.");
  }
  
  // 1. Deal with command line arguments
  if (parsedArgs.find("--seed") != parsedArgs.end()) {
    G4long seed = std::stol(parsedArgs["--seed"]);
    std::cout<<"Random seed is "<<seed<<std::endl;
    G4Random::setTheSeed(seed);
  }
  else{
      G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4long pid = getpid();
    time_t systime = time(nullptr);
    G4long seed = (systime << 16) ^ pid;    
    std::cout<<"Random seed is "<<seed<<std::endl;
    G4Random::setTheSeed(seed);
  }
  
  //
  std::string outputFilename = "";
  if (parsedArgs.find("--outputFile") != parsedArgs.end()) {
    outputFilename = parsedArgs["--outputFile"];
  }

  // 2. Construct run manager
  auto* runManager = new G4RunManager();

  // 3. Messenger FIRST — must exist before reading macro
  auto* messenger = new PaleoSimMessenger();

  // 4a. Load macro BEFORE creating detector, output manager
  G4UImanager* ui = G4UImanager::GetUIpointer();
  ui->ApplyCommand("/control/execute " + G4String(macroFilename));
  if (outputFilename != "") {
    messenger->SetOutputPath(outputFilename);
  }

  //4b. Load the geometry macro
  // If user set a geometry macro, load it now
  G4String geomMacro = messenger->GetGeometryMacroPath();
  auto* geoMessenger = new PaleoSimGeometryMessenger(messenger);
  if (!geomMacro.empty()) {
      G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + geomMacro);
  }
  else{
    G4Exception("main", "NoGeometryMacro", FatalException, "Geometry macro file is required.");
  }

  //4c. Check for errors in macro files
  messenger->CheckForMacroErrors();
  messenger->ComputeCoordinates();


  // 5. Create detector, register, create output manager
  auto* detector       = new PaleoSimDetectorConstruction(*messenger);
  runManager->SetUserInitialization(detector);  

  // 6. Create output manager
  auto* outputManager  = new PaleoSimOutputManager(*messenger);

  // 7. Physics list
  //Josh commented this out for Pranav and added the two lines below:
  // runManager->SetUserInitialization(new PaleoSimPhysicsList());
  QGSP_BIC_HP* physicsList = new QGSP_BIC_HP();
  runManager->SetUserInitialization(physicsList);
  
  // 8. Register actions (via ActionInitialization). Generator is built in here.
  runManager->SetUserInitialization(new PaleoSimActionInitialization(*messenger, *outputManager));

  // 8. Initialize run manager AFTER all setup is complete
  runManager->Initialize();  // or RunInitialization if directly used

  // 9. BeamOn loop (manual, from messenger)
  G4int nps = messenger->GetNPS();
  runManager->BeamOn(nps);

  // 10. Write geometry if requested
  if (messenger->GetVRMLStatus()) { 
    G4String geoMacroPath = messenger->GetGeometryMacroPath();

    size_t dotPos = geoMacroPath.rfind(".");
    G4String vrmlFilename = (dotPos != G4String::npos)
        ? geoMacroPath.substr(0, dotPos) + ".wrl"
        : geoMacroPath + ".wrl";

    outputManager->WriteVRMLGeometry(vrmlFilename);
  }

  // 11. Clean up
  delete runManager;
  delete messenger;
  return 0;
}
