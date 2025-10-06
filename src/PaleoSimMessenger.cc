#include "PaleoSimMessenger.hh"
#include <set>
#include <fstream>

PaleoSimMessenger::PaleoSimMessenger() {    
    /////////////////////
    //Geometry commands//
    /////////////////////
    fGeomDirectory = new G4UIdirectory("/geom/");
    fGeomDirectory->SetGuidance("Controls for geometry");

    fSetGeometryMacroCmd = new G4UIcmdWithAString("/geom/setGeometry", this);
    fSetGeometryMacroCmd->SetGuidance("Set full path to geometry macro file to load.");
    fSetGeometryMacroCmd->SetParameterName("filepath", false); //true = optional, false = required
    fSetGeometryMacroCmd->SetDefaultValue(fGeometryMacroPath); // Purely for documentation

    ///////////////////
    //Output commands//
    ///////////////////
    fOutputDirectory = new G4UIdirectory("/output/");
    fOutputDirectory->SetGuidance("Controls for output writing");

    fSetPrimariesTreeStatusCmd = new G4UIcmdWithABool("/output/setPrimariesTreeStatus", this);
    fSetPrimariesTreeStatusCmd->SetGuidance("Enable tracking of primary generation properties");
    fSetPrimariesTreeStatusCmd->SetParameterName("fPrimariesTreeStatus", true);
    fSetPrimariesTreeStatusCmd->SetDefaultValue(fPrimariesTreeStatus);

    fSetMINTreeStatusCmd = new G4UIcmdWithABool("/output/setMINTreeStatus", this);
    fSetMINTreeStatusCmd->SetGuidance("Enable tracking of muon-induced neutrons");
    fSetMINTreeStatusCmd->SetParameterName("fMINTreeStatus", true);
    fSetMINTreeStatusCmd->SetDefaultValue(fMINTreeStatus);

    fSetNeutronTallyTreeVolumesCmd = new G4UIcmdWithAString("/output/setNeutronTallyTreeVolumes", this);
    fSetNeutronTallyTreeVolumesCmd->SetGuidance("List of names of volumes to track neutrons entering, separated by spaces");
    fSetNeutronTallyTreeVolumesCmd->SetParameterName("fNeutronTallyTreeVolumes", true);

    fSetRecoilTreeVolumesCmd = new G4UIcmdWithAString("/output/setRecoilTreeVolumes", this);
    fSetRecoilTreeVolumesCmd->SetGuidance("List of names of volumes to track nuclear recoils in, separated by spaces");
    fSetRecoilTreeVolumesCmd->SetParameterName("fRecoilTreeVolumes", true);

    fSetVRMLStatusCmd = new G4UIcmdWithABool("/output/setVRMLOutputStatus", this);
    fSetVRMLStatusCmd->SetGuidance("If true, will save vrml file of geometry in current folder with same name as geometry macro.");
    fSetVRMLStatusCmd->SetParameterName("fVRMLStatus", true); 
    fSetVRMLStatusCmd->SetDefaultValue(fVRMLStatus); 

    //////////////////////
    //Generator commands//
    //////////////////////
    fGeneratorDirectory = new G4UIdirectory("/generator/");
    fGeneratorDirectory->SetGuidance("Controls for generator");

    fSourceTypeCmd = new G4UIcmdWithAString("/generator/setSourceType", this);
    fSourceTypeCmd->SetGuidance("Set the source type");
    fSourceTypeCmd->SetParameterName("fSourceType", true);
    fSourceTypeCmd->SetDefaultValue(fSourceType);

    fNPSCmd = new G4UIcmdWithAString("/generator/setNPS", this);    
    fNPSCmd->SetGuidance("Set number of events to throw");
    fNPSCmd->SetParameterName("fNPS", true);
    fNPSCmd->SetDefaultValue(G4String(std::to_string(fNPS)));

    //Add your own generator commands here
    //CUSTOM_GENERATOR_HOOK
    //
    //Mei & Hime muon generator
    fMeiHimeGeneratorDirectory = new G4UIdirectory("/generator/meiHimeMuonGenerator/");
    fMeiHimeGeneratorDirectory->SetGuidance("Controls for Mei & Hime muon generator");

    fMeiHimeMuonDepthCmd = new G4UIcmdWithADoubleAndUnit("/generator/meiHimeMuonGenerator/setMuonEffectiveDepth", this);
    fMeiHimeMuonDepthCmd->SetGuidance("Set the effective muon depth for generation");
    fMeiHimeMuonDepthCmd->SetParameterName("fMeiHimeMuonDepth", true);
    fMeiHimeMuonDepthCmd->SetRange("fMeiHimeMuonDepth >= 0.");
    fMeiHimeMuonDepthCmd->SetDefaultUnit("km");
    fMeiHimeMuonDepthCmd->SetDefaultValue(fMeiHimeMuonDepth);

    //Mute generator
    fMuteGeneratorDirectory = new G4UIdirectory("/generator/muteGenerator/");
    fMuteGeneratorDirectory->SetGuidance("Controls for Mute Generator");

    fSetMuteHistFilenameCmd = new G4UIcmdWithAString("/generator/muteGenerator/setMuteHistFilename", this);
    fSetMuteHistFilenameCmd->SetGuidance("File containing 'muonHist' TH2D with GeV on x and Theta (rad) on y");
    fSetMuteHistFilenameCmd->SetParameterName("muteHistFilename", true);
    fSetMuteHistFilenameCmd->SetDefaultValue(fMuteHistFilename);

    //CRY generator
    fCRYGeneratorDirectory = new G4UIdirectory("/generator/cry/");
    fCRYGeneratorDirectory->SetGuidance("Controls for Cry Generator");

    fSetCRYFilenameCmd = new G4UIcmdWithAString("/generator/cry/setCRYFilename", this);
    fSetCRYFilenameCmd->SetGuidance("Pass in output of cryGenerator code (root file)");
    fSetCRYFilenameCmd->SetParameterName("fCRYFilename", true);
    fSetCRYFilenameCmd->SetDefaultValue(fCRYFilename);

    //Volumetric Source Generator
    fVolumetricSourceDirectory = new G4UIdirectory("/generator/VolumetricSource/");
    fVolumetricSourceDirectory->SetGuidance("Controls for Volumetric Source Generator");

    fSetVolumetricSourceVolumeNameCmd = new G4UIcmdWithAString("/generator/VolumetricSource/setVolumeName",this);
    fSetVolumetricSourceVolumeNameCmd->SetGuidance("Set Volumetric source name, must be defined in geometry file");
    fSetVolumetricSourceVolumeNameCmd->SetParameterName("fVolumetricSourceVolumeName", true);
    fSetVolumetricSourceVolumeNameCmd->SetDefaultValue(fVolumetricSourceVolumeName);
    fSetVolumetricSourcePDGCodeCmd = new G4UIcmdWithAnInteger("/generator/VolumetricSource/setPDGCode",this);
    fSetVolumetricSourcePDGCodeCmd->SetGuidance("Input PDG code of particle to throw");
    fSetVolumetricSourcePDGCodeCmd->SetParameterName("fVolumetricSourcePDGCode", true);
    fSetVolumetricSourcePDGCodeCmd->SetDefaultValue(fVolumetricSourcePDGCode);
    fSetVolumetricSourceTypeCmd = new G4UIcmdWithAString("/generator/VolumetricSource/setVolumetricSourceType",this);
    fSetVolumetricSourceTypeCmd->SetGuidance("Set Volumetric source type: 'mono' or 'hist'");
    fSetVolumetricSourceTypeCmd->SetParameterName("fVolumetricSourceType", true);
    fSetVolumetricSourceTypeCmd->SetDefaultValue(fVolumetricSourceType);
    //Root 'hist'
    fSetVolumetricSourceSpectrumFilenameCmd = new G4UIcmdWithAString("/generator/VolumetricSource/setVolumetricSourceSpectrumFilename",this);
    fSetVolumetricSourceSpectrumFilenameCmd->SetGuidance("Input name of root file with spectrum TH1D or TH1F and energy in MeV");
    fSetVolumetricSourceSpectrumFilenameCmd->SetParameterName("fVolumetricSourceSpectrumFilename", true);
    fSetVolumetricSourceSpectrumFilenameCmd->SetDefaultValue(fVolumetricSourceSpectrumFilename);
    fSetVolumetricSourceSpectrumHistNameCmd = new G4UIcmdWithAString("/generator/VolumetricSource/setVolumetricSourceSpectrumHistName",this);
    fSetVolumetricSourceSpectrumHistNameCmd->SetGuidance("Input name of TH1D or TH1F with energy in MeV!");
    fSetVolumetricSourceSpectrumHistNameCmd->SetParameterName("fVolumetricSourceSpectrumHistName", true);
    fSetVolumetricSourceSpectrumHistNameCmd->SetDefaultValue(fVolumetricSourceSpectrumHistName);
    //'mono'
    fSetVolumetricSourceMonoEnergyCmd = new G4UIcmdWithADoubleAndUnit("/generator/VolumetricSource/setVolumetricSourceMonoEnergy", this);
    fSetVolumetricSourceMonoEnergyCmd->SetGuidance("Set energy and unit of particles");
    fSetVolumetricSourceMonoEnergyCmd->SetParameterName("fVolumetricSourceMonoEnergy", true);
    fSetVolumetricSourceMonoEnergyCmd->SetRange("fVolumetricSourceMonoEnergy >= 0.");
    fSetVolumetricSourceMonoEnergyCmd->SetDefaultUnit("MeV");
    fSetVolumetricSourceMonoEnergyCmd->SetDefaultValue(fVolumetricSourceMonoEnergy);

    //Disk Source Generator
    fDiskSourceGeneratorDirectory = new G4UIdirectory("/generator/diskSource/");
    fDiskSourceGeneratorDirectory->SetGuidance("Controls for Disk Source Generator");
    //
    fSetDiskSourcePDGCodeCmd = new G4UIcmdWithAnInteger("/generator/diskSource/pdgCode",this);
    fSetDiskSourcePDGCodeCmd->SetGuidance("Input PDG code of particle to throw");
    fSetDiskSourcePDGCodeCmd->SetParameterName("fDiskSourcePDGCode", true);
    fSetDiskSourcePDGCodeCmd->SetDefaultValue(fDiskSourcePDGCode);
    fSetDiskSourceTypeCmd = new G4UIcmdWithAString("/generator/diskSource/setDiskSourceType",this);
    fSetDiskSourceTypeCmd->SetGuidance("Set disk source type: 'mono' or 'hist'");
    fSetDiskSourceTypeCmd->SetParameterName("fDiskSourceType", true);
    fSetDiskSourceTypeCmd->SetDefaultValue(fDiskSourceType);
    //Root 'hist'
    fSetDiskSourceSpectrumFilenameCmd = new G4UIcmdWithAString("/generator/diskSource/setDiskSourceSpectrumFilename",this);
    fSetDiskSourceSpectrumFilenameCmd->SetGuidance("Input name of root file with spectrum TH1D or TH1F and energy in MeV");
    fSetDiskSourceSpectrumFilenameCmd->SetParameterName("fDiskSourceSpectrumFilename", true);
    fSetDiskSourceSpectrumFilenameCmd->SetDefaultValue(fDiskSourceSpectrumFilename);
    fSetDiskSourceSpectrumHistNameCmd = new G4UIcmdWithAString("/generator/diskSource/setDiskSourceSpectrumHistName",this);
    fSetDiskSourceSpectrumHistNameCmd->SetGuidance("Input name of TH1D or TH1F with energy in MeV!");
    fSetDiskSourceSpectrumHistNameCmd->SetParameterName("fDiskSourceSpectrumHistName", true);
    fSetDiskSourceSpectrumHistNameCmd->SetDefaultValue(fDiskSourceSpectrumHistName);
    //'mono'
    fSetDiskSourceMonoEnergyCmd = new G4UIcmdWithADoubleAndUnit("/generator/diskSource/setDiskSourceMonoEnergy", this);
    fSetDiskSourceMonoEnergyCmd->SetGuidance("Set energy and unit of particles");
    fSetDiskSourceMonoEnergyCmd->SetParameterName("fDiskSourceMonoEnergy", true);
    fSetDiskSourceMonoEnergyCmd->SetRange("fDiskSourceMonoEnergy >= 0.");
    fSetDiskSourceMonoEnergyCmd->SetDefaultUnit("MeV");
    fSetDiskSourceMonoEnergyCmd->SetDefaultValue(fDiskSourceMonoEnergy);
    //Geometric properties
    fSetDiskSourceRadiusCmd = new G4UIcmdWithADoubleAndUnit("/generator/diskSource/setDiskSourceRadius", this);
    fSetDiskSourceRadiusCmd->SetGuidance("Set radius of disk");
    fSetDiskSourceRadiusCmd->SetParameterName("fDiskSourceRadius", true);
    fSetDiskSourceRadiusCmd->SetRange("fDiskSourceRadius >= 0.");
    fSetDiskSourceRadiusCmd->SetDefaultUnit("cm");
    fSetDiskSourceRadiusCmd->SetDefaultValue(fDiskSourceRadius);
    fSetDiskSourcePositionCmd = new G4UIcmdWith3VectorAndUnit("/generator/diskSource/setDiskSourcePositionCmd", this);
    fSetDiskSourcePositionCmd->SetGuidance("Set position of center of disk");
    fSetDiskSourcePositionCmd->SetDefaultUnit("cm");
    fSetDiskSourcePositionCmd->SetDefaultValue(fDiskSourcePosition);
    fSetDiskSourceAxisCmd = new G4UIcmdWith3Vector("/generator/diskSource/setDiskSourceAxis", this);
    fSetDiskSourceAxisCmd->SetGuidance("Set axis of the disk.");
    fSetDiskSourceAxisCmd->SetDefaultValue(fDiskSourceAxis);
    fSetDiskSourceDirectionCmd = new G4UIcmdWith3Vector("/generator/diskSource/setDiskSourceDirection", this);
    fSetDiskSourceDirectionCmd->SetGuidance("Set direction of particles emergying from the disk");
    fSetDiskSourceDirectionCmd->SetDefaultValue(fDiskSourceDirection);
}

