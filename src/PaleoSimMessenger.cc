#include "PaleoSimMessenger.hh"

PaleoSimMessenger::PaleoSimMessenger()
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

    fUserPrimariesTreeStatusCmd = new G4UIcmdWithABool("/output/setPrimariesTreeStatus", this);
    fUserPrimariesTreeStatusCmd->SetGuidance("Enable tracking of primary generation properties");
    fUserPrimariesTreeStatusCmd->SetParameterName("setPrimariesTreeStatus", true);

    fUserNeutronTallyTreeStatusCmd = new G4UIcmdWithABool("/output/setNeutronTallyTreeStatus", this);
    fUserNeutronTallyTreeStatusCmd->SetGuidance("Enable recording of neutrons entering air cavity");
    fUserNeutronTallyTreeStatusCmd->SetParameterName("setNeutronTallyTreeStatus", true);

    //////////////////////
    //Generator commands//
    //////////////////////
    fGeneratorDirectory = new G4UIdirectory("/generator/");
    fGeneratorDirectory->SetGuidance("Controls for generator");

    fSourceTypeCmd = new G4UIcmdWithAString("/generator/setSourceType", this);
    fSourceTypeCmd->SetGuidance("Set the source type");
    fSourceTypeCmd->SetParameterName("sourceType", true);

    fNPSCmd = new G4UIcmdWithAnInteger("/generator/setNPS", this);    
    fNPSCmd->SetGuidance("Set number of events to throw");
    fNPSCmd->SetParameterName("setNPS", true);

    //Add your own generator commands here
    //CUSTOM_GENERATOR_HOOK
    //
    //Mei & Hime muon generator
    fMuonEffectiveDepthCmd = new G4UIcmdWithADoubleAndUnit("/generator/setMuonEffectiveDepth", this);
    fMuonEffectiveDepthCmd->SetGuidance("Set the effective muon depth for generation");
    fMuonEffectiveDepthCmd->SetParameterName("muonEffectiveDepth", true);
    fMuonEffectiveDepthCmd->SetRange("muonEffectiveDepth >= 0.");
    fMuonEffectiveDepthCmd->SetDefaultUnit("km");
}

void PaleoSimMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    //Geometry
    if (command == fOverburdenSizeCmd) {
        fOverburdenSideLength = fOverburdenSizeCmd->GetNewDoubleValue(newValue);
        G4cout << "Overburden size set in macro to: " << newValue << G4endl;
    } 
    else if (command == fOverburdenMaterialCmd) {
        fOverburdenMaterial = newValue;
        G4cout << "Overburden material set in macro to: " << newValue << G4endl;
    }
    else if (command == fAirCavitySizeCmd) {
        fAirCavitySideLength = fAirCavitySizeCmd->GetNewDoubleValue(newValue);
        G4cout << "Air cavity size set in macro to: " << newValue << G4endl;
    }
    else if (command == fTargetSizeCmd) {
        fTargetSideLength = fTargetSizeCmd->GetNewDoubleValue(newValue);
        G4cout << "Detector size set in macro to: " << newValue << G4endl;
    }
    else if (command == fTargetMaterialCmd) {
        fTargetMaterial = newValue;
        G4cout << "Detector material set in macro to: " << newValue << G4endl;
    }

    // Output
    else if (command == fSetOutputFileCmd) {
        fOutputFile = newValue;
        G4cout << "Output file set in macro to: " << newValue << G4endl;
    }
    else if (command == fUserPrimariesTreeStatusCmd) {
        fUserPrimariesTreeOutputStatus = newValue;
        G4cout << "Primaries tree output set in macro to: " << newValue << G4endl;
    }
    else if (command == fUserNeutronTallyTreeStatusCmd) {
      fUserNeutronTallyTreeOutputStatus = newValue;
        G4cout << "Neutron tally tree output set in macro to: " << newValue << G4endl;
    }
    else if (command == fNPSCmd) {
        fNPS = fNPSCmd->GetNewIntValue(newValue);
        G4cout << "NPS set in macro to: " << newValue << G4endl;
    }

    //Generator
    else if (command == fSourceTypeCmd) {
        fSourceType = newValue;
        G4cout << "Generator set in macro to: " << newValue << G4endl;
    }
    //CUSTOM_GENERATOR_HOOK
    //Set custom parameters for generators from messenger here
    //
    //Mei & Hime 2006
    else if (command == fMuonEffectiveDepthCmd) {
        fMuonEffectiveDepth = fMuonEffectiveDepthCmd->GetNewDoubleValue(newValue);
        G4cout << "Effective muon depth set in macro to: " << newValue << G4endl;
    }
}
