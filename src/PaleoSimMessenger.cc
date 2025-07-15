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

    fSetNeutronTallyTreeVolumeCmd = new G4UIcmdWithAString("/output/setNeutronTallyTreeVolume", this);
    fSetNeutronTallyTreeVolumeCmd->SetGuidance("Set to track particles entering volume in neutronTallyTree");
    fSetNeutronTallyTreeVolumeCmd->SetParameterName("fNeutronTallyTreeVolume", true);
    fSetNeutronTallyTreeVolumeCmd->SetDefaultValue(fNeutronTallyTreeVolume);

    fSetRecoilTreeVolumeCmd = new G4UIcmdWithAString("/output/setRecoilTreeVolume", this);
    fSetRecoilTreeVolumeCmd->SetGuidance("Set to track heavy secondaries in volume in recoilTree");
    fSetRecoilTreeVolumeCmd->SetParameterName("fRecoilTreeVolume", true);
    fSetRecoilTreeVolumeCmd->SetDefaultValue(fRecoilTreeVolume);

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

    fNPSCmd = new G4UIcmdWithAnInteger("/generator/setNPS", this);    
    fNPSCmd->SetGuidance("Set number of events to throw");
    fNPSCmd->SetParameterName("fNPS", true);
    fNPSCmd->SetDefaultValue(fNPS);

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
    delete fSetNeutronTallyTreeVolumeCmd;
    delete fSetRecoilTreeVolumeCmd;
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
    else if (command == fSetNeutronTallyTreeVolumeCmd) {
        fNeutronTallyTreeVolume = newValue;
        G4cout << "Neutron tally tree will track neutrons entering volume: " << newValue << G4endl;
    }
    else if (command == fSetRecoilTreeVolumeCmd) {
        fRecoilTreeVolume = newValue;        
        G4cout << "Recoil tally tree will track heavy aprticles in volume: " << newValue << G4endl;
    }
    else if (command == fNPSCmd) {
        fNPS = fNPSCmd->GetNewIntValue(newValue);
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
    if (PaleoSimMessenger::VolumeNameExists(fNeutronTallyTreeVolume)) {
        fNeutronTallyTreeStatus = true;
    }
    else if (!fNeutronTallyTreeVolume.empty()){
        G4Exception("PaleoSimMessenger", "BadNeutronTallyTreeVolume", FatalException,
            ("Tried to track neutrons entering "+fNeutronTallyTreeVolume+" but volume not found in geometry!").c_str());
    }
    //Check the user-specified volume for neutronTallyTree exists. If so, enable output
    if (PaleoSimMessenger::VolumeNameExists(fRecoilTreeVolume)){
        fRecoilTreeStatus = true;
    }
    else if (!fRecoilTreeVolume.empty()){
        G4Exception("PaleoSimMessenger", "BadRecoilTreeVolume", FatalException,
            ("Tried to track heavy particles in "+fRecoilTreeVolume+" but volume not found in geometry!").c_str());
    }

    //Make sure we have at least one volume
    if (fVolumes.size()==0) {
        G4Exception("PaleoSimMessenger", "No volumes", FatalException,
                        "No volumes found!");
    }

    //Check for duplicate cells
    std::set<G4String> volumeNames;
    for (const auto& volume : fVolumes) {
        const G4String& name = volume->name;
        if (volumeNames.find(name) != volumeNames.end()) {
            G4Exception("PaleoSimMessenger", "DuplicateName", FatalException,
                        ("Duplicate volume name: " + name).c_str());
        }
        volumeNames.insert(name);
    }

    //Check for exactly one world volume    
    int nWorldVolumes=0;
    for (const auto& volume : fVolumes) {
        const G4String& parentName = volume->parentName;
        if (parentName=="None"){
            nWorldVolumes++;
        }
    }

    if (nWorldVolumes == 0) {
        G4Exception("PaleoSimMessenger", "NoRoot", FatalException,
                    "No world volume. One volume must be the world (Parent = 'None' or omitted).");
    } else if (nWorldVolumes > 1) {
        G4Exception("PaleoSimMessenger", "MultipleRoots", FatalException,
                    "Multiple root volumes found. Only one world volume is allowed.");
    }

    //Check parent volumes actually exist, and not set as its own parent
    for (const auto& volume : fVolumes) {
        const G4String& parent = volume->parentName;

        if (parent=="None") continue;

        if (volumeNames.find(parent) == volumeNames.end()) {
            G4Exception("PaleoSimMessenger", "MissingParent", FatalException,
                        ("Parent not found: " + parent + " for volume " + volume->name).c_str());
        }

        if (parent == volume->name) {
            G4Exception("PaleoSimMessenger", "SelfParent", FatalException,
                        ("Volume cannot parent itself: " + volume->name).c_str());
        }
    }

    // World volume must be placed at absolute (0,0,0)
    for (const auto& volume : fVolumes) {
        if (volume->parentName == "None") {
            if (volume->userPosition != G4ThreeVector(0, 0, 0)) {
                G4Exception("PaleoSimMessenger", "WorldNotAtOrigin", FatalException,
                            ("World volume '" + volume->name + "' must be positioned at (0,0,0)").c_str());
            }
            break; // already verified there’s only one world
        }
    }
}

