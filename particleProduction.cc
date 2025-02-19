//Main Program


#include "G4RunManager.hh"
#include "G4UImanager.hh"
//#include "G4GDMLParser.hh"
#include "G4TransportationManager.hh"
#include "MiniBooNEBeamlineAnalysis.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "QGSP_INCLXX.hh"
#include "FTFP_INCLXX.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BIC.hh"
#include "FTFP_BERT.hh"
#include "MiniBooNEBeamlineActionInitialization.hh"
#include "Randomize.hh"
#include "time.h"
#include <unistd.h>


#ifdef G4UI_USE
        #include "G4VisExecutive.hh"
        #include "G4UIExecutive.hh"
#endif

int main(int argc, char** argv)
{
    //Set the random seed based on system time
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    //Set the seed in the macro so we can vary it for different jobs
    G4long pid = getpid();
    //G4long seed = 271;
    time_t systime = time(NULL);
    //G4long seeds[2] = (long) (systime*G4UniformRand()+pid);
    G4long seeds[2] = {systime, pid};
    G4Random::setTheSeeds(seeds, 0);

    //    TRandom3 randGen(0);
    // long seeds[2] = {randGen.Integer(INT_MAX), randGen.Integer(INT_MAX)};
    //G4Random::setTheSeeds(seeds, 0);
    //G4cout << "Random seeds set to: " << G4Random::getTheSeeds()[0] << " and " << G4Random::getTheSeeds()[1] << "\n";

    G4cout<<"Seeds set to: "<<G4Random::getTheSeeds()[0]<<" "<<G4Random::getTheSeeds()[1]<<endl;
    
    // Construct the default run manager
    G4RunManager* runManager = new G4RunManager;

    // Set mandatory initialization classes
    // Detector construction
    G4cout << "Constructing the geometry..." << G4endl;
    runManager->SetUserInitialization(new MiniBooNEBeamlineConstruction());
    G4cout << "Geometry Constructed!" << G4endl;

    // Physics list
    G4cout << "Loading the physics list..." << G4endl;
    //QGSP_INCLXX* physicsList = new QGSP_INCLXX();
    //FTFP_INCLXX* physicsList = new FTFP_INCLXX();
    QGSP_BERT* physicsList = new QGSP_BERT();
    //QGSP_BIC* physicsList = new QGSP_BIC();
    //FTFP_BERT* physicsList = new FTFP_BERT();
    runManager->SetUserInitialization(physicsList);
    G4cout << "Physics Loaded!" << G4endl;
    
    // User action initialization
    G4cout << "Setting the Action Initialization..." << G4endl;
    runManager->SetUserInitialization(new MiniBooNEBeamlineActionInitialization());
    G4cout << "Action Initialziation Complete!" << G4endl;

    //Initialize the run
    G4cout << "Initializing the run..." << G4endl;
    runManager->Initialize();
    G4cout << "Run Initialized!" << G4endl;

    //Turn this code on and off to write the geometry to a gdml file
    //For some reason it seems one cannot overwrite an old file so
    //this should only be used when changes are made and the new
    //geometry needs to be written. The old one must be deleted first
    //or the code will fail

    /*
    G4cout << "Writing the geometry to a .gdml file." << G4endl;
    G4VPhysicalVolume* pWorld = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
    G4GDMLParser parser;
    parser.Write("minibooneBeamline-NoHorn.gdml", pWorld);
    */

    //Turn off the annoying "Track stuck" warnings. They appear harmless
    //and occur infrequently
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->SetPushVerbosity(0);

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Process macro
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    //UImanager->ApplyCommand(command+fileName); //ALEXOCT_31_2024




    // Interactive session
    if (argc == 1) {
#ifdef G4UI_USE
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
      UImanager->ApplyCommand("/control/execute init.mac");
      ui->SessionStart();
      delete ui;
#endif
    }

    // Batch mode; process command line arguments + marco
    else {
      // Check for command line arguments (other than the macro)
      int argsLeft = argc - 1;
      G4cout << argsLeft << G4endl;
      while (argsLeft > 1) {
	G4String argFlag = argv[argc-argsLeft];
	if (argFlag == "-o") {
	  G4String anaFilename = argv[argc-argsLeft+1];
	  G4AnalysisManager::Instance()->SetFileName(anaFilename);
	  argsLeft -= 2;
	}
	G4cout << argsLeft << G4endl;
      }
      // Last argument should be the macro
      G4String command = "/control/execute ";
      G4String macName = argv[argc-1];
      UImanager->ApplyCommand(command+macName);
    }



    
    delete runManager;
    return 0;




}