PaleoSimMessenger::~PaleoSimMessenger() {
    // Free VolumeDefinitions
    for (auto* vol : fVolumes) delete vol;
    fVolumes.clear();

    // Delete all G4UI command objects you created
    delete fGeomDirectory;
    delete fSetGeometryMacroCmd;

    delete fOutputDirectory;
    delete fSetPrimariesTreeStatusCmd;
    delete fSetMINTreeStatusCmd;
    delete fSetNeutronTallyTreeVolumesCmd;
    delete fSetRecoilTreeVolumesCmd;
    delete fSetVRMLStatusCmd;

    delete fGeneratorDirectory;
    delete fSourceTypeCmd;
    delete fNPSCmd;

    //CUSTOM_GENERATOR_HOOK
    //Memory management
    //
    //MeiHimeGenerator
    delete fMeiHimeGeneratorDirectory;
    delete fMeiHimeMuonDepthCmd;
    //
    //Mute generator
    delete fMuteGeneratorDirectory;
    delete fSetMuteHistFilenameCmd;
    //CRY
    delete fCRYGeneratorDirectory;
    delete fSetCRYFilenameCmd;
    //Volumetric source
    delete fVolumetricSourceDirectory;
    delete fSetVolumetricSourceVolumeNameCmd;
    delete fSetVolumetricSourcePDGCodeCmd;
    delete fSetVolumetricSourceTypeCmd;
    delete fSetVolumetricSourceSpectrumFilenameCmd;
    delete fSetVolumetricSourceSpectrumHistNameCmd;
    delete fSetVolumetricSourceMonoEnergyCmd;
    //
    //Disk source
    delete fDiskSourceGeneratorDirectory;
    delete fSetDiskSourcePDGCodeCmd;
    delete fSetDiskSourceTypeCmd;
    delete fSetDiskSourceSpectrumFilenameCmd;
    delete fSetDiskSourceSpectrumHistNameCmd;
    delete fSetDiskSourceMonoEnergyCmd;
    delete fSetDiskSourceRadiusCmd;
    delete fSetDiskSourcePositionCmd;
    delete fSetDiskSourceAxisCmd;
    delete fSetDiskSourceDirectionCmd;
}

void PaleoSimMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    //Geometry
    if (command == fSetGeometryMacroCmd) {
        G4String geomPath;
        G4String messengerPath = __FILE__;
        G4String sourceDir = messengerPath.substr(0, messengerPath.find_last_of("/"));
        geomPath = sourceDir + "/.." + newValue;
        G4cout << "Opening geometry file from " << geomPath << "\n" << G4endl;


        std::ifstream file(geomPath);
        if (!file.good()) {
            G4Exception("PaleoSimMessenger", "InvalidGeometryMacro", FatalException,
                        ("Specified geometry macro file does not exist: " + newValue).c_str());
        }
        fGeometryMacroPath = geomPath;
        G4cout << "Geometry macro file set in macro to: " << geomPath << G4endl;
    }
    
    // Output
    else if (command == fSetPrimariesTreeStatusCmd) {
        fPrimariesTreeStatus = fSetPrimariesTreeStatusCmd->GetNewBoolValue(newValue);
        G4cout << "Primaries tree output set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetMINTreeStatusCmd) {    
        fMINTreeStatus = fSetMINTreeStatusCmd->GetNewBoolValue(newValue);
        G4cout << "Muon-induced neutron tree output set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetNeutronTallyTreeVolumesCmd) {
        std::istringstream iss(newValue);
        G4String name;
        G4cout << "Neutron tally tree will track neutrons entering volumes: " << G4endl;
        while (iss >> name) {
            fNeutronTallyTreeVolumes.push_back(name);
            G4cout << "\t" << name << G4endl;
        }
    }
    else if (command == fSetRecoilTreeVolumesCmd) {
        std::istringstream iss(newValue);
        G4String name;
        G4cout << "Recoil tree will track nuclear recoils in volumes: " << G4endl;
        while (iss >> name) {
            fRecoilTreeVolumes.push_back(name);
            G4cout << "\t" << name << G4endl;
        }
    }
    else if (command == fNPSCmd) {
        fNPS = std::stoll(newValue);
        G4cout << "NPS set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetVRMLStatusCmd) {
        fVRMLStatus = fSetVRMLStatusCmd->GetNewBoolValue(newValue);
        G4cout << "VRML file will be written in current directory"<< G4endl;
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
    else if (command == fMeiHimeMuonDepthCmd) {
        fMeiHimeMuonDepth = fMeiHimeMuonDepthCmd->GetNewDoubleValue(newValue);
        G4cout << "Effective muon depth set in macro to: " << newValue << G4endl;
    }
    //Mute
    else if (command == fSetMuteHistFilenameCmd) {
        fMuteHistFilename = newValue;
        G4cout << "Mei Hist filename set to " << newValue << G4endl;
        if (fMuteHistFilename.empty()) {
            G4Exception("SetNewValue", "EmptyMuteFile", FatalException,
                        "/generator/muteGenerator/setMuteHistFilename needs an argument");
        }
    }
    //CRY
    else if (command == fSetCRYFilenameCmd) {
        fCRYFilename = newValue;
        G4cout << "Cry filename set to " << newValue << G4endl;
        if (fCRYFilename.empty()) {
            G4Exception("SetNewValue", "EmptyCryFile", FatalException,
                        "/generator/cry/setCRYFilename needs an argument");
        }
    }
    //Volumetric source
    else if (command == fSetVolumetricSourceVolumeNameCmd) {
        fVolumetricSourceVolumeName = newValue;
        G4cout << "Volumetric source volume set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetVolumetricSourcePDGCodeCmd) {
        fVolumetricSourcePDGCode = fSetVolumetricSourcePDGCodeCmd->GetNewIntValue(newValue);
        G4cout << "Volumetric source PDG code set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetVolumetricSourceTypeCmd) {
        fVolumetricSourceType = newValue;
        if (fVolumetricSourceType.empty()) {
            G4Exception("SetNewValue", "EmptyVolumetricSourceCmd", FatalException,
                        "/generator/VolumetricSource/setVolumetricSourceType needs an argument ('mono' or 'hist')");
        }
        if (fVolumetricSourceType!="mono" && fVolumetricSourceType!="hist") {
            G4Exception("SetNewValue", "BadVolumetricSourceType", FatalException,
                        "/generator/VolumetricSource/setVolumetricSourceType acceptable commands are 'mono' or 'hist'");
        }
        G4cout << "Volumetric source type set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetVolumetricSourceSpectrumFilenameCmd) {
        fVolumetricSourceSpectrumFilename = newValue;
        if (fVolumetricSourceSpectrumFilename.empty()) {
            G4Exception("SetNewValue", "EmtpyVolumetricSourceFilename", FatalException,
                        "/generator/VolumetricSource/setVolumetricSourceFilename needs an argument");
        }
        G4cout << "Volumetric source filename set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetVolumetricSourceSpectrumHistNameCmd) {
        fVolumetricSourceSpectrumHistName = newValue;
        if (fVolumetricSourceSpectrumHistName.empty()) {
            G4Exception("SetNewValue", "EmtpyVolumetricSourceHistName", FatalException,
                        "/generator/VolumetricSource/setVolumetricSourceHistName needs an argument");
        }
        G4cout << "Volumetric source hist name set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetVolumetricSourceMonoEnergyCmd) {
        fVolumetricSourceMonoEnergy = fSetVolumetricSourceMonoEnergyCmd->GetNewDoubleValue(newValue);
        G4cout << "Volumetric source energy set in macro to: " << newValue << G4endl;
    }
    //Disk source
    else if (command == fSetDiskSourcePDGCodeCmd) {
        fDiskSourcePDGCode = fSetDiskSourcePDGCodeCmd->GetNewIntValue(newValue);
        G4cout << "Disk source PDG code set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetDiskSourceTypeCmd) {
        fDiskSourceType = newValue;
        if (fDiskSourceType.empty()) {
            G4Exception("SetNewValue", "EmptyDiskSourceCmd", FatalException,
                        "/generator/diskSource/setDiskSourceType needs an argument ('mono' or 'hist')");
        }
        if (fDiskSourceType!="mono" && fDiskSourceType!="hist") {
            G4Exception("SetNewValue", "BadDiskSourceType", FatalException,
                        "/generator/diskSource/setDiskSourceType acceptable commands are 'mono' or 'hist'");
        }
        G4cout << "Disk source type set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetDiskSourceSpectrumFilenameCmd) {
        fDiskSourceSpectrumFilename = newValue;
        if (fDiskSourceSpectrumFilename.empty()) {
            G4Exception("SetNewValue", "EmtpyDiskSourceFilename", FatalException,
                        "/generator/diskSource/setDiskSourceFilename needs an argument");
        }
        G4cout << "Disk source filename set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetDiskSourceSpectrumHistNameCmd) {
        fDiskSourceSpectrumHistName = newValue;
        if (fDiskSourceSpectrumHistName.empty()) {
            G4Exception("SetNewValue", "EmtpyDiskSourceHistName", FatalException,
                        "/generator/diskSource/setDiskSourceHistName needs an argument");
        }
        G4cout << "Disk source hist name set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetDiskSourceMonoEnergyCmd) {
        fDiskSourceMonoEnergy = fSetDiskSourceMonoEnergyCmd->GetNewDoubleValue(newValue);
        G4cout << "Disk source energy set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetDiskSourceRadiusCmd) {
        fDiskSourceRadius = fSetDiskSourceRadiusCmd->GetNewDoubleValue(newValue);
        G4cout << "Disk source radius set in macro to: " << newValue << G4endl;
    }
    else if (command == fSetDiskSourcePositionCmd) {
        fDiskSourcePosition = fSetDiskSourcePositionCmd->GetNew3VectorValue(newValue);
    }
    else if (command == fSetDiskSourceAxisCmd) {
        fDiskSourceAxis = fSetDiskSourceAxisCmd->GetNew3VectorValue(newValue);
    }
    else if (command == fSetDiskSourceDirectionCmd) {
        fDiskSourceDirection = fSetDiskSourceDirectionCmd->GetNew3VectorValue(newValue);
    }
}

