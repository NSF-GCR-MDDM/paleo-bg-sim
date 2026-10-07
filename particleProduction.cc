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
/*
To-do (prioritized):

P0 - Wrong results or crashes in current configs (fix before any production run):
  - MeiHime: fMuonEnergyDist->SetParameter(1, h_km) missing (removed in 727d468), so energies are sampled at zero depth.
    Add after h_km is computed, before GetRandom()
  - CRY: GeneratePrimaries never sets PaleoSimUserEventInformation (lost in 0fa1ef9). SteppingAction dereferences
    info for CRY when MIN or neutron tally trees are on (lines 59, 71, 138, 143) -> segfault (conus_cry.mac)
  - SteppingAction recoil tree uses postStepVolume (lines 207-212, 224, 271); energy deposits belong to preStepVolume,
    so ERs at boundaries are misattributed
  - SteppingAction line 248 early return skips prevVolume update (line 291) -> wrong prevVolumeNumbers in neutron
    tally when recoil tree is on
  - MeiHime angular sampling/normalization is inconsistent with generation from a horizontal top surface:
    p(theta) should be I_th(theta)*sin(theta)*cos(theta), which simplifies to
    (I1 e.. + I2 e..)*sin(theta). Horizontal-plane flux normalization should likewise use
    integral I_th(theta)*cos(theta)dOmega, not the angle-integrated Mei-Hime flux directly

P1 - Needed before running with >1 thread:
  - --nThreads is overridden by /run/numberOfThreads in macros (meiHime.mac line 20); remove from macros or apply
    cmd-line value after macro
  - Make ROOT random sampling consistent with MT-codes
    ROOT GetRandom() calls use global gRandom (MeiHime TF1, Disk/Volumetric TH1, MUTE TH3): data race with >1 thread
    and not reproducible from G4 seed. Avoid gRandom; make ROOT sampling use an event-local RNG derived from the
    Geant4 event RNG (e.g. per-event TRandom3 seeded from G4UniformRand(), passed to GetRandom(&rng))
  - Sources write to shared messenger from every worker: SetMeiHimeFluxNormalization, SetCRYAltitude/Latitude/Norm
    (SCT covered below). Restrict writes to one thread (e.g. IsMetadataThread helper)
  - Make sure secondaries tree/generator consistent with MT refactor
  - Verify merged MT output: run --nThreads 2, check tree->Print() shows vector<int>/vector<double> branches,
    and that SCT source and analysis scripts read the merged trees
  - Check G4 MT codes
  - Very minimal testing has been done

P2 - Latent bugs and provenance:
  - GeometryMessenger ComputeMissingCoordinates (lines 409, 412): absolute <-> relative position ignores parent
    rotation; wrong for off-center children of rotated volumes (affects point sampling, IsPointInside, geometry tree)
  - SCT capture boundary semantics: preStepVolume is the volume just entered, not the one left. Verify intended behavior;
    if capture is supposed to represent crossings between the two listed volumes, use prevVolume for the volume being left
  - Check MUTE code
  - headerTree needs updating - include seed, more macro args
  - Minor: ~PaleoSimPrimaryGeneratorAction doesn't delete fMuteSource; GenerateRandomPointInside gives false fatal
    error if 1000th try succeeds; CmdLineParser silently drops values starting with '-' (e.g. negative seeds)

P3 - Physics validation:
  - Compare with Geant4 Mei & hime simulation paper from--different settings?
  - Check different physics lists - Are we tracking correctly?
  - Do we care about mu- vs. mu+ in meiHime & mute

P4 - Features:
  - Energy deposition tree instead of lumping EM into recoil tree
  - Add option for loading CRY file into memory (cmd line?)
  - Volumetric sampling
  - AmBe source defined
  - Could have a 'cellNum' argument for volumes, and then potentially track multiple cells with recoils, tallies, etc.,
    or store cellNum -> volumeName map in header tree
  - Maybe we should append units to output branches
  - Error checking on parameters for RGBA in geometry file
  - Use existing GDML viewing? Was buggy before

P5 - Cleanup and long-term:
  - Remove H5 stuff from CMake or figure out how to reimplement with parallel. I'd argue for an external converter.
    The main reason I wanted it was to READ h5 CRY files
  - Update documentation
  - Consistency with variable names needs some cleaning up
  - Long-term goal: refactor messenger class to make less messy
*/
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