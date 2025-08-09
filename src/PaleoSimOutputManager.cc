#include "PaleoSimOutputManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "G4SystemOfUnits.hh"
#include <stdexcept>
#include <sys/stat.h> 
#include <thread>
#include <chrono>

#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4UImanager.hh"

//Constructor
PaleoSimOutputManager::PaleoSimOutputManager(PaleoSimMessenger& messenger)
    : fMessenger(messenger) {}

//Write and close
void PaleoSimOutputManager::WriteAndClose() {
  if (fFile) {
      fFile->cd();
      fHeaderTree->Write("headerTree", TFile::kOverwrite); 
      fGeometryTree->Write("geometryTree", TFile::kOverwrite); 
      if (fMessenger.GetPrimariesTreeStatus() && fPrimariesTree) {
          fPrimariesTree->Write("primariesTree", TFile::kOverwrite); 
      }
      if (fMessenger.GetMINTreeStatus() && fMINTree) {
        fMINTree->Write("MINTree", TFile::kOverwrite);
      }
      if (fMessenger.GetNeutronTallyTreeStatus() && fNeutronTallyTree) {
          fNeutronTallyTree->Write("neutronTallyTree", TFile::kOverwrite); 
      }
      if (fMessenger.GetRecoilTreeStatus() && fRecoilTree) {
        fRecoilTree->Write("recoilTree", TFile::kOverwrite); 
      }
      fFile->Close();
      delete fFile;
      fFile = nullptr;
  }
  fHeaderTree = nullptr;
  fGeometryTree = nullptr;
  fMINTree = nullptr;
  fPrimariesTree = nullptr;
  fNeutronTallyTree = nullptr;
  fRecoilTree = nullptr;
}


