#include "PaleoSimOutputManager.hh"

#include "G4RootAnalysisManager.hh"
#include "G4Exception.hh"
#include "G4Threading.hh"
#include "G4ThreeVector.hh"

#include <filesystem>
#include <string>

#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4UImanager.hh"


// Constructor
PaleoSimOutputManager::PaleoSimOutputManager(PaleoSimMessenger& messenger)
    : fMessenger(messenger) {}


// Book ntuples
void PaleoSimOutputManager::Book() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  // Merge worker ntuples into one ROOT file when running multithreaded
  fMergeNtuples = G4Threading::IsMultithreadedApplication();
  if (fMergeNtuples) analysisManager->SetNtupleMerging(true);

  //Create Trees
  BookHeader();
  BookGeometry();
  if (fMessenger.GetPrimariesTreeStatus()) {
    BookPrimaries();
  }

  if (fMessenger.GetMINTreeStatus()) {
    BookMIN();
  }
  if (fMessenger.GetNeutronTallyTreeStatus()) {
    BookNeutronTally();
  }
  if (fMessenger.GetSecondaryCaptureTreeStatus()) {
    BookSecondaryCapture();
  }
  if (fMessenger.GetRecoilTreeStatus()) {
    BookRecoil();
  }
}


// Open output file and write run-level information
void PaleoSimOutputManager::BeginOfRun() {
  //Get output file, check the path to it exists
  std::filesystem::path outputPath(fMessenger.GetOutputPath());
  std::filesystem::path outputDir = outputPath.parent_path();
  if (!G4Threading::IsWorkerThread()) {
    if (!outputDir.empty() && !std::filesystem::is_directory(outputDir)) {
      G4Exception("PaleoSimOutputManager","outputFolderMissing",FatalException,("Specified output folder does not exist: "+outputDir.string()).c_str());
    }
  }

  //Open output file
  auto* analysisManager = G4RootAnalysisManager::Instance();

  if (!analysisManager->OpenFile(outputPath.string())) {
    G4Exception("PaleoSimOutputManager","outputFileOpenFailed",FatalException,("Failed to create output file: "+outputPath.string()).c_str());
  }

  //Sequential runs write run-level ntuples here.
  //For MT, worker 0 writes the single copy that will be merged into the output file.
  if (!fMergeNtuples || (G4Threading::IsWorkerThread() && G4Threading::G4GetThreadId() == 0)) {
    FillRunLevelNtuples();
  }
}


// Write and close
void PaleoSimOutputManager::EndOfRun() {
  auto* analysisManager = G4RootAnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}


// Book header ntuple
void PaleoSimOutputManager::BookHeader() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fHeaderID = analysisManager->CreateNtuple("headerTree","Run meta information");

  fHeaderNpsCol = analysisManager->CreateNtupleDColumn(fHeaderID, "nps");
  fHeaderSourceTypeCol = analysisManager->CreateNtupleSColumn(fHeaderID, "sourceType");

  //Add your own generator commands here
  //CUSTOM_GENERATOR_HOOK
  //
  //Mei & Hime muon generator
  if (fMessenger.GetSourceType()=="meiHimeMuonGenerator") {
    fHeaderMeiHimeEffectiveDepthCol = analysisManager->CreateNtupleDColumn(fHeaderID, "meiHimeMuonEffectiveDepth_mm");
    fHeaderMeiHimeFluxNormalizationCol = analysisManager->CreateNtupleDColumn(fHeaderID, "meiHimeFluxNormalization_per_cm2_per_s");
  }

  //CRY generator
  if (fMessenger.GetSourceType()=="CRYGenerator") {
    fHeaderCRYAltitudeCol = analysisManager->CreateNtupleDColumn(fHeaderID, "CRYAltitude_m");
    fHeaderCRYLatitudeCol = analysisManager->CreateNtupleDColumn(fHeaderID, "CRYLatitude");
    fHeaderCRYNormCol = analysisManager->CreateNtupleDColumn(fHeaderID, "showers_per_cm2_per_s");
  }

  //Secondary capture generator
  if (fMessenger.GetSourceType()=="SCTGenerator") {
    fHeaderSCTCapturedParticlesCol = analysisManager->CreateNtupleIColumn(fHeaderID, "SCTCapturedParticles");
  }

  //MUTE generator
  if (fMessenger.GetSourceType()=="muteGenerator") {
    fHeaderMuteFluxNormalizationCol = analysisManager->CreateNtupleDColumn(fHeaderID, "muteFluxNormalization");
  }

  analysisManager->FinishNtuple(fHeaderID);
}


