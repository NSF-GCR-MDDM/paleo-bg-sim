#include "PaleoSimRootOutputWriter.hh"

#include "PaleoSimOutputManager.hh"
#include "G4Exception.hh"
#include "TFile.h"

void PaleoSimRootOutputWriter::Write(PaleoSimOutputManager& manager) {
  if (!manager.fFile) {
    G4Exception("PaleoSimOutputManager", "NoRootFile", FatalException,
                "ROOT output requested but no ROOT file is open.");
  }

  manager.fFile->cd();
  if (manager.fHeaderTree) manager.fHeaderTree->Write("headerTree", TFile::kOverwrite);
  if (manager.fGeometryTree) manager.fGeometryTree->Write("geometryTree", TFile::kOverwrite);
  if (manager.fMessenger.GetPrimariesTreeStatus() && manager.fPrimariesTree) manager.fPrimariesTree->Write("primariesTree", TFile::kOverwrite);
  if (manager.fMessenger.GetMINTreeStatus() && manager.fMINTree) manager.fMINTree->Write("MINTree", TFile::kOverwrite);
  if (manager.fMessenger.GetNeutronTallyTreeStatus() && manager.fNeutronTallyTree) manager.fNeutronTallyTree->Write("neutronTallyTree", TFile::kOverwrite);
  if (manager.fMessenger.GetRecoilTreeStatus() && manager.fRecoilTree) manager.fRecoilTree->Write("recoilTree", TFile::kOverwrite);

  manager.fFile->Close();
  delete manager.fFile;
  manager.fFile = nullptr;

  manager.fHeaderTree = nullptr;
  manager.fGeometryTree = nullptr;
  manager.fPrimariesTree = nullptr;
  manager.fMINTree = nullptr;
  manager.fNeutronTallyTree = nullptr;
  manager.fRecoilTree = nullptr;
}