//Create files and trees
void PaleoSimOutputManager::CreateOutputFileAndTrees() {

  // Create files
  G4String outputPath = fMessenger.GetOutputPath();

  // Check if the specified output directory exists; if it does not, create it

  std::string outputDir;
  size_t slashPos = outputPath.find_last_of("/\\");
  if (slashPos != std::string::npos) {
      outputDir = outputPath.substr(0, slashPos);
  } else {
      outputDir = ".";  // No slash means current directory
  }

  if (!outputDir.empty() && outputDir != ".") {
    struct stat info;
    bool pathExists = stat(outputDir.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
    if (!pathExists) {
      G4Exception("PaleoSimOutputManager", "outputFolderMissing", FatalException,
                  ("Specified output folder does not exist: " + outputDir).c_str());
    }
  }

  //Make output file
  fFile = new TFile(outputPath.c_str(), "RECREATE");
  if (!fFile || fFile->IsZombie()) {
      throw std::runtime_error("Failed to create output file.");
  }

  ////////////////////
  //Make header tree//
  ////////////////////
  fHeaderTree = new TTree("headerTree","Run meta information");

  long long nps = fMessenger.GetNPS();
  char sourceType[256] = "";
  strcpy(sourceType, fMessenger.GetSourceType().substr(0, std::min<size_t>(255, fMessenger.GetSourceType().length())).c_str());

  // Set branches
  fHeaderTree->Branch("nps", &nps);
  fHeaderTree->Branch("sourceType", &sourceType, "sourceType/C");
  //Add your own generator commands here
  //CUSTOM_GENERATOR_HOOK
  //
  //Mei & Hime muon generator
  if (fMessenger.GetSourceType()=="meiHimeMuonGenerator") {
    double meiHimeMuonEffectiveDepth = fMessenger.GetMeiHimeMuonEffectiveDepth();
    fHeaderTree->Branch("meiHimeMuonEffectiveDepth_mm", &meiHimeMuonEffectiveDepth);
    double meiHimeFluxNormalization = fMessenger.GetMeiHimeFluxNormalization();
    fHeaderTree->Branch("meiHimeFluxNormalization_per_cm2_per_s", &meiHimeFluxNormalization);
  }
  if (fMessenger.GetSourceType()=="CRYGenerator") {
    double CRYAltitude = fMessenger.GetCRYAltitude();
    double CRYLatitude = fMessenger.GetCRYLatitude();
    double CRYNorm = fMessenger.GetCRYNorm();
    fHeaderTree->Branch("CRYAltitude_m", &CRYAltitude);
    fHeaderTree->Branch("CRYLatitude", &CRYLatitude);
    fHeaderTree->Branch("primaries_per_cm2_per_s", &CRYNorm);
  }
  // Fill once when we make the tree, we aren't ever updating this
  fHeaderTree->Fill();

  ////////////////////////
  // MAKE GEOMETRY TREE //
  ////////////////////////
  fGeometryTree = new TTree("fGeometryTree","Run geometry");

  //Branch vars
  char volumeName[256] = "";
  char volumeShape[256] = "";
  char parentName[256] = "";
  char materialName[256] = "";
  int geomNumber;
  double geomAbsX, geomAbsY, geomAbsZ;
  std::vector<double> geomXs, geomYs, geomZs;

  // Set branches
  fGeometryTree->Branch("name", &volumeName, "name/C");
  fGeometryTree->Branch("shape", &volumeShape, "shape/C");
  fGeometryTree->Branch("parent", &parentName, "parent/C");
  fGeometryTree->Branch("material", &materialName, "material/C");
  fGeometryTree->Branch("number", &geomNumber, "number/I");
  fGeometryTree->Branch("abs_x", &geomAbsX, "abs_x/D");
  fGeometryTree->Branch("abs_y", &geomAbsY, "abs_y/D");
  fGeometryTree->Branch("abs_z", &geomAbsZ, "abs_z/D");
  fGeometryTree->Branch("pointCloud_xs", &geomXs);
  fGeometryTree->Branch("pointCloud_ys", &geomYs);
  fGeometryTree->Branch("pointCloud_zs", &geomZs);

  for (auto* vol : fMessenger.GetVolumes()) {
    strcpy(volumeName, vol->name.substr(0, std::min<size_t>(255, vol->name.length())).c_str());
    strcpy(volumeShape, vol->shape.substr(0, std::min<size_t>(255, vol->shape.length())).c_str());
    strcpy(parentName, vol->parentName.substr(0, std::min<size_t>(255, vol->parentName.length())).c_str());
    strcpy(materialName, vol->materialName.substr(0, std::min<size_t>(255, vol->materialName.length())).c_str());
    geomAbsX = vol->absolutePosition.x();
    geomAbsY = vol->absolutePosition.y();
    geomAbsZ = vol->absolutePosition.z();
    geomNumber = vol->volumeNumber;
    int nPoints = 5000;
    geomXs.clear();
    geomYs.clear();
    geomZs.clear();
    if (vol->parentName != "None") {
      for (int pointNum=0; pointNum<nPoints; pointNum++) {
        G4ThreeVector randPos = vol->GenerateRandomPointInside();
        geomXs.push_back(randPos.x());
        geomYs.push_back(randPos.y());
        geomZs.push_back(randPos.z());
      }
    }
    fGeometryTree->Fill();
  }


  /////////////////////////
  // MAKE PRIMARIES TREE //
  /////////////////////////
  if (fMessenger.GetPrimariesTreeStatus()) {
    fPrimariesTree = new TTree("primariesTree", "Generated primary particles");

    fPrimariesTree->Branch("eventID", &fPrimaryEventID);
    fPrimariesTree->Branch("pdgID", &fPrimaryPdgID);
    fPrimariesTree->Branch("energy", &fPrimaryEnergy);
    fPrimariesTree->Branch("x", &fPrimaryX);
    fPrimariesTree->Branch("y", &fPrimaryY);
    fPrimariesTree->Branch("z", &fPrimaryZ);
    fPrimariesTree->Branch("px", &fPrimaryPx);
    fPrimariesTree->Branch("py", &fPrimaryPy);
    fPrimariesTree->Branch("pz", &fPrimaryPz);
    //CUSTOM_GENERATOR_HOOK 
    //Add branches stored to primary tree here
    //
    // Mei & Hime muon generator - first two also used for mute generator
    fPrimariesTree->Branch("muonTheta", &fPrimaryMuonTheta);
    fPrimariesTree->Branch("muonPhi", &fPrimaryMuonPhi);
    fPrimariesTree->Branch("muonSlant", &fPrimaryMuonSlant);
    //
    //CRY
    fPrimariesTree->Branch("CRYCoreX", &fCRYCoreX);
    fPrimariesTree->Branch("CRYCoreY", &fCRYCoreY);
    fPrimariesTree->Branch("CRYCoreZ", &fCRYCoreZ);
    fPrimariesTree->Branch("CRYCoreTheta", &fCRYCoreTheta);
    fPrimariesTree->Branch("CRYCorePhi", &fCRYCorePhi);
    fPrimariesTree->Branch("CRYTotalEnergy", &fCRYTotalEnergy);
  }

  ///////////////////////////////////
  // MAKE MUON-INDUCED NEUTRON TREE//
  ///////////////////////////////////
  if (fMessenger.GetMINTreeStatus()) {
    fMINTree = new TTree("MINTree", "Muon-induced neutrons");

    fMINTree->Branch("eventID", &fMINEventID);
    fMINTree->Branch("multiplicity", &fMINEventMultiplicity);
    fMINTree->Branch("angleRelToMuon", &fMINEventAngleRelMuon);
    fMINTree->Branch("energy", &fMINEventEnergy);
    fMINTree->Branch("distanceToMuonTrack", &fMINEventDistanceToMuonTrack);
  }

  /////////////////////////////
  // MAKE NEUTRON TALLY TREE //
  ////////////////////////////
  if (fMessenger.GetNeutronTallyTreeStatus()) {
    fNeutronTallyTree = new TTree("neutronTallyTree", "Muon-induced neutrons entering cavity");

    fNeutronTallyTree->Branch("eventID", &fNeutronTallyEventID);
    fNeutronTallyTree->Branch("numNeutronsEntered", &fNeutronEntryMultiplicity);
    fNeutronTallyTree->Branch("entry_energy", &fNeutron_entryEnergy);
    fNeutronTallyTree->Branch("entry_x", &fNeutron_entryX);
    fNeutronTallyTree->Branch("entry_y", &fNeutron_entryY);
    fNeutronTallyTree->Branch("entry_z", &fNeutron_entryZ);
    fNeutronTallyTree->Branch("entry_u", &fNeutron_entryU);
    fNeutronTallyTree->Branch("entry_v", &fNeutron_entryV);
    fNeutronTallyTree->Branch("entry_w", &fNeutron_entryW);
    fNeutronTallyTree->Branch("angleRelMuon", &fNeutron_angle);
    fNeutronTallyTree->Branch("distanceToMuonTrack", &fNeutron_distance);
    fNeutronTallyTree->Branch("volumeNumbers", &fNeutronTallyVolumeNumbers);
  }

  //////////////////////
  // MAKE RECOIL TREE //
  //////////////////////
  if (fMessenger.GetRecoilTreeStatus()) {
    fRecoilTree = new TTree("recoilTree", "Ion recoils in target");

    fRecoilTree->Branch("historyNum", &fRecoilEventID);
    fRecoilTree->Branch("pdgCode", &fRecoilEventPDGCode);
    fRecoilTree->Branch("parent_pdgCode", &fRecoilEventParentPDGCode);
    fRecoilTree->Branch("energy", &fRecoilEventEnergy);
    fRecoilTree->Branch("x", &fRecoilEventX);
    fRecoilTree->Branch("y", &fRecoilEventY);
    fRecoilTree->Branch("z", &fRecoilEventZ);
    fRecoilTree->Branch("u", &fRecoilEventU);
    fRecoilTree->Branch("v", &fRecoilEventV);
    fRecoilTree->Branch("w", &fRecoilEventW);
    fRecoilTree->Branch("time", &fRecoilEventTime);
    fRecoilTree->Branch("code", &fRecoilEventCode);
    fRecoilTree->Branch("nRecoils", &fNRecoils);
    fRecoilTree->Branch("volumeNumbers", &fRecoilVolumeNumbers);
  }
}

//Fill primaries tree
void PaleoSimOutputManager::FillPrimariesTreeEvent() {
    if (!fMessenger.GetPrimariesTreeStatus() || !fPrimariesTree) return;
    if (fPrimaryPdgID.size() == 0) return;
    fPrimariesTree->Fill();
}

void PaleoSimOutputManager::FillMINTreeEvent() {
    if (!fMessenger.GetMINTreeStatus() || !fMINTree) return;
    if (fMINEventMultiplicity == 0) return;
    fMINTree->Fill();
}

//Fill neutron tally tree
void PaleoSimOutputManager::FillNeutronTallyTreeEvent() {
    if (!fMessenger.GetNeutronTallyTreeStatus() || !fNeutronTallyTree) return;
    if (fNeutronEntryMultiplicity == 0) return;
    fNeutronTallyTree->Fill();
}

//Fill recoil tree
void PaleoSimOutputManager::FillRecoilTreeEvent() {
  if (!fMessenger.GetRecoilTreeStatus() || !fRecoilTree) return;
  if (fNRecoils==0) return;
  fRecoilTree->Fill();
}

void PaleoSimOutputManager::ClearPrimariesTreeEvent() {
  fPrimaryEventID = -1;
  fPrimaryPdgID.clear();
  fPrimaryEnergy.clear();
  fPrimaryX.clear();
  fPrimaryY.clear();
  fPrimaryZ.clear();
  fPrimaryPx.clear();
  fPrimaryPy.clear();
  fPrimaryPz.clear();
  //CUSTOM_GENERATOR_HOOK
  //Clear/reset vars here
}

void PaleoSimOutputManager::ClearMINTreeEvent() {
  fMINEventMultiplicity = 0;
  fMINEventAngleRelMuon.clear();
  fMINEventEnergy.clear();
  fMINEventDistanceToMuonTrack.clear();
}

void PaleoSimOutputManager::ClearNeutronTallyTreeEvent() {
  fNeutronTallyEventID = -1;
  fNeutronEntryMultiplicity = 0;
  fNeutron_entryEnergy.clear();
  fNeutron_entryX.clear();
  fNeutron_entryY.clear();
  fNeutron_entryZ.clear();
  fNeutron_entryU.clear();
  fNeutron_entryV.clear();
  fNeutron_entryW.clear();
  fNeutron_angle.clear();
  fNeutron_distance.clear();
  fNeutronTallyVolumeNumbers.clear();
}

void PaleoSimOutputManager::ClearRecoilTreeEvent() {
  fNeutronTallyEventID = -1;
  fNRecoils = 0;
  fRecoilEventPDGCode.clear();
  fRecoilEventParentPDGCode.clear();
  fRecoilEventEnergy.clear();
  fRecoilEventX.clear();
  fRecoilEventY.clear();
  fRecoilEventZ.clear();
  fRecoilEventU.clear();
  fRecoilEventV.clear();
  fRecoilEventW.clear();
  fRecoilEventTime.clear();
  fRecoilEventCode.clear();
  fRecoilVolumeNumbers.clear();
}

void PaleoSimOutputManager::WriteVRMLGeometry(const G4String& vrmlFilename)
{
    // Set environment variable to control VRML output filename
    setenv("G4VRMLFILE_FILE_NAME", vrmlFilename.c_str(), 1);

    auto* visManager = new G4VisExecutive();
    visManager->SetVerboseLevel(0);
    visManager->Initialize();

    auto* ui = G4UImanager::GetUIpointer();
    ui->ApplyCommand("/vis/open VRML2FILE");
    ui->ApplyCommand("/vis/scene/create");

    ui->ApplyCommand("/vis/drawVolume");
    ui->ApplyCommand("/vis/viewer/flush");
    ui->ApplyCommand("/vis/sceneHandler/flush");

    delete visManager;
}