// Book geometry ntuple
void PaleoSimOutputManager::BookGeometry() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fGeometryID = analysisManager->CreateNtuple("fGeometryTree","Run geometry");

  fGeometryNameCol = analysisManager->CreateNtupleSColumn(fGeometryID, "name");
  fGeometryShapeCol = analysisManager->CreateNtupleSColumn(fGeometryID, "shape");
  fGeometryParentCol = analysisManager->CreateNtupleSColumn(fGeometryID, "parent");
  fGeometryMaterialCol = analysisManager->CreateNtupleSColumn(fGeometryID, "material");
  fGeometryNumberCol = analysisManager->CreateNtupleIColumn(fGeometryID, "number");
  fGeometryAbsXCol = analysisManager->CreateNtupleDColumn(fGeometryID, "abs_x");
  fGeometryAbsYCol = analysisManager->CreateNtupleDColumn(fGeometryID, "abs_y");
  fGeometryAbsZCol = analysisManager->CreateNtupleDColumn(fGeometryID, "abs_z");

  analysisManager->CreateNtupleDColumn(fGeometryID, "pointCloud_xs", fGeomXs);
  analysisManager->CreateNtupleDColumn(fGeometryID, "pointCloud_ys", fGeomYs);
  analysisManager->CreateNtupleDColumn(fGeometryID, "pointCloud_zs", fGeomZs);

  analysisManager->FinishNtuple(fGeometryID);
}


// Book primaries ntuple
void PaleoSimOutputManager::BookPrimaries() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fPrimariesID = analysisManager->CreateNtuple("primariesTree", "Generated primary particles");

  fPrimaryEventIDCol = analysisManager->CreateNtupleIColumn(fPrimariesID, "eventID");
  analysisManager->CreateNtupleIColumn(fPrimariesID, "pdgID", fPrimaryPdgID);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "energy", fPrimaryEnergy);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "x", fPrimaryX);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "y", fPrimaryY);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "z", fPrimaryZ);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "px", fPrimaryPx);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "py", fPrimaryPy);
  analysisManager->CreateNtupleDColumn(fPrimariesID, "pz", fPrimaryPz);

  //CUSTOM_GENERATOR_HOOK
  //Add branches stored to primary tree here
  //
  // Mei & Hime muon generator - first two also used for mute generator
  fPrimaryMuonThetaCol = analysisManager->CreateNtupleDColumn(fPrimariesID, "muonTheta");
  fPrimaryMuonPhiCol = analysisManager->CreateNtupleDColumn(fPrimariesID, "muonPhi");
  fPrimaryMuonSlantCol = analysisManager->CreateNtupleDColumn(fPrimariesID, "muonSlant");
  //

  analysisManager->FinishNtuple(fPrimariesID);
}


// Book muon-induced neutron ntuple
void PaleoSimOutputManager::BookMIN() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fMINID = analysisManager->CreateNtuple("MINTree", "Muon-induced neutrons");

  fMINEventIDCol = analysisManager->CreateNtupleIColumn(fMINID, "eventID");
  fMINMultiplicityCol = analysisManager->CreateNtupleIColumn(fMINID, "multiplicity");
  analysisManager->CreateNtupleDColumn(fMINID, "angleRelToMuon", fMINEventAngleRelMuon);
  analysisManager->CreateNtupleDColumn(fMINID, "energy", fMINEventEnergy);
  analysisManager->CreateNtupleDColumn(fMINID, "distanceToMuonTrack", fMINEventDistanceToMuonTrack);

  analysisManager->FinishNtuple(fMINID);
}


// Book neutron tally ntuple
void PaleoSimOutputManager::BookNeutronTally() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fNeutronTallyID = analysisManager->CreateNtuple("neutronTallyTree", "Muon-induced neutrons entering cavity");

  fNeutronTallyEventIDCol = analysisManager->CreateNtupleIColumn(fNeutronTallyID, "eventID");
  fNeutronTallyMultiplicityCol = analysisManager->CreateNtupleIColumn(fNeutronTallyID, "numNeutronsEntered");
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_energy", fNeutron_entryEnergy);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_x", fNeutron_entryX);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_y", fNeutron_entryY);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_z", fNeutron_entryZ);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_u", fNeutron_entryU);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_v", fNeutron_entryV);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "entry_w", fNeutron_entryW);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "angleRelMuon", fNeutron_angle);
  analysisManager->CreateNtupleDColumn(fNeutronTallyID, "distanceToMuonTrack", fNeutron_distance);
  analysisManager->CreateNtupleIColumn(fNeutronTallyID, "volumeNumbers", fNeutronTallyVolumeNumbers);
  analysisManager->CreateNtupleIColumn(fNeutronTallyID, "prevVolumeNumbers", fPrevNeutronTallyVolumeNumbers);

  analysisManager->FinishNtuple(fNeutronTallyID);
}


