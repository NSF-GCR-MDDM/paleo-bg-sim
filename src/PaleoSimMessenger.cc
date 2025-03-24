#include "PaleoSimMessenger.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "PaleoSimOutputManager.hh"

PaleoSimMessenger::PaleoSimMessenger(MiniBooNEBeamlineConstruction* detector,
                                     MiniBooNEBeamlinePrimaryGeneratorAction* generator)
: fDetector(detector), fGenerator(generator)
{
    /////////////////////
    //Geometry commands//
    /////////////////////
    fGeomDirectory = new G4UIdirectory("/geom/");
    fGeomDirectory->SetGuidance("Controls for geometry");

    fOverburdenSizeCmd = new G4UIcmdWithADoubleAndUnit("/geom/setOverburdenSideLength", this);
    fOverburdenSizeCmd->SetGuidance("Set the side length of the overburden cube (world volume, cenetered at 0,0,0)");
    fOverburdenSizeCmd->SetParameterName("sideLength", true);
    fOverburdenSizeCmd->SetRange("sideLength > 0.");
    fOverburdenSizeCmd->SetDefaultUnit("m");

    fOverburdenMaterialCmd = new G4UIcmdWithAString("/geom/setOverburdenMaterial", this);
    fOverburdenMaterialCmd->SetGuidance("Set the material of the overburden rock");
    fOverburdenMaterialCmd->SetParameterName("materialName", true);

    fAirCavitySizeCmd = new G4UIcmdWithADoubleAndUnit("/geom/setAirCavitySideLength", this);
    fAirCavitySizeCmd->SetGuidance("Set the side length of the air cavity cube (centered at 0,0,0).");
    fAirCavitySizeCmd->SetParameterName("sideLength", true);
    fAirCavitySizeCmd->SetRange("sideLength >= 0.");
    fAirCavitySizeCmd->SetDefaultUnit("m");

    fTargetSizeCmd = new G4UIcmdWithADoubleAndUnit("/geom/setTargetSideLength", this);
    fTargetSizeCmd->SetGuidance("Set the side length of the target cube (centered at 0,0,0).");
    fTargetSizeCmd->SetParameterName("sideLength", true);
    fTargetSizeCmd->SetRange("sideLength >= 0.");
    fTargetSizeCmd->SetDefaultUnit("m");

    fTargetMaterialCmd = new G4UIcmdWithAString("/geom/setTargetMaterial", this);
    fTargetMaterialCmd->SetGuidance("Set the material for the target volume");
    fTargetMaterialCmd->SetParameterName("materialName", true);

    ///////////////////
    //Output commands//
    ///////////////////
    fOutputDirectory = new G4UIdirectory("/output/");
    fOutputDirectory->SetGuidance("Controls for output writing");

    fSetOutputFileCmd = new G4UIcmdWithAString("/output/setOutputFile", this);
    fSetOutputFileCmd->SetGuidance("Set the output name for TTrees file");
    fSetOutputFileCmd->SetParameterName("outputFile", true);

    fEnablePrimariesOutputCmd = new G4UIcmdWithABool("/output/enablePrimariesOutput", this);
    fEnablePrimariesOutputCmd->SetGuidance("Enable tracking of primary generation properties");
    fEnablePrimariesOutputCmd->SetParameterName("enablePrimariesOutput", true);

    fEnableNeutronTallyTreeCmd = new G4UIcmdWithABool("/output/enableNeutronTallyOutput", this);
    fEnableNeutronTallyTreeCmd->SetGuidance("Enable recording of neutrons entering air cavity");
    fEnableNeutronTallyTreeCmd->SetParameterName("enableNeutronTallyOutput", true);

    //////////////////////
    //Generator commands//
    //////////////////////
    fGeneratorDirectory = new G4UIdirectory("/generator/");
    fGeneratorDirectory->SetGuidance("Controls for generator");

    fSourceTypeCmd = new G4UIcmdWithAString("/generator/setSourceType", this);
    fSourceTypeCmd->SetGuidance("Set the source type");
    fSourceTypeCmd->SetParameterName("sourceType", true);

    fMuonEffectiveDepthCmd = new G4UIcmdWithADoubleAndUnit("/generator/setMuonEffectiveDepth", this);
    fMuonEffectiveDepthCmd->SetGuidance("Set the effective muon depth for generation");
    fMuonEffectiveDepthCmd->SetParameterName("muonEffectiveDepth", true);
    fMuonEffectiveDepthCmd->SetRange("muonEffectiveDepth >= 0.");
    fMuonEffectiveDepthCmd->SetDefaultUnit("km");
}