/////////////////////////////
///////ERROR CHECKER/////////
/////////////////////////////
void PaleoSimMessenger::CheckForMacroErrors() {
    //Check the user-specified volume for neutronTallyTree exists. If so, enable output
    for (auto volumeName: fNeutronTallyTreeVolumes) {
        if (!PaleoSimMessenger::VolumeNameExists(volumeName)) {
            G4Exception("PaleoSimMessenger", "BadNeutronTallyTreeVolume", FatalException,
            ("Tried to track neutrons entering "+volumeName+" but volume not found in geometry!").c_str());
        }
    }
    if (fNeutronTallyTreeVolumes.size()>0) {
        fNeutronTallyTreeStatus = true;
    }

    //Check the user-specified volume for neutronTallyTree exists. If so, enable output

    for (auto volumeName: fRecoilTreeVolumes) {
        if (!PaleoSimMessenger::VolumeNameExists(volumeName)) {
            G4Exception("PaleoSimMessenger", "BadRecoilTreeVolume", FatalException,
            ("Tried to track recoils in "+volumeName+" but volume not found in geometry!").c_str());
        }
    }
    if (fRecoilTreeVolumes.size()>0) {
        fRecoilTreeStatus = true;
    }

    //Make sure we have at least one volume
    if (fVolumes.size()==0) {
        G4Exception("PaleoSimMessenger", "No volumes", FatalException,
                        "No volumes found!");
    }
}
