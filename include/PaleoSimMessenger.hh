#ifndef PALEO_SIM_MESSENGER_HH
#define PALEO_SIM_MESSENGER_HH

#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"
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
    void SetMeiHimeFluxNormalization(double val) { fMeiHimeFluxNormalization = val;};
    G4double GetMeiHimeFluxNormalization() const { return fMeiHimeFluxNormalization;};
    //
    //Mute generator
    G4String GetMuteHistFilename() const { return fMuteHistFilename;};
    //
    //CRY generator
    G4String GetCRYFilename() const { return fCRYFilename; };
    void SetCRYAltitude(double val) { fCRYAltitude = val;};
    void SetCRYLatitude(double val) { fCRYLatitude = val;};   
    void SetCRYNorm(double val) { fCRYNorm = val;};    
    G4double GetCRYAltitude() const { return fCRYAltitude;};  
    G4double GetCRYLatitude() const { return fCRYLatitude;};
    G4double GetCRYNorm() const { return fCRYNorm;};
    //Volumetric source generator
    G4String GetVolumetricSourceVolumeName() const { return fVolumetricSourceVolumeName; };
    G4int GetVolumetricSourcePDGCode() const { return fVolumetricSourcePDGCode; };
    G4String GetVolumetricSourceType() const { return fVolumetricSourceType; };
    G4String GetVolumetricSourceSpectrumFilename() const { return fVolumetricSourceSpectrumFilename; };
    G4String GetVolumetricSourceSpectrumHistName() const { return fVolumetricSourceSpectrumHistName; };
    G4double GetVolumetricSourceMonoEnergy() const { return fVolumetricSourceMonoEnergy; };
    void SetVolumetricSourceVolumeName(G4String val) { fVolumetricSourceVolumeName = val; };
    void SetVolumetricSourcePDGCode(int val) { fVolumetricSourcePDGCode = val; };
    void SetVolumetricSourceType(G4String val) { fVolumetricSourceType = val; };
    void SetVolumetricSourceSpectrumFilename(G4String val) { fVolumetricSourceSpectrumFilename = val; };
    void SetVolumetricSourceSpectrumHistName(G4String val) { fVolumetricSourceSpectrumHistName = val; };
    void SetVolumetricSourceMonoEnergy(double val) { fVolumetricSourceMonoEnergy = val;};
    
    //Geometric calculations
    void ComputeCoordinates();
    void ComputeAbsoluteCoordinatesRecursive(PaleoSimVolumeDefinition* vol);
    void ComputeRelativeCoordinatesRecursive(PaleoSimVolumeDefinition* vol);
    void ComputeCumulativeRotationsRecursive(PaleoSimVolumeDefinition* vol);

    //New geometry helper functions
    G4String GetGeometryMacroPath() const { return fGeometryMacroPath; }
    void AddVolume(PaleoSimVolumeDefinition* vol) {fVolumes.push_back(vol); };
    const std::vector<PaleoSimVolumeDefinition*>& GetVolumes() const { return fVolumes; };
    bool VolumeNameExists(const G4String& name) const {
        for (const auto& vol : fVolumes) {
            if (vol->name == name) return true;
        }
        return false;
    };
    PaleoSimVolumeDefinition* GetVolumeByName(const std::string& name) const {
        for (auto* vol : fVolumes) {
            if (vol->name == name) return vol;
        }
        return nullptr;
    }


private:
    //New geometry configuration
    G4UIdirectory* fGeomDirectory = nullptr;
    
    std::vector<PaleoSimVolumeDefinition*> fVolumes;
    G4UIcmdWithAString* fSetGeometryMacroCmd = nullptr;
    G4String fGeometryMacroPath = "";

    // Output configuration
    G4UIdirectory* fOutputDirectory = nullptr;
    G4UIcmdWithABool* fSetVRMLStatusCmd = nullptr;
    G4UIcmdWithABool* fSetPrimariesTreeStatusCmd = nullptr;
    G4UIcmdWithABool* fSetMINTreeStatusCmd = nullptr;
    G4UIcmdWithAString* fSetNeutronTallyTreeVolumeCmd = nullptr;
    G4UIcmdWithAString* fSetRecoilTreeVolumeCmd = nullptr;

    G4String fOutputFile = "outputFiles/output.root";
    G4bool fPrimariesTreeStatus = true;

    G4bool fMINTreeStatus = false;

    G4String fNeutronTallyTreeVolume = "";
    G4bool fNeutronTallyTreeStatus = false;
    
    G4String fRecoilTreeVolume = "";
    G4bool fRecoilTreeStatus = false;
    
    G4bool fVRMLStatus = false;

    //Generator general
    G4UIdirectory* fGeneratorDirectory = nullptr;
    G4UIcmdWithAString* fSourceTypeCmd = nullptr;
    G4UIcmdWithAnInteger* fNPSCmd;

    //Source types:
    // CUSTOM_GENERATOR_HOOK
    // Add new generator names to the list below
    std::vector<G4String> fValidSourceTypes = {
          "meiHimeMuonGenerator", //Mei & Hime muon generator, with TF1s
          "muteGenerator", //Mute, samples from 2D histogram of muon energy and thetas (root file)
          "CRYGenerator", //Samples root file containing list of events
          "volumetricSource" //Samples particles from a flat Volumetric with supplied direction and spectrum 
      };
    
    G4String fSourceType = "meiHimeMuonGenerator";
    int fNPS = 100;

    // CUSTOM_GENERATOR_HOOK
    // Implement your own generator variables/functions here
    //
    // "meiHimeMuonGenerator"
    G4UIdirectory* fMeiHimeGeneratorDirectory = nullptr;
    G4UIcmdWithADoubleAndUnit* fMeiHimeMuonDepthCmd = nullptr;
    G4double fMeiHimeMuonDepth = 6.*km;
    G4double fMeiHimeFluxNormalization = 0;
    //
    // "muteGenerator"
    G4UIdirectory* fMuteGeneratorDirectory = nullptr;
    G4UIcmdWithAString* fSetMuteHistFilenameCmd = nullptr;
    G4String fMuteHistFilename="";
    //
    // "cry"
    G4UIdirectory* fCRYGeneratorDirectory = nullptr;
    G4UIcmdWithAString* fSetCRYFilenameCmd = nullptr;
    G4String fCRYFilename="";
    G4double fCRYAltitude, fCRYLatitude;
    G4double fCRYNorm;
    //
    // "VolumetricSourceGenerator"
    G4UIdirectory* fVolumetricSourceDirectory = nullptr;
    G4UIcmdWithAString* fSetVolumetricSourceVolumeNameCmd = nullptr;
    G4UIcmdWithAnInteger* fSetVolumetricSourcePDGCodeCmd = nullptr;
    G4UIcmdWithAString* fSetVolumetricSourceTypeCmd = nullptr; //"hist" or "mono"
    G4UIcmdWithAString* fSetVolumetricSourceSpectrumFilenameCmd = nullptr;
    G4UIcmdWithAString* fSetVolumetricSourceSpectrumHistNameCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetVolumetricSourceMonoEnergyCmd = nullptr;
    G4int fVolumetricSourcePDGCode = 2112; //Neutron
    G4String fVolumetricSourceType = "mono";
    G4String fVolumetricSourceVolumeName = "";
    G4String fVolumetricSourceSpectrumFilename = "";
    G4String fVolumetricSourceSpectrumHistName = "";
    G4double fVolumetricSourceMonoEnergy = 1.*MeV;
};

#endif

