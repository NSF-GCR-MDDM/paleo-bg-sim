#ifndef PALEO_SIM_MESSENGER_HH
#define PALEO_SIM_MESSENGER_HH

#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

class G4UIcmdWithADoubleAndUnit;
class G4UIdirectory;
class MiniBooNEBeamlineConstruction;
class MiniBooNEBeamlinePrimaryGeneratorAction;

class PaleoSimMessenger : public G4UImessenger {
public:
    PaleoSimMessenger(MiniBooNEBeamlineConstruction* detector,
                      MiniBooNEBeamlinePrimaryGeneratorAction* generator);
    ~PaleoSimMessenger() override;

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    MiniBooNEBeamlineConstruction* fDetector;
    MiniBooNEBeamlinePrimaryGeneratorAction* fGenerator;

    G4UIdirectory* fGeomDirectory;
    G4UIcmdWithADoubleAndUnit* fOverburdenSizeCmd;  
    G4UIcmdWithAString* fOverburdenMaterialCmd;  
    G4UIcmdWithADoubleAndUnit* fAirCavitySizeCmd;  
    G4UIcmdWithADoubleAndUnit* fTargetSizeCmd;
    G4UIcmdWithAString* fTargetMaterialCmd;


    G4UIdirectory* fGeneratorDirectory;
    G4UIcmdWithAString* fSourceTypeCmd;

    //Muon Generator
    G4UIcmdWithADoubleAndUnit* fMuonEffectiveDepthCmd;

};

#endif

