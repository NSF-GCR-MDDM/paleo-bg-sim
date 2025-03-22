#include "PaleoSimMessenger.hh"
#include "MiniBooNEBeamlineConstruction.hh"

PaleoSimMessenger::PaleoSimMessenger(MiniBooNEBeamlineConstruction* detector)
: fDetector(detector)
{
    // Create a UI directory: /geom/
    fGeomDirectory = new G4UIdirectory("/geom/");
    fGeomDirectory->SetGuidance("Controls for geometry");

    // Command: /geom/setOverburdenSideLength
    fOverburdenSizeCmd = new G4UIcmdWithADoubleAndUnit("/geom/setOverburdenSideLength", this);
    fOverburdenSizeCmd->SetGuidance("Set the side length of the overburden cube (world volume, cenetered at 0,0,0)");
    fOverburdenSizeCmd->SetParameterName("sideLength", false);
    fOverburdenSizeCmd->SetRange("sideLength > 0.");
    fOverburdenSizeCmd->SetDefaultUnit("m");

    // Command: /geom/setAirCavitySideLength
    fAirCavitySizeCmd = new G4UIcmdWithADoubleAndUnit("/geom/setAirCavitySideLength", this);
    fAirCavitySizeCmd->SetGuidance("Set the side length of the air cavity cube (centered at 0,0,0).");
    fAirCavitySizeCmd->SetParameterName("sideLength", false);
    fAirCavitySizeCmd->SetRange("sideLength >= 0.");
    fAirCavitySizeCmd->SetDefaultUnit("m");

    // Command: /geom/setTargetSideLength
    fTargetSizeCmd = new G4UIcmdWithADoubleAndUnit("/geom/setTargetSideLength", this);
    fTargetSizeCmd->SetGuidance("Set the side length of the target cube (centered at 0,0,0).");
    fTargetSizeCmd->SetParameterName("sideLength", false);
    fTargetSizeCmd->SetRange("sideLength >= 0.");
    fTargetSizeCmd->SetDefaultUnit("m");

    // Command: /geom/setTargetMaterial
    fTargetMaterialCmd = new G4UIcmdWithAString("/geom/setTargetMaterial", this);
    fTargetMaterialCmd->SetGuidance("Set the material for the target volume");
    fTargetMaterialCmd->SetParameterName("materialName", false);
}

PaleoSimMessenger::~PaleoSimMessenger() {
    delete fOverburdenSizeCmd;
    delete fAirCavitySizeCmd;
    delete fTargetSizeCmd;
    delete fTargetMaterialCmd;
    delete fGeomDirectory;
}

void PaleoSimMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fOverburdenSizeCmd) {
        G4double val = fOverburdenSizeCmd->GetNewDoubleValue(newValue);
        fDetector->SetOverburdenSideLength(val);
    } 
    else if (command == fAirCavitySizeCmd) {
        G4double val = fAirCavitySizeCmd->GetNewDoubleValue(newValue);
        fDetector->SetAirCavitySideLength(val);
    }
    else if (command == fTargetSizeCmd) {
        G4double val = fTargetSizeCmd->GetNewDoubleValue(newValue);
        fDetector->SetTargetSideLength(val);
    }
    else if (command == fTargetMaterialCmd) {
        fDetector->SetTargetMaterial(newValue);
    }
}

