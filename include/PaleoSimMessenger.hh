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
#include "G4UIcmdWithADouble.hh"
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
    void SetOutputPath(G4String path) {
        fOutputFile = path;
        fOutputPathExplicitlySet = true;
    };
    G4String GetOutputPath() const { return fOutputFile; };
    void SetOutputFormat(const G4String& fmt) {
        fOutputFormat = fmt;
        if (!fOutputPathExplicitlySet) {
            fOutputFile = GetDefaultOutputPathForFormat(fOutputFormat);
        }
    };
    G4String GetOutputFormat() const { return fOutputFormat; };
    //Trees can be enabled/disabled from macro. These flags track those
    bool GetPrimariesTreeStatus() const { return fPrimariesTreeStatus; };
    bool GetNeutronTallyTreeStatus() const { return fNeutronTallyTreeStatus; };
    bool GetMINTreeStatus() const { return fMINTreeStatus; };
    const std::vector<G4String>& GetNeutronTallyTreeVolumes() const { return fNeutronTallyTreeVolumes; };
    bool GetSecondaryCaptureTreeStatus() const { return fSecondaryCaptureTreeStatus; };
    const std::vector<G4String>& GetSecondaryCaptureTreeVolumes() const { return fSecondaryCaptureTreeVolumes; };
    bool GetRecoilTreeStatus() const { return fRecoilTreeStatus; };
    const std::vector<G4String>& GetRecoilTreeVolumes() const { return fRecoilTreeVolumes; };
    bool GetVRMLStatus() const { return fVRMLStatus; };

    //Generators
    std::vector<G4String> GetValidSourceTypes() const { return fValidSourceTypes; };
    G4String GetSourceType() const {return fSourceType;};
    long long GetNPS() const { return fNPS;};

    //CUSTOM_GENERATOR_HOOK
    //Source-specific getters and setters here
    //
    //Mei & Hime muon generator
    G4double GetMeiHimeMuonEffectiveDepth() const { return fMeiHimeMuonDepth;};
    void SetMeiHimeFluxNormalization(double val) { fMeiHimeFluxNormalization = val;};
    G4double GetMeiHimeFluxNormalization() const { return fMeiHimeFluxNormalization;};

    //Mute generator
    G4String GetMuteHistFilename() const { return fMuteHistFilename;};
    G4String GetMuteOverburdenType() const { return fMuteOverburdenType;};
    G4String GetMuteMountainProfileFilename() const { return fMuteMountainProfileFilename;};
    G4double GetMuteFluxDepth() const { return fMuteFluxDepth;};
    G4double GetMuteFluxNormalization() const { return fMuteFluxNormalization;};

    //CRY generator
    G4String GetCRYFilename() const { return fCRYFilename; };
    void SetCRYAltitude(double val) { fCRYAltitude = val;};
    void SetCRYLatitude(double val) { fCRYLatitude = val;};   
    void SetCRYNorm(double val) { fCRYNorm = val;};    
    G4double GetCRYAltitude() const { return fCRYAltitude;};  
    G4double GetCRYLatitude() const { return fCRYLatitude;};
    G4double GetCRYNorm() const { return fCRYNorm;};
    
    //SCT generator
    G4String GetSCTFilename() const { return fSCTFilename; };
    G4String GetSCTBootstrap() const { return fSCTBootstrap; };
    void SetSCTNorm(double val) { fSCTNorm = val; };     
    G4double GetSCTNorm() const { return fSCTNorm; };
    void SetSCTCapturedParticles(int val) { fSCTCapturedParticles = val; };
    G4int GetSCTCapturedParticles() const { return fSCTCapturedParticles; };
    
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

    //Beam source generator
    G4int GetBeamSourcePDGCode() const { return fBeamSourcePDGCode; };
    G4String GetBeamSourceType() const { return fBeamSourceType; };
    G4String GetBeamSourceDirectionType() const { return fBeamSourceDirectionType; };
    G4String GetBeamSourceSpectrumFilename() const { return fBeamSourceSpectrumFilename; };
    G4String GetBeamSourceSpectrumHistName() const { return fBeamSourceSpectrumHistName; };
    G4double GetBeamSourceMonoEnergy() const { return fBeamSourceMonoEnergy; };
    G4double GetBeamSourceRadius() const { return fBeamSourceRadius; };
    G4ThreeVector GetBeamSourcePosition() const { return fBeamSourcePosition; };
    G4ThreeVector GetBeamSourceAxis() const { return fBeamSourceAxis; };
    void SetBeamSourcePDGCode(int val) { fBeamSourcePDGCode = val; };
    void SetBeamSourceType(G4String val) { fBeamSourceType = val; };
    void SetBeamSourceDirectionType(G4String val) { fBeamSourceDirectionType = val; };
    void SetBeamSourceSpectrumFilename(G4String val) { fBeamSourceSpectrumFilename = val; };
    void SetBeamSourceSpectrumHistName(G4String val) { fBeamSourceSpectrumHistName = val;};
    void SetBeamSourceMonoEnergy(double val) { fBeamSourceMonoEnergy = val;};
    void SetBeamSourceRadius(double val) { fBeamSourceRadius = val;};
    void SetBeamSourcePosition(G4ThreeVector val) { fBeamSourcePosition = val;};
    void SetBeamSourceAxis(G4ThreeVector val) { fBeamSourceAxis = val;};
    
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
    //For overwriting the volume list with one sorted by hierarchy (guarantee parents placed before children)
    void OverwriteVolumes(const std::vector<PaleoSimVolumeDefinition*>& newVolumes) {
        fVolumes = newVolumes;
    }