PaleoSimMessenger::~PaleoSimMessenger() {
    delete fOverburdenSizeCmd;
    delete fAirCavitySizeCmd;
    delete fTargetSizeCmd;
    delete fTargetMaterialCmd;
    delete fGeomDirectory;

    delete fOutputDirectory;
    delete fSetOutputFileCmd;
    delete fEnablePrimariesOutputCmd;
    delete fEnableNeutronTallyTreeCmd;

    delete fSourceTypeCmd;
    delete fMuonEffectiveDepthCmd;
    delete fGeneratorDirectory;
}

void PaleoSimMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    //Geometry
    if (command == fOverburdenSizeCmd) {
        G4double val = fOverburdenSizeCmd->GetNewDoubleValue(newValue);
        fDetector->SetOverburdenSideLength(val);
        G4cout << "Overburden size set to: " << newValue << G4endl;
    } 
    else if (command == fOverburdenMaterialCmd) {
        fDetector->SetOverburdenMaterial(newValue);
        G4cout << "Overburden material set to: " << newValue << G4endl;
    }
    else if (command == fAirCavitySizeCmd) {
        G4double val = fAirCavitySizeCmd->GetNewDoubleValue(newValue);
        fDetector->SetAirCavitySideLength(val);
        G4cout << "Air cavity size set to: " << val << G4endl;
    }
    else if (command == fTargetSizeCmd) {
        G4double val = fTargetSizeCmd->GetNewDoubleValue(newValue);
        fDetector->SetTargetSideLength(val);
        G4cout << "Detector size set to: " << val << G4endl;
    }
    else if (command == fTargetMaterialCmd) {
        fDetector->SetTargetMaterial(newValue);
        G4cout << "Detector material set to: " << newValue << G4endl;
    }

    // Output
    else if (command == fSetOutputFileCmd) {
        PaleoSimOutputManager::Get().SetOutputPath(newValue);
        G4cout << "Output file set to: " << newValue << G4endl;
    }
    else if (command == fEnablePrimariesOutputCmd) {
        G4bool val = fEnablePrimariesOutputCmd->GetNewBoolValue(newValue);
        PaleoSimOutputManager::Get().EnablePrimariesTree(val);
        G4cout << "Primary output enabled: " << (val ? "true" : "false") << G4endl;
    }
    else if (command == fEnableNeutronTallyTreeCmd) { //TODO Disable if no airCavityCmd
        G4bool val = fEnableNeutronTallyTreeCmd->GetNewBoolValue(newValue);
        if (val && !fDetector->GetAirCavitySideLength() > 0) {
            G4Exception("PaleoSimMessenger", "InputErr001", FatalException,
                "Air cavity not defined so cannot enable tracking!");
        }
        PaleoSimOutputManager::Get().EnableNeutronTallyTree(val);
        G4cout << "Neutron tally output enabled: " << (val ? "true" : "false") << G4endl;
    }

    //Generator
    else if (command == fSourceTypeCmd) {
        fGenerator->SetSourceType(newValue);
        G4cout << "Generator set to: " << newValue << G4endl;
    }
    else if (command == fMuonEffectiveDepthCmd) {
        G4double val = fMuonEffectiveDepthCmd->GetNewDoubleValue(newValue);
        fGenerator->SetMuonEffectiveDepth(val);
        G4cout << "Effective muon depth set to: " << val << G4endl;
    }
}