//Relative coordinates are pretty insane to deal with. We supply them in absolute coordinate
//in our geometry file because that makes sense. This function converts from
//supplied absolute coordinates to relative ones, continuing up the parent
//chain until we get the world, which cannot have a rotation/translation
// Computes global position by walking up the parent chain

void PaleoSimMessenger::ComputeCoordinates() {
    //First compute absolute coordinates for all volumes
    for (auto* vol : fVolumes) {
        ComputeAbsoluteCoordinatesRecursive(vol);
    }

    //Then, use those to compute relative coordinates for all volumes
    for (auto* vol : fVolumes) {
        ComputeRelativeCoordinatesRecursive(vol);
    }

    //Compute rotations because of the whole awkward cylinder definition scheme in geant4
    for (auto* vol : fVolumes) {
        ComputeCumulativeRotationsRecursive(vol);
    }
}

void PaleoSimMessenger::ComputeAbsoluteCoordinatesRecursive(PaleoSimVolumeDefinition* vol) {
    if (vol->parentName =="None" || vol->positionType =="absolute") return;
    if (vol->absolutePosition != G4ThreeVector(0, 0, 0)) return;

    PaleoSimVolumeDefinition* parent = GetVolumeByName(vol->parentName);    
    
    ComputeAbsoluteCoordinatesRecursive(parent); 

    vol->absolutePosition = parent->absolutePosition + vol->relativePosition;
}

void PaleoSimMessenger::ComputeRelativeCoordinatesRecursive(PaleoSimVolumeDefinition* vol) {
    if (vol->parentName == "None") return;
    if (vol->positionType == "relative") return;

    PaleoSimVolumeDefinition* parent = GetVolumeByName(vol->parentName);

    vol->relativePosition = vol->absolutePosition - parent->absolutePosition;
}

void PaleoSimMessenger::ComputeCumulativeRotationsRecursive(PaleoSimVolumeDefinition* vol) {
    if (vol->parentName == "None") return;  

    PaleoSimVolumeDefinition* parent = GetVolumeByName(vol->parentName);
    ComputeCumulativeRotationsRecursive(parent); // ensure parent's rotation is set

    vol->cumulativeRotationMatrix = parent->cumulativeRotationMatrix;

    if (vol->shape == "cylinder") {
        //All G4 cylinders generated w.r.t z-axis. We need to compute the rotation
        //to align it with our user-specified absolute axis.
        const G4ThreeVector zAxis(0, 0, 1);
        G4ThreeVector target = vol->absoluteCylinderAxis.unit();

        // Rotation to align Z with the desired axis
        G4ThreeVector rotationAxis = zAxis.cross(target);
        double angle = std::acos(zAxis.dot(target));

        if (rotationAxis.mag2() < 1e-12) {
            // Axis already aligned or 180° flip
            if (angle < 1e-12) {
                vol->rotationMatrix = G4RotationMatrix(); // no rotation
            } else {
                // 180° rotation about any perpendicular axis (e.g., X)
                vol->rotationMatrix = G4RotationMatrix();
                vol->rotationMatrix.rotateX(CLHEP::pi);
            }
        } else {
            rotationAxis = rotationAxis.unit();
            vol->rotationMatrix = G4RotationMatrix();
            vol->rotationMatrix.rotate(angle, rotationAxis);
        }

        // Update cumulative rotation
        vol->cumulativeRotationMatrix = vol->rotationMatrix * vol->cumulativeRotationMatrix;
    }
}