// Book secondary capture ntuple
void PaleoSimOutputManager::BookSecondaryCapture() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fSecondaryCaptureID = analysisManager->CreateNtuple("secondaryCaptureTree", "Muon-induced secondaries passing through boundary between volumes");

  fSecondaryCaptureEventIDCol = analysisManager->CreateNtupleIColumn(fSecondaryCaptureID, "eventID");
  analysisManager->CreateNtupleIColumn(fSecondaryCaptureID, "pdgCode", fSecondary_entryPDG);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "energy", fSecondary_entryEnergy);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "entry_x", fSecondary_entryX);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "entry_y", fSecondary_entryY);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "entry_z", fSecondary_entryZ);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "entry_u", fSecondary_entryU);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "entry_v", fSecondary_entryV);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "entry_w", fSecondary_entryW);
  analysisManager->CreateNtupleDColumn(fSecondaryCaptureID, "creation_z", fSecondary_creationZ);

  analysisManager->FinishNtuple(fSecondaryCaptureID);
}


// Book recoil ntuple
void PaleoSimOutputManager::BookRecoil() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  fRecoilID = analysisManager->CreateNtuple("recoilTree", "Ion recoils in target");

  fRecoilEventIDCol = analysisManager->CreateNtupleIColumn(fRecoilID, "historyNum");
  analysisManager->CreateNtupleIColumn(fRecoilID, "pdgCode", fRecoilEventPDGCode);
  analysisManager->CreateNtupleIColumn(fRecoilID, "parent_pdgCode", fRecoilEventParentPDGCode);
  analysisManager->CreateNtupleDColumn(fRecoilID, "energy", fRecoilEventEnergy);
  analysisManager->CreateNtupleDColumn(fRecoilID, "x", fRecoilEventX);
  analysisManager->CreateNtupleDColumn(fRecoilID, "y", fRecoilEventY);
  analysisManager->CreateNtupleDColumn(fRecoilID, "z", fRecoilEventZ);
  analysisManager->CreateNtupleDColumn(fRecoilID, "u", fRecoilEventU);
  analysisManager->CreateNtupleDColumn(fRecoilID, "v", fRecoilEventV);
  analysisManager->CreateNtupleDColumn(fRecoilID, "w", fRecoilEventW);
  analysisManager->CreateNtupleDColumn(fRecoilID, "time", fRecoilEventTime);
  analysisManager->CreateNtupleDColumn(fRecoilID, "code", fRecoilEventCode);
  fRecoilNRecoilsCol = analysisManager->CreateNtupleIColumn(fRecoilID, "nRecoils");
  analysisManager->CreateNtupleIColumn(fRecoilID, "volumeNumbers", fRecoilVolumeNumbers);

  analysisManager->FinishNtuple(fRecoilID);
}


// Fill run-level ntuples
void PaleoSimOutputManager::FillRunLevelNtuples() {
  auto* analysisManager = G4RootAnalysisManager::Instance();

  ////////////////////
  //Fill header tree//
  ////////////////////
  analysisManager->FillNtupleDColumn(fHeaderID, fHeaderNpsCol, static_cast<G4double>(fMessenger.GetNPS()));
  analysisManager->FillNtupleSColumn(fHeaderID, fHeaderSourceTypeCol, fMessenger.GetSourceType());

  //Add your own generator commands here
  //CUSTOM_GENERATOR_HOOK
  //
  //Mei & Hime muon generator
  if (fMessenger.GetSourceType()=="meiHimeMuonGenerator") {
    analysisManager->FillNtupleDColumn(fHeaderID, fHeaderMeiHimeEffectiveDepthCol, fMessenger.GetMeiHimeMuonEffectiveDepth());
    analysisManager->FillNtupleDColumn(fHeaderID, fHeaderMeiHimeFluxNormalizationCol, fMessenger.GetMeiHimeFluxNormalization());
  }

  //CRY generator
  if (fMessenger.GetSourceType()=="CRYGenerator") {
    analysisManager->FillNtupleDColumn(fHeaderID, fHeaderCRYAltitudeCol, fMessenger.GetCRYAltitude());
    analysisManager->FillNtupleDColumn(fHeaderID, fHeaderCRYLatitudeCol, fMessenger.GetCRYLatitude());
    analysisManager->FillNtupleDColumn(fHeaderID, fHeaderCRYNormCol, fMessenger.GetCRYNorm());
  }

  //Secondary capture generator
  if (fMessenger.GetSourceType()=="SCTGenerator") {
    analysisManager->FillNtupleIColumn(fHeaderID, fHeaderSCTCapturedParticlesCol, static_cast<G4int>(fMessenger.GetSCTCapturedParticles()));
  }

  //MUTE generator
  if (fMessenger.GetSourceType()=="muteGenerator") {
    analysisManager->FillNtupleDColumn(fHeaderID, fHeaderMuteFluxNormalizationCol, fMessenger.GetMuteFluxNormalization());
  }

  analysisManager->AddNtupleRow(fHeaderID);

  ////////////////////////
  // FILL GEOMETRY TREE //
  ////////////////////////
  for (auto* vol : fMessenger.GetVolumes()) {
    analysisManager->FillNtupleSColumn(fGeometryID, fGeometryNameCol, vol->name);
    analysisManager->FillNtupleSColumn(fGeometryID, fGeometryShapeCol, vol->shape);
    analysisManager->FillNtupleSColumn(fGeometryID, fGeometryParentCol, vol->parentName);
    analysisManager->FillNtupleSColumn(fGeometryID, fGeometryMaterialCol, vol->materialName);
    analysisManager->FillNtupleIColumn(fGeometryID, fGeometryNumberCol, vol->volumeNumber);
    analysisManager->FillNtupleDColumn(fGeometryID, fGeometryAbsXCol, vol->absolutePosition.x());
    analysisManager->FillNtupleDColumn(fGeometryID, fGeometryAbsYCol, vol->absolutePosition.y());
    analysisManager->FillNtupleDColumn(fGeometryID, fGeometryAbsZCol, vol->absolutePosition.z());

    int nPoints = 5000;
    fGeomXs.clear();
    fGeomYs.clear();
    fGeomZs.clear();

    for (int pointNum=0; pointNum<nPoints; pointNum++) {
      G4ThreeVector randPos = vol->GenerateRandomPointInside();
      fGeomXs.push_back(randPos.x());
      fGeomYs.push_back(randPos.y());
      fGeomZs.push_back(randPos.z());
    }

    analysisManager->AddNtupleRow(fGeometryID);
  }
}


