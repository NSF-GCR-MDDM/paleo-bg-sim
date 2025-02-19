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
    // Get nist material manager this allows you to find and reference materials stored in the G4NistManager.hh

    G4NistManager* nist = G4NistManager::Instance();
    
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); //Makes air    
    G4Element* Si = nist->FindOrBuildElement("Si"); // Silicon
    G4Element* O = nist->FindOrBuildElement("O");  // Oxygen
    G4Element* Al = nist->FindOrBuildElement("Al"); // Aluminum
    G4Element* Fe = nist->FindOrBuildElement("Fe"); // Iron
    G4Element* Ca = nist->FindOrBuildElement("Ca"); // Calcium
    G4Element* Mg = nist->FindOrBuildElement("Mg"); // Magnesium
    G4Element* Na = nist->FindOrBuildElement("Na"); // Sodium
    G4Element* K = nist->FindOrBuildElement("K");  // Potassium
    
    G4Box* solidWorld = new G4Box("World", 50*m, 50*m, 50*m); //setting the parameters for the background where you place your geometry

    
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "World"); //Telling what to make the background out of i.e. we want it to be air     

    G4bool checkOverlaps = true; //tells you if you have overlapping volumes
  
    //Physical volumes                              
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps); //Places our background 
    //Parameters beign 
    G4double NoriteHalfSize=10*m;
    //Parameters end
    

    //Begin Norite
    G4Material* Norite = new G4Material("Norite", 2.894*g/cm3, 8);
    
    // Add elements to the material (fractions are by mass)
    Norite->AddElement(Si, 0.245); // 24.5% Silicon
    Norite->AddElement(O, 0.420);  // 42.0% Oxygen
    Norite->AddElement(Al, 0.100); // 10.0% Aluminum
    Norite->AddElement(Fe, 0.100); // 10.0% Iron
    Norite->AddElement(Ca, 0.050); // 5.0% Calcium
    Norite->AddElement(Mg, 0.050); // 5.0% Magnesium
    Norite->AddElement(Na, 0.020); // 2.0% Sodium
    Norite->AddElement(K, 0.015);  // 1.5% Potassium
    //End Norite
    
    //Solid and Logic Volume begin

    G4Box* solidNorite = new G4Box("Norite", NoriteHalfSize, NoriteHalfSize, NoriteHalfSize);
    G4LogicalVolume* logicNorite = new G4LogicalVolume(solidNorite, Norite, "Norite");

    //Solid and Logic Volume end
    
    
    //Placement begin
    new G4PVPlacement(0, G4ThreeVector(0.0*m, 0.0*m, NoriteHalfSize), logicNorite, "Norite", logicWorld, false, 0, checkOverlaps);
    return physWorld;
}

void MiniBooNEBeamlineConstruction::ConstructSDandField()
{
    fMagneticField = new HornMagneticField();
    fFieldMgr = new G4FieldManager();
    fFieldMgr->SetDetectorField(fMagneticField);
    fFieldMgr->CreateChordFinder(fMagneticField);
}
