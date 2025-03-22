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
{ }

MiniBooNEBeamlineConstruction::~MiniBooNEBeamlineConstruction()
{ 
    delete fMessenger;
}

G4VPhysicalVolume* MiniBooNEBeamlineConstruction::Construct()
{  
    // Find and reference materials stored in G4NistManager.hh
    G4NistManager* nist = G4NistManager::Instance();
    
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); // Air    
    G4Element* H = nist->FindOrBuildElement("H");		   // Hydrogen
    G4Element* C = nist->FindOrBuildElement("C");		   // Carbon
    G4Element* O = nist->FindOrBuildElement("O");		   // Oxygen
    G4Element* Na = nist->FindOrBuildElement("Na");		   // Sodium
    G4Element* Mg = nist->FindOrBuildElement("Mg");		   // Magnesium
    G4Element* Al = nist->FindOrBuildElement("Al");		   // Aluminum
    G4Element* Si = nist->FindOrBuildElement("Si");		   // Silicon
    G4Element* K = nist->FindOrBuildElement("K");		   // Potassium
    G4Element* Ca = nist->FindOrBuildElement("Ca");		   // Calcium
    G4Element* Mn = nist->FindOrBuildElement("Mn");		   // Manganese
    G4Element* Fe = nist->FindOrBuildElement("Fe");		   // Iron
    G4Element* Ti = nist->FindOrBuildElement("Ti");		   // Titanium
    
    //Begin Norite
    G4Material* Norite = new G4Material("Norite", 2.894*g/cm3, 12);
    
    // Add elements to the material (fractions are by mass)
    Norite->AddElement(H, 0.001425);  // 0.15% Silicon
	Norite->AddElement(C, 0.000325);  // 0.04% Carbon
	Norite->AddElement(O, 0.459925);  // 46.0% Oxygen
	Norite->AddElement(Na, 0.021925); // 2.2% Sodium
	Norite->AddElement(Mg, 0.032925); // 3.3% Magnesium
	Norite->AddElement(Al, 0.089925); // 9.0% Aluminum
	Norite->AddElement(Si, 0.261925); // 26.2% Sodium
	Norite->AddElement(K, 0.011925);  // 1.2% Potassium
	Norite->AddElement(Ca, 0.051925); // 5.2% Calcium
	Norite->AddElement(Mn, 0.000925); // 0.1% Manganese
	Norite->AddElement(Fe, 0.061925); // 6.2% Iron
	Norite->AddElement(Ti, 0.004925); // 0.5% Titanium

    //Rock overburden
    G4double halfOverburdenLength = 0.5 * fOverburdenSideLength;
    G4Box* rockBox = new G4Box("rockBox",halfOverburdenLength,halfOverburdenLength,halfOverburdenLength);
    G4LogicalVolume* logicRock = new G4LogicalVolume(rockBox,Norite,"rockBox");

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
    G4LogicalVolume* logicCavity = nullptr; //Create outside if statement so can be used in next conditional
                                            //for target w/o creating compile errors
    if (fAirCavitySideLength > 0.0) {
        if (fAirCavitySideLength >= fOverburdenSideLength) {
            G4Exception("MiniBooNEBeamlineConstruction", "GeomErr001", FatalException,
                "Air cavity side length exceeds rock side length.");
        } else {
            G4double halfCavityLength = 0.5 * fAirCavitySideLength;
            G4Box* cavityBox = new G4Box("AirCavity", halfCavityLength, halfCavityLength, halfCavityLength);
            logicCavity = new G4LogicalVolume(cavityBox, air, "AirCavity");
    
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
            G4Material* placeholderMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

            G4double targetHalfSideLength = 0.5 * fTargetSideLength;
            G4Box* targetBox = new G4Box("TargetBox", targetHalfSideLength, targetHalfSideLength, targetHalfSideLength);
            G4LogicalVolume* logicTarget = new G4LogicalVolume(targetBox, placeholderMaterial, "Target");

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

        //Modifying visibility settings
        G4VisAttributes* rockVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.2));
        rockVis->SetVisibility(true);
        rockVis->SetForceSolid(true);
        logicRock->SetVisAttributes(rockVis);

        G4VisAttributes* airVis = new G4VisAttributes(G4Colour(0.5, 0.0, 0.0, 0.2));
        airVis->SetVisibility(true);
        airVis->SetForceSolid(true);
        if (fAirCavitySideLength>0.) {
            logicCavity->SetVisAttributes(airVis);
        }

        remove("geometry.gdml");

        if (logicRock->GetVisAttributes()) {
            G4cout << "[DEBUG] logicRock vis attributes are set." << G4endl;
        } else {
            G4cout << "[DEBUG] logicRock vis attributes are NULL!" << G4endl;
        }
        
        parser.Write("geometry.gdml", physWorld, true);
    #endif
    
    return physWorld;
}

void MiniBooNEBeamlineConstruction::SetOverburdenSideLength(G4double val) {
    fOverburdenSideLength = val;
}
void MiniBooNEBeamlineConstruction::SetAirCavitySideLength(G4double val) {
    fAirCavitySideLength = val;
}
void MiniBooNEBeamlineConstruction::SetTargetSideLength(G4double val) {
    fTargetSideLength = val;
}
void MiniBooNEBeamlineConstruction::SetTargetMaterial(const G4String& val) {
    fTargetMaterial = val;
}
