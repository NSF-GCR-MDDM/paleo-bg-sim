//Geometry specification
#include "MiniBooNEBeamlineConstruction.hh"
#include "HornMagneticField.hh"
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
G4ThreadLocal HornMagneticField* MiniBooNEBeamlineConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager* MiniBooNEBeamlineConstruction::fFieldMgr = 0;

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
    
	// Set params for background
    G4Box* solidWorld = new G4Box("World", 50*m, 50*m, 50*m);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "World");

    G4bool checkOverlaps = true; // Prints if there are overlapping volumes
  
    //Place background                      
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld,
													 "World", 0, false, 0, checkOverlaps);

    //Parameters begin 
    G4double NoriteHalfSize=10*m;
    //Parameters end
    
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

    //End Norite
    
    //Solid and Logic Volume begin
    G4Box* solidNorite = new G4Box("Norite", NoriteHalfSize, NoriteHalfSize,
								   NoriteHalfSize);
    G4LogicalVolume* logicNorite = new G4LogicalVolume(solidNorite, Norite, "Norite");
    //Solid and Logic Volume end

	// Place Norite
    new G4PVPlacement(0, G4ThreeVector(0.0*m, 0.0*m, NoriteHalfSize), logicNorite,
					  "Norite", logicWorld, false, 0, checkOverlaps);
    return physWorld;
}

void MiniBooNEBeamlineConstruction::ConstructSDandField()
{
    fMagneticField = new HornMagneticField();
    fFieldMgr = new G4FieldManager();
    fFieldMgr->SetDetectorField(fMagneticField);
    fFieldMgr->CreateChordFinder(fMagneticField);
}
