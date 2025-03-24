//Geometry specification
#include "MiniBooNEBeamlineConstruction.hh"
#include "PaleoSimMaterialManager.hh"
#include "PaleoSimMessenger.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#ifdef PALEOSIM_ENABLE_GDML
    #include "G4GDMLParser.hh"
#endif


MiniBooNEBeamlineConstruction::MiniBooNEBeamlineConstruction(PaleoSimMessenger& messenger)
: G4VUserDetectorConstruction(),
  fMessenger(messenger)
{
    fMaterialManager = new PaleoSimMaterialManager();
}

MiniBooNEBeamlineConstruction::~MiniBooNEBeamlineConstruction()
{ 
    delete fMaterialManager;
}

G4VPhysicalVolume* MiniBooNEBeamlineConstruction::Construct()
{  
    G4bool checkOverlaps = true; // Prints if there are overlapping volumes

    //Rock overburden
    G4String overburdenMaterialName = fMessenger->GetUserOverburdenMaterial();
    G4cout << "Overburden material is = " << overburdenMaterialName << G4endl;
    G4Material* overburdenMaterial = fMaterialManager->GetMaterial(overburdenMaterialName); 
    G4double overburdenSideLength = fMessenger->GetUserOverburdenSideLength();
    G4cout << "Overburden side length = " << overburdenSideLength << G4endl;
    G4double halfOverburdenLength = 0.5 * overburdenSideLength;
    G4Box* rockBox = new G4Box("rockBox",halfOverburdenLength,halfOverburdenLength,halfOverburdenLength);
    G4LogicalVolume* logicRock = new G4LogicalVolume(rockBox,overburdenMaterial,"rockBox");

    G4VPhysicalVolume* physWorld = new G4PVPlacement(
        nullptr,              // No rotation
        G4ThreeVector(0,0,0),      // Centered at origin
        logicRock,            // Logical volume
        "RockBox",            // Name
        nullptr,              // Mother volume (this is world)
        false,                // No boolean operations
        0,                    // Copy number
        checkOverlaps         // Check for overlaps
    );

    //Air cavity
    G4LogicalVolume* logicCavity = nullptr; //Create outside if statement so can be used in next conditional
    //for target w/o creating compile errors
    if (fMessenger->GetAirCavitySideLength() > 0.0) {
      G4Material* cavityMaterial = fMaterialManager->GetMaterial("Air"); 
      G4double airCavitySideLength = fMessenger->GetAirCavitySideLength();
      G4cout << "Air cavity side length = " << airCavitySideLength << G4endl;

      G4double halfCavityLength = 0.5 * airCavitySideLength;
      G4Box* cavityBox = new G4Box("AirCavity", halfCavityLength, halfCavityLength, halfCavityLength);
      logicCavity = new G4LogicalVolume(cavityBox, cavityMaterial, "AirCavity");

      new G4PVPlacement(
          nullptr,                // No rotation
          G4ThreeVector(),        // Centered at origin
          logicCavity,            // Logical volume
          "AirCavity",            // Name
          logicRock,              // Mother volume is the rock
          false,                  // No boolean ops
          0,                      // Copy number
          true                    // Check overlaps
      );
    }

    //Target
    if (fMessenger->GetTargetSideLength() > 0.0) {

        //Decide what the mother volume is depending on whether or not we have an air cavity
        //Default to logicRock (world) and overwrite if an air cavity is present
        G4LogicalVolume* motherVolume = logicRock;
        if (fMessenger->GetAirCavitySideLength() > 0.0) {
            motherVolume = logicCavity;
        }

        G4String targetMaterialName = fMessenger->GetTargetMaterial();
        G4cout << "Target material is = " << targetMaterialName << G4endl;
        G4Material* targetMaterial = fMaterialManager->GetMaterial(targetMaterialName);

        G4double targetSideLength = fMessenger->GetTargetSideLength();
        G4cout << "Target side length is = " << targetSideLength << G4endl;

        G4double targetHalfSideLength = 0.5 * targetSideLength;
        G4Box* targetBox = new G4Box("TargetBox", targetHalfSideLength, targetHalfSideLength, targetHalfSideLength);
        G4LogicalVolume* logicTarget = new G4LogicalVolume(targetBox, targetMaterial, "Target");

        new G4PVPlacement(
            nullptr,             // no rotation
            G4ThreeVector(),     // place at origin
            logicTarget,
            "Target",
            motherVolume,
            false,
            0,
            true                // check overlaps
        );
    }

    #ifdef PALEOSIM_ENABLE_GDML
        G4GDMLParser parser;
        remove("geometry.gdml");
        parser.Write("geometry.gdml", physWorld, true);
    #endif
    
    return physWorld;
}
