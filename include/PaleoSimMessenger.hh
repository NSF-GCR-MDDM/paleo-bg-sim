#ifndef PALEO_SIM_MESSENGER_HH
#define PALEO_SIM_MESSENGER_HH

#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include <vector>

#include "PaleoSimVolumeDefinition.hh"

class PaleoSimMessenger : public G4UImessenger {
public:
    PaleoSimMessenger();    
    virtual ~PaleoSimMessenger();
    void CheckForMacroErrors();

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

    //Output
    void SetOutputPath(G4String path) { fOutputFile=path; };
    G4String GetOutputPath() const { return fOutputFile; };
    //Trees can be enabled/disabled from macro. These flags track those
    bool GetPrimariesTreeStatus() const { return fPrimariesTreeStatus; };
    bool GetNeutronTallyTreeStatus() const { return fNeutronTallyTreeStatus; };
    bool GetMINTreeStatus() const { return fMINTreeStatus; };
    G4String GetNeutronTallyTreeVolume() const { return fNeutronTallyTreeVolume; };
    bool GetRecoilTreeStatus() const { return fRecoilTreeStatus; };
    G4String GetRecoilTreeVolume() const { return fRecoilTreeVolume; };
    bool GetVRMLStatus() const { return fVRMLStatus; };

    //Generator
    std::vector<G4String> GetValidSourceTypes() const { return fValidSourceTypes; };
    G4String GetSourceType() const {return fSourceType;};
    G4int GetNPS() const { return fNPS; };

    //Add your own generator commands here
    //CUSTOM_GENERATOR_HOOK
    //
    //Mei & Hime muon generator
    G4double GetMeiHimeMuonEffectiveDepth() const { return fMeiHimeMuonDepth;};
    //
    //Mute generator
    G4String GetMuteHistFilename() const { return fMuteHistFilename;};

    //Geometric calculations
    void ComputeCoordinates();
    void ComputeAbsoluteCoordinatesRecursive(VolumeDefinition* vol);
    void ComputeRelativeCoordinatesRecursive(VolumeDefinition* vol);
    void ComputeCumulativeRotationsRecursive(VolumeDefinition* vol);

    //New geometry helper functions
    G4String GetGeometryMacroPath() const { return fGeometryMacroPath; }
    void AddVolume(VolumeDefinition* vol) {fVolumes.push_back(vol); };
    const std::vector<VolumeDefinition*>& GetVolumes() const { return fVolumes; };
    bool VolumeNameExists(const G4String& name) const {
        for (const auto& vol : fVolumes) {
            if (vol->name == name) return true;
        }
        return false;
    };
    VolumeDefinition* GetVolumeByName(const std::string& name) const {
        for (auto* vol : fVolumes) {
            if (vol->name == name) return vol;
        }
        return nullptr;
    }


private:
    //New geometry configuration
    G4UIdirectory* fGeomDirectory;
    
    std::vector<VolumeDefinition*> fVolumes;
    G4UIcmdWithAString* fSetGeometryMacroCmd;
    G4String fGeometryMacroPath = "";

    // Output configuration
    G4UIdirectory* fOutputDirectory;
    G4UIcmdWithABool* fSetVRMLStatusCmd;
    G4UIcmdWithABool* fSetPrimariesTreeStatusCmd;
    G4UIcmdWithABool* fSetMINTreeStatusCmd;
    G4UIcmdWithAString* fSetNeutronTallyTreeVolumeCmd;
    G4UIcmdWithAString* fSetRecoilTreeVolumeCmd;

    G4String fOutputFile = "outputFiles/output.root";
    G4bool fPrimariesTreeStatus = true;

    G4bool fMINTreeStatus = false;

    G4String fNeutronTallyTreeVolume = "";
    G4bool fNeutronTallyTreeStatus = false;
    
    G4String fRecoilTreeVolume = "";
    G4bool fRecoilTreeStatus = false;
    
    G4bool fVRMLStatus = false;

    //Generator general
    G4UIdirectory* fGeneratorDirectory;
    G4UIcmdWithAString* fSourceTypeCmd;
    G4UIcmdWithAnInteger* fNPSCmd;

    //Source types:
    // CUSTOM_GENERATOR_HOOK
    // Add new generator names to the list below
    std::vector<G4String> fValidSourceTypes = {
          "meiHimeMuonGenerator", //Mei & Hime muon generator, with TF1s
          "muteGenerator" //Mute, samples from 2D histogram of muon energy and thetas (root file)
      };
    
    G4String fSourceType = "meiHimeMuonGenerator";
    int fNPS = 100;

    // CUSTOM_GENERATOR_HOOK
    // Implement your own generator variables/functions here
    //
    // "meiHimeMuonGenerator"
    G4UIdirectory* fMeiHimeGeneratorDirectory;
    G4UIcmdWithADoubleAndUnit* fMeiHimeMuonDepthCmd;
    G4double fMeiHimeMuonDepth = 6 * km;
    //
    // "muteGenerator"
    G4UIdirectory* fMuteGeneratorDirectory;
    G4UIcmdWithAString* fSetMuteHistFilenameCmd;
    G4String fMuteHistFilename;

};

#endif