// Fill primaries tree
void PaleoSimOutputManager::FillPrimariesTreeEvent() {
  if (fPrimariesID < 0) return;
  if (fPrimaryPdgID.empty()) return;

  auto* analysisManager = G4RootAnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(fPrimariesID, fPrimaryEventIDCol, fPrimaryEventID);
  analysisManager->FillNtupleDColumn(fPrimariesID, fPrimaryMuonThetaCol, fPrimaryMuonTheta);
  analysisManager->FillNtupleDColumn(fPrimariesID, fPrimaryMuonPhiCol, fPrimaryMuonPhi);
  analysisManager->FillNtupleDColumn(fPrimariesID, fPrimaryMuonSlantCol, fPrimaryMuonSlant);

  analysisManager->AddNtupleRow(fPrimariesID);
}


void PaleoSimOutputManager::FillMINTreeEvent() {
  if (fMINID < 0) return;
  if (fMINEventMultiplicity == 0) return;

  auto* analysisManager = G4RootAnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(fMINID, fMINEventIDCol, fMINEventID);
  analysisManager->FillNtupleIColumn(fMINID, fMINMultiplicityCol, fMINEventMultiplicity);

  analysisManager->AddNtupleRow(fMINID);
}


// Fill neutron tally tree
void PaleoSimOutputManager::FillNeutronTallyTreeEvent() {
  if (fNeutronTallyID < 0) return;
  if (fNeutronEntryMultiplicity == 0) return;

  auto* analysisManager = G4RootAnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(fNeutronTallyID, fNeutronTallyEventIDCol, fNeutronTallyEventID);
  analysisManager->FillNtupleIColumn(fNeutronTallyID, fNeutronTallyMultiplicityCol, fNeutronEntryMultiplicity);

  analysisManager->AddNtupleRow(fNeutronTallyID);
}


// Fill secondary capture tree
void PaleoSimOutputManager::FillSecondaryCaptureTreeEvent() {
  if (fSecondaryCaptureID < 0) return;

  auto* analysisManager = G4RootAnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(fSecondaryCaptureID, fSecondaryCaptureEventIDCol, fSecondaryCaptureEventID);

  analysisManager->AddNtupleRow(fSecondaryCaptureID);
}


// Fill recoil tree
void PaleoSimOutputManager::FillRecoilTreeEvent() {
  if (fRecoilID < 0) return;
  if (fNRecoils == 0) return;

  auto* analysisManager = G4RootAnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(fRecoilID, fRecoilEventIDCol, fRecoilEventID);
  analysisManager->FillNtupleIColumn(fRecoilID, fRecoilNRecoilsCol, fNRecoils);

  analysisManager->AddNtupleRow(fRecoilID);
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
  fPrevNeutronTallyVolumeNumbers.clear();
}


void PaleoSimOutputManager::ClearSecondaryCaptureTreeEvent() {
  fSecondaryCaptureEventID = -1;
  fSecondary_entryPDG.clear();
  fSecondary_entryEnergy.clear();
  fSecondary_entryX.clear();
  fSecondary_entryY.clear();
  fSecondary_entryZ.clear();
  fSecondary_entryU.clear();
  fSecondary_entryV.clear();
  fSecondary_entryW.clear();
  fSecondary_creationZ.clear();
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