#ifndef PALEO_SIM_MESSENGER_HH
#define PALEO_SIM_MESSENGER_HH

#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"

class G4UIcmdWithADoubleAndUnit;
class G4UIdirectory;

class PaleoSimMessenger : public G4UImessenger {
public:
    PaleoSimMessenger() override;
    ~PaleoSimMessenger() = default;

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

    //Getters for the macro commands
    G4double GetUserOverburdenSideLength() const { return fOverburdenSideLength; };
    G4String GetUserOverburdenMaterial() const { return fOverburdenMaterial; };
    G4double GetAirCavitySideLength() const { return fAirCavitySideLength; };
    G4double GetTargetSideLength() const { return fTargetSideLength; };
    G4String GetTargetMaterial() const { return fTargetMaterial; };
    
    //Output
    G4bool GetUserPrimariesTreeOutputStatus() const { return fUserPrimariesTreeOutputStatus; };
    G4bool GetUserNeutronTallyTreeOutputStatus() const { return fUserNeutronTallyTreeOutputStatus; };

    //Generator
    G4String GetSourceType() const {return fSourceType;};
    G4int GetNPS() const { return fNPS; };

    //Add your own generator commands here
    //CUSTOM_GENERATOR_HOOK
    //
    //Mei & Hime muon generator
    G4double GetMuonEffectiveDepth() const { return fMuonEffectiveDepth;};

private:
    G4UIdirectory* fGeomDirectory;
    G4UIcmdWithADoubleAndUnit* fOverburdenSizeCmd;  
    G4UIcmdWithAString* fOverburdenMaterialCmd;  
    G4UIcmdWithADoubleAndUnit* fAirCavitySizeCmd;  
    G4UIcmdWithADoubleAndUnit* fTargetSizeCmd;
    G4UIcmdWithAString* fTargetMaterialCmd;

    //
    G4double fOverburdenSideLength = 20 * m;
    G4String fOverburdenMaterial = "Norite";
    G4double fAirCavitySideLength = 0 * m;
    G4double fTargetSideLength = 1 * cm;
    G4String fTargetMaterial = "Norite";

    // Output configuration
    G4UIdirectory* fOutputDirectory;
    G4UIcmdWithAString* fSetOutputFileCmd;
    G4UIcmdWithABool* fUserPrimariesTreeStatusCmd;
    G4UIcmdWithABool* fUserNeutronTallyTreeStatusCmd;

    G4String fOutputFile = "outputFiles/output.root";
    G4bool fUserPrimariesTreeOutputStatus = true;
    G4bool fUserNeutronTallyTreeOutputStatus = false;

    //Generator general
    G4UIdirectory* fGeneratorDirectory;
    G4UIcmdWithAString* fSourceTypeCmd;
    G4UIcmdWithAnInteger* fNPSCmd;

    G4String fSourceType = "muonGenerator";
    int fNPS = 100;

    //Add your own generator commands here
    //CUSTOM_GENERATOR_HOOK
    //
    //Mei & Hime muon generator
    G4UIcmdWithADoubleAndUnit* fMuonEffectiveDepthCmd;
    G4double fMuonEffectiveDepth = 6 * km;


};

#endif

