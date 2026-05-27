#include "PaleoSimOutputManager.hh"
#include "PaleoSimRootOutputWriter.hh"
#include "PaleoSimH5OutputWriter.hh"

#include "TFile.h"
#include "TTree.h"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"
#include <stdexcept>
#include <sys/stat.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>

#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4UImanager.hh"


PaleoSimOutputManager::PaleoSimOutputManager(PaleoSimMessenger& messenger)
    : fMessenger(messenger) {}

void PaleoSimOutputManager::WriteAndClose() {
  const auto fmt = fMessenger.GetOutputFormat();
  if (fmt == "root") {
    PaleoSimRootOutputWriter::Write(*this);
  } else if (fmt == "h5") {
    PaleoSimH5OutputWriter::Write(*this);
  } else {
    G4Exception("PaleoSimOutputManager", "UnknownOutputFormat", FatalException,
                ("Unknown output format: " + std::string(fmt)).c_str());
  }
}

void PaleoSimOutputManager::CreateOutputFileAndTrees() {
  G4String outputPath = fMessenger.GetOutputPath();

  std::string outputDir;
  size_t slashPos = outputPath.find_last_of("/\\");
  if (slashPos != std::string::npos) outputDir = outputPath.substr(0, slashPos);
  else outputDir = ".";

  if (!outputDir.empty() && outputDir != ".") {
    struct stat info;
    bool pathExists = stat(outputDir.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
    if (!pathExists) {
      G4Exception("PaleoSimOutputManager", "outputFolderMissing", FatalException,
                  ("Specified output folder does not exist: " + outputDir).c_str());
    }
  }

  const auto fmt = fMessenger.GetOutputFormat();
  if (fmt == "root") {
    fFile = new TFile(outputPath.c_str(), "RECREATE");
    if (!fFile || fFile->IsZombie()) {
      throw std::runtime_error("Failed to create ROOT output file.");
    }
  } else if (fmt == "h5") {
    fFile = nullptr;
  } else {
    G4Exception("PaleoSimOutputManager", "UnknownOutputFormat", FatalException,
                ("Unknown output format: " + std::string(fmt)).c_str());
  }

  fHeaderTree = new TTree("headerTree","Run meta information");

  long long nps = fMessenger.GetNPS();
  char sourceType[256] = "";
  std::strncpy(sourceType, fMessenger.GetSourceType().c_str(), 255);

  fHeaderTree->Branch("nps", &nps);
  fHeaderTree->Branch("sourceType", &sourceType, "sourceType/C");

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
  fHeaderTree->Fill();

  fGeometryTree = new TTree("fGeometryTree","Run geometry");

  char volumeName[256] = "";
  char volumeShape[256] = "";
  char parentName[256] = "";
  char materialName[256] = "";
  int geomNumber;
  double geomAbsX, geomAbsY, geomAbsZ;
  std::vector<double> geomXs, geomYs, geomZs;

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
    std::strncpy(volumeName, vol->name.c_str(), 255);
    std::strncpy(volumeShape, vol->shape.c_str(), 255);
    std::strncpy(parentName, vol->parentName.c_str(), 255);
    std::strncpy(materialName, vol->materialName.c_str(), 255);
    geomAbsX = vol->absolutePosition.x();
    geomAbsY = vol->absolutePosition.y();
    geomAbsZ = vol->absolutePosition.z();
    geomNumber = vol->volumeNumber;

    int nPoints = 5000;
    geomXs.clear(); geomYs.clear(); geomZs.clear();
    if (vol->parentName != "None") {
      for (int pointNum = 0; pointNum < nPoints; pointNum++) {
        G4ThreeVector randPos = vol->GenerateRandomPointInside();
        geomXs.push_back(randPos.x());
        geomYs.push_back(randPos.y());
        geomZs.push_back(randPos.z());
      }
    }
    fGeometryTree->Fill();
  }

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
    fPrimariesTree->Branch("muonTheta", &fPrimaryMuonTheta);
    fPrimariesTree->Branch("muonPhi", &fPrimaryMuonPhi);
    fPrimariesTree->Branch("muonSlant", &fPrimaryMuonSlant);
    fPrimariesTree->Branch("CRYCoreX", &fCRYCoreX);
    fPrimariesTree->Branch("CRYCoreY", &fCRYCoreY);
    fPrimariesTree->Branch("CRYCoreZ", &fCRYCoreZ);
    fPrimariesTree->Branch("CRYCoreTheta", &fCRYCoreTheta);
    fPrimariesTree->Branch("CRYCorePhi", &fCRYCorePhi);
    fPrimariesTree->Branch("CRYTotalEnergy", &fCRYTotalEnergy);
  }

  if (fMessenger.GetMINTreeStatus()) {
    fMINTree = new TTree("MINTree", "Muon-induced neutrons");
    fMINTree->Branch("eventID", &fMINEventID);
    fMINTree->Branch("multiplicity", &fMINEventMultiplicity);
    fMINTree->Branch("angleRelToMuon", &fMINEventAngleRelMuon);
    fMINTree->Branch("energy", &fMINEventEnergy);
    fMINTree->Branch("distanceToMuonTrack", &fMINEventDistanceToMuonTrack);
  }

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
    fNeutronTallyTree->Branch("prevVolumeNumbers", &fPrevNeutronTallyVolumeNumbers);
  }

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

void PaleoSimOutputManager::FillPrimariesTreeEvent() {
  if (!fMessenger.GetPrimariesTreeStatus() || !fPrimariesTree) return;
  if (fPrimaryPdgID.empty()) return;
  fPrimariesTree->Fill();
}

void PaleoSimOutputManager::FillMINTreeEvent() {
  if (!fMessenger.GetMINTreeStatus() || !fMINTree) return;
  if (fMINEventMultiplicity == 0) return;
  fMINTree->Fill();
}

void PaleoSimOutputManager::FillNeutronTallyTreeEvent() {
  if (!fMessenger.GetNeutronTallyTreeStatus() || !fNeutronTallyTree) return;
  if (fNeutronEntryMultiplicity == 0) return;
  fNeutronTallyTree->Fill();
}

void PaleoSimOutputManager::FillRecoilTreeEvent() {
  if (!fMessenger.GetRecoilTreeStatus() || !fRecoilTree) return;
  if (fNRecoils == 0) return;
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
  fPrevNeutronTallyVolumeNumbers.clear();
}

void PaleoSimOutputManager::ClearRecoilTreeEvent() {
  fRecoilEventID = -1;
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

void PaleoSimOutputManager::WriteVRMLGeometry(const G4String& vrmlFilename) {
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