private:
    //New geometry configuration
    G4UIdirectory* fGeomDirectory = nullptr;
    
    std::vector<PaleoSimVolumeDefinition*> fVolumes;
    G4UIcmdWithAString* fSetGeometryMacroCmd = nullptr;
    G4String fGeometryMacroPath = "";

    // Output configuration
    G4String GetDefaultOutputPathForFormat(const G4String& fmt) const;
    G4bool fOutputPathExplicitlySet = false;

    G4UIdirectory* fOutputDirectory = nullptr;
    G4UIcmdWithABool* fSetVRMLStatusCmd = nullptr;
    G4UIcmdWithABool* fSetPrimariesTreeStatusCmd = nullptr;
    G4UIcmdWithABool* fSetMINTreeStatusCmd = nullptr;
    G4UIcmdWithAString* fSetNeutronTallyTreeVolumesCmd = nullptr;
    G4UIcmdWithAString* fSetSecondaryCaptureTreeVolumesCmd = nullptr;
    G4UIcmdWithAString* fSetRecoilTreeVolumesCmd = nullptr;

    G4String fOutputFile = "outputFiles/output.root";
    G4UIcmdWithAString* fSetOutputFormatCmd = nullptr;
    G4String fOutputFormat = "root";
    G4bool fPrimariesTreeStatus = true;

    G4bool fMINTreeStatus = false;

    std::vector<G4String> fNeutronTallyTreeVolumes;
    G4bool fNeutronTallyTreeStatus = false;

    std::vector<G4String> fSecondaryCaptureTreeVolumes;
    G4bool fSecondaryCaptureTreeStatus = false;
    
    std::vector<G4String> fRecoilTreeVolumes;
    G4bool fRecoilTreeStatus = false;
    
    G4bool fVRMLStatus = false;

    //Generator general
    G4UIdirectory* fGeneratorDirectory = nullptr;
    G4UIcmdWithAString* fSourceTypeCmd = nullptr;
    G4UIcmdWithAString* fNPSCmd;

    //Source types:
    // CUSTOM_GENERATOR_HOOK
    // Add new generator names to the list below
    std::vector<G4String> fValidSourceTypes = {
          "meiHimeMuonGenerator", //Mei & Hime muon generator, with TF1s
          "muteGenerator", //Mute, samples from 3D histogram of muon energy, thetas, and slant depths (root file)
          "CRYGenerator", //Samples root file containing list of events
          "SCTGenerator", //Samples root file containing secondaryCaptureTree
          "volumetricSource", //Samples particles from a flat Volumetric with supplied direction and spectrum 
          "beamSource"
      };
    
    G4String fSourceType = "meiHimeMuonGenerator";
    long long fNPS = 100;

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
    G4UIcmdWithAString* fSetMuteOverburdenTypeCmd = nullptr;
    G4UIcmdWithAString* fSetMuteMountainProfileFilenameCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetMuteFluxDepthCmd = nullptr;
    G4UIcmdWithADouble* fSetMuteFluxNormalizationCmd = nullptr;
    G4String fMuteHistFilename="";
    G4String fMuteOverburdenType="flat";
    G4String fMuteMountainProfileFilename="";
    G4double fMuteFluxDepth = 6.*km;
    G4double fMuteFluxNormalization = 0;

    //
    // "cry"
    G4UIdirectory* fCRYGeneratorDirectory = nullptr;
    G4UIcmdWithAString* fSetCRYFilenameCmd = nullptr;
    G4String fCRYFilename="";
    G4double fCRYAltitude, fCRYLatitude;
    G4double fCRYNorm;
    //
    // "SCTGenerator"
    G4UIdirectory* fSCTGeneratorDirectory = nullptr;
    G4UIcmdWithAString* fSetSCTFilenameCmd = nullptr;
    G4UIcmdWithAString* fSetSCTBootstrapCmd = nullptr;
    G4String fSCTFilename="";
    G4String fSCTBootstrap="standard";
    G4double fSCTNorm;
    G4int fSCTCapturedParticles;
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
    
    //Beam source
    G4UIdirectory* fBeamSourceGeneratorDirectory = nullptr;
    G4UIcmdWithAnInteger* fSetBeamSourcePDGCodeCmd = nullptr;
    G4UIcmdWithAString* fSetBeamSourceTypeCmd = nullptr;
    G4UIcmdWithAString* fSetBeamSourceDirectionTypeCmd = nullptr;
    G4UIcmdWithAString* fSetBeamSourceSpectrumFilenameCmd = nullptr;
    G4UIcmdWithAString* fSetBeamSourceSpectrumHistNameCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetBeamSourceMonoEnergyCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetBeamSourceRadiusCmd = nullptr;
    G4UIcmdWith3VectorAndUnit* fSetBeamSourcePositionCmd = nullptr;
    G4UIcmdWith3Vector* fSetBeamSourceAxisCmd = nullptr;
    G4int fBeamSourcePDGCode = 2112;
    G4String fBeamSourceType = "mono";
    G4String fBeamSourceDirectionType = "fixed";
    G4String fBeamSourceSpectrumFilename = "";
    G4String fBeamSourceSpectrumHistName = "";
    G4double fBeamSourceMonoEnergy = 1.*MeV;
    G4double fBeamSourceRadius = 1.*cm;
    G4ThreeVector fBeamSourcePosition = G4ThreeVector(0,0,0);
    G4ThreeVector fBeamSourceAxis = G4ThreeVector(0,0,1);
};

#endif

