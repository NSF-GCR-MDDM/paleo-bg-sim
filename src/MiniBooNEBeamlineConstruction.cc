//Geometry specification
#include "MiniBooNEBeamlineConstruction.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Polycone.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4Trap.hh"
#include "G4VisAttributes.hh"
#include "G4Sphere.hh"
#ifdef PALEOSIM_ENABLE_GDML
    #include "G4GDMLParser.hh"
#endif
#include "G4VisAttributes.hh"


MiniBooNEBeamlineConstruction::MiniBooNEBeamlineConstruction()
: G4VUserDetectorConstruction(),
  fMessenger(0)
{     
    materials = new PaleoSimMaterialManager();
}

MiniBooNEBeamlineConstruction::~MiniBooNEBeamlineConstruction()
{ 
    delete fMessenger;
    delete materials;
}

G4VPhysicalVolume* MiniBooNEBeamlineConstruction::Construct()
{  

    //Rock overburden
    G4Material* overburdenMaterial = materials->GetMaterial(fOverburdenMaterial); 
    G4double halfOverburdenLength = 0.5 * fOverburdenSideLength;
    G4Box* rockBox = new G4Box("rockBox",halfOverburdenLength,halfOverburdenLength,halfOverburdenLength);
    G4LogicalVolume* logicRock = new G4LogicalVolume(rockBox,overburdenMaterial,"rockBox");

    G4bool checkOverlaps = true; // Prints if there are overlapping volumes

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
    G4Material* cavityMaterial = materials->GetMaterial("Air"); 
    G4LogicalVolume* logicCavity = nullptr; //Create outside if statement so can be used in next conditional
                                            //for target w/o creating compile errors
    if (fAirCavitySideLength > 0.0) {
        G4cout << "You fAirCavitySideLength = " << fAirCavitySideLength << G4endl;
        if (fAirCavitySideLength >= fOverburdenSideLength) {
            G4Exception("MiniBooNEBeamlineConstruction", "GeomErr001", FatalException,
                "Air cavity side length exceeds rock side length.");
        } else {
            G4double halfCavityLength = 0.5 * fAirCavitySideLength;
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
    }

    //Target
    if (fTargetSideLength > 0.0) {

        //Decide what the mother volume is depending on whether or not we have an air cavity
        //Default to logicRock (world) and overwrite if an air cavity is present
        G4LogicalVolume* motherVolume = logicRock;
        if (fAirCavitySideLength > 0.0) {
            motherVolume = logicCavity;
        }

        //Check for errors
        if (fTargetSideLength >= fOverburdenSideLength) {
            G4Exception("MiniBooNEBeamlineConstruction", "GeomErr002", FatalException,
                "Target side length exceeds rock side length.");
        } 
        else if (fAirCavitySideLength > 0.0 && fTargetSideLength >= fAirCavitySideLength) {
            G4Exception("MiniBooNEBeamlineConstruction", "GeomErr004", FatalException,
                 "Target side length exceeds air cavity side length.");
        } 
        else {
            G4Material* targetMaterial = materials->GetMaterial(fTargetMaterial);

            G4double targetHalfSideLength = 0.5 * fTargetSideLength;
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
    }

    #ifdef PALEOSIM_ENABLE_GDML
        G4GDMLParser parser;
        remove("geometry.gdml");
        parser.Write("geometry.gdml", physWorld, true);
    #endif
    
    return physWorld;
}
