//Main Program
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4TransportationManager.hh"
#include "Randomize.hh"

#include "PaleoSimDetectorConstruction.hh"
#include "PaleoSimPrimaryGeneratorAction.hh"
#include "PaleoSimActionInitialization.hh"
#include "PaleoSimPhysicsList.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimGeometryMessenger.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimCmdLineParser.hh"
#include "TRandom.h"
#include "TROOT.h"

#include "time.h"
#include <unistd.h>

/*
To-do:
  - Consistency with variable names needs some cleaning up
  - Very minimal testing has been done. 
  - Maybe we should append units to output branches
  - Long-term goal: refactor messenger class to make less messy
  - Do we care about mu- vs. mu+ in meiHime & mute
  - Error checking on parameters for RGBA in geometry file
  - Check G4 MT codes
  - Check different physics lists
  - headerTree needs updating - include seed, more macro args
  - Could have a 'cellNum' argument for volumes, and then potentially track multiple cells with recoils, tallies, etc.,
    or store cellNum -> volumeName map in header tree
  - Compare with Geant4 Mei & hime simulation paper from--different settings?
  - Check MUTE code
  - Volumetric sampling
  - AmBe source defined
  - Energy deposition tree instead of lumping EM into recoil tree
  - Make sure secondaries tree/generator consistent with MT refactor
  - Add option for loading CRY file into memory (cmd line?)
  - Update documentation
  - Use existing GDML viewing? Was buggy before
  - Remove H5 stuff from CMake or figure out how to reimplement with parallel. I'd argue for an external converter.
    The main reason I wanted it was to READ h5 CRY files
*/

int main(int argc, char** argv) {
  ROOT::EnableThreadSafety();
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
    gRandom->SetSeed(seed);
  }
  else{
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4long pid = getpid();
    time_t systime = time(nullptr);
    G4long seed = (systime << 16) ^ pid;    
    std::cout<<"Random seed is "<<seed<<std::endl;
    G4Random::setTheSeed(seed);
    gRandom->SetSeed(seed);
  }
  
  //Threading
  G4int nThreads = 1;
  if (parsedArgs.find("--nThreads") != parsedArgs.end()) {
    nThreads = std::stoi(parsedArgs["--nThreads"]);
  }

  //
  std::string outputFilename = "";
  if (parsedArgs.find("--outputFile") != parsedArgs.end()) {
    outputFilename = parsedArgs["--outputFile"];
  }

  // 2. Construct run manager
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MTOnly);
  runManager->SetNumberOfThreads(nThreads);

  // 3. Messenger FIRST — must exist before reading macro
  auto* messenger = new PaleoSimMessenger();

  // 4a. Load main macro BEFORE creating detector
  G4UImanager* ui = G4UImanager::GetUIpointer();
  ui->ApplyCommand("/control/execute " + G4String(macroFilename));
  if (outputFilename != "") {
    messenger->SetOutputPath(outputFilename);
  }

  //4b. Load the geometry macro
  G4String geomMacro = messenger->GetGeometryMacroPath();
  auto* geoMessenger = new PaleoSimGeometryMessenger(messenger);
  if (!geomMacro.empty()) {
      G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + geomMacro);
  }
  else{
    G4Exception("main", "NoGeometryMacro", FatalException, "Geometry macro file is required.");
  }

  //5a. Check geometry for err
  geoMessenger->ValidateGeometry();

  //5b. Check for errors in macro files
  messenger->CheckForMacroErrors();

  // 6. Create detector, register
  auto* detector = new PaleoSimDetectorConstruction(*messenger);
  runManager->SetUserInitialization(detector);  

  // 7. Physics list
  runManager->SetUserInitialization(new PaleoSimPhysicsList(*messenger));

  // 8. Register actions (via ActionInitialization). Generator is built in here.
  runManager->SetUserInitialization(new PaleoSimActionInitialization(*messenger));

  // 9. Initialize run manager AFTER all setup is complete
  runManager->Initialize();  // or RunInitialization if directly used

  // 10. BeamOn loop (manual, from messenger)
  G4int nps = messenger->GetNPS();
  runManager->BeamOn(nps);

  // 11. Write geometry VRML if requested
  if (messenger->GetVRMLStatus()) { 
    G4String geoMacroPath = messenger->GetGeometryMacroPath();

    size_t dotPos = geoMacroPath.rfind(".");
    G4String vrmlFilename = (dotPos != G4String::npos)
        ? geoMacroPath.substr(0, dotPos) + ".wrl"
        : geoMacroPath + ".wrl";

    PaleoSimOutputManager outputManager(*messenger);
    outputManager.WriteVRMLGeometry(vrmlFilename);
  }

  // 12. Clean up
  delete runManager;
  delete messenger;
  return 0;
}