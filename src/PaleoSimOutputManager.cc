#include "PaleoSimOutputManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "G4SystemOfUnits.hh"
#include <stdexcept>

//Constructor
PaleoSimOutputManager::PaleoSimOutputManager(PaleoSimMessenger& messenger)
    : fMessenger(messenger) {}

//Write and close
void PaleoSimOutputManager::WriteAndClose() {
  if (fFile) {
      fFile->cd();
      if (fPrimariesTreeStatus && fPrimariesTree) {
          fPrimariesTree->Write("", TFile::kOverwrite); 
      }
      if (fNeutronTallyTreeStatus && fNeutronTallyTree) {
          fNeutronTallyTree->Write("", TFile::kOverwrite); 
      }
      fFile->Close();
      delete fFile;
      fFile = nullptr;
  }
  fPrimariesTree = nullptr;
  fNeutronTallyTree = nullptr;
}

//Determine status of primaries tree
void PaleoSimOutputManager::DeterminePrimariesTreeOutputStatus() {
  fPrimariesTreeStatus = fMessenger.GetUserPrimaryTreeOutputStatus();
}

//Determine status of neutron tally tree
void PaleoSimOutputManager::DetermineNeutronTallyTreeOutputStatus() {
  G4double airCavitySideLength = fMessenger.GetAirCavitySideLength();
  G4bool neutronTallyUserStatus = fMessenger.GetUserNeutronTallyTreeOutputStatus();
  if ((airCavitySideLength > 0.0) && (neutronTallyUserStatus)) {
    fNeutronTallyTreeStatus = true;
  }
  else {
    fNeutronTallyTreeStatus = false;
  }
}

//Create files and trees
void PaleoSimOutputManager::CreateOutputFileAndTrees() {
  //Determine tree status
  DeterminePrimariesTreeOutputStatus();
  DetermineNeutronTallyTreeOutputStatus();

  //Create files
  G4String outputPath = fMessenger.GetOutputPath();
  if (outputPath.empty()) {
      //TODO--create folders automatically if needed
      fFile = new TFile("outputFiles/paleosim_output.root", "RECREATE");  // Default filename if no path set
  } else {
      fFile = new TFile(outputPath.c_str(), "RECREATE");
  }

  if (!fFile || fFile->IsZombie()) {
      throw std::runtime_error("Failed to create output file.");
  }

  if (fPrimariesTreeStatus) {
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
    // Mei & Hime muon generator
    fPrimariesTree->Branch("muonTheta", &fPrimaryMuonTheta);
    fPrimariesTree->Branch("muonPhi", &fPrimaryMuonPhi);
    fPrimariesTree->Branch("muonSlant", &fPrimaryMuonSlant);
  }

  if (fNeutronTallyTreeStatus) {
    fNeutronTallyTree = new TTree("neutronTallyTree", "Muon-induced neutrons entering cavity");

    fNeutronTallyTree->Branch("eventID", &fNeutronTallyEventID);
    fNeutronTallyTree->Branch("entry_energy", &fNeutron_entryEnergy);
    fNeutronTallyTree->Branch("entry_x", &fNeutron_entryX);
    fNeutronTallyTree->Branch("entry_y", &fNeutron_entryY);
    fNeutronTallyTree->Branch("entry_z", &fNeutron_entryZ);
  }
}

//Fill primaries tree
void PaleoSimOutputManager::FillPrimariesTreeEvent() {
    if (!fPrimariesTreeStatus || !fPrimariesTree) return;
    fPrimariesTree->Fill();
}

//Fill neutron tally tree
void PaleoSimOutputManager::FillNeutronTallyTreeEvent() {
    if (!fNeutronTallyTreeStatus || !fNeutronTallyTree) return;
    fNeutronTallyTree->Fill();
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
  //
  //Mei & Hime muon generator
  fPrimaryMuonTheta.clear();
  fPrimaryMuonPhi.clear();
  fPrimaryMuonSlant.clear();
}

void PaleoSimOutputManager::ClearNeutronTallyTreeEvent() {
  fNeutronTallyEventID = -1;
  fNeutron_entryEnergy.clear();
  fNeutron_entryX.clear();
  fNeutron_entryY.clear();
  fNeutron_entryZ.clear();
}