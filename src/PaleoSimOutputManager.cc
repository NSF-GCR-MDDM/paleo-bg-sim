#include "PaleoSimOutputManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "G4SystemOfUnits.hh"
#include <stdexcept>

PaleoSimOutputManager& PaleoSimOutputManager::Get() {
    static PaleoSimOutputManager instance;
    return instance;
}

void PaleoSimOutputManager::EnablePrimariesTree(bool enable) {
    fEnablePrimariesTree = enable;
}

void PaleoSimOutputManager::SetOutputPath(const std::string& path) {
    fOutputPath = path;
}

void PaleoSimOutputManager::Initialize(bool hasTallyVolume, bool hasRecoilVolume) {
    if (fOutputPath.empty()) {
        fFile = new TFile("outputFiles/paleosim_output.root", "RECREATE");  // Default filename if no path set
    } else {
        fFile = new TFile(fOutputPath.c_str(), "RECREATE");
    }

    if (!fFile || fFile->IsZombie()) {
        throw std::runtime_error("Failed to create output file.");
    }

    if (fEnablePrimariesTree) {
        fPrimariesTree = new TTree("primariesTree", "Generated primary particles");

        fPrimariesTree->Branch("eventID", &fEventID);
        fPrimariesTree->Branch("pdgID", &fPrimaryPdgID);
        fPrimariesTree->Branch("energy", &fPrimaryEnergy);
        fPrimariesTree->Branch("x", &fPrimaryX);
        fPrimariesTree->Branch("y", &fPrimaryY);
        fPrimariesTree->Branch("z", &fPrimaryZ);
        fPrimariesTree->Branch("px", &fPrimaryPx);
        fPrimariesTree->Branch("py", &fPrimaryPy);
        fPrimariesTree->Branch("pz", &fPrimaryPz);
        fPrimariesTree->Branch("muonTheta", &fPrimaryTheta);
        fPrimariesTree->Branch("muonPhi", &fPrimaryPhi);
        fPrimariesTree->Branch("muonSlant", &fPrimarySlant);
    }
}

void PaleoSimOutputManager::FillPrimaryEvent(int eventID,
                                             const std::vector<int>& pdgIDs,
                                             const std::vector<double>& primaryEnergies,
                                             const std::vector<double>& primary_x,
                                             const std::vector<double>& primary_y,
                                             const std::vector<double>& primary_z,
                                             const std::vector<double>& primary_px,
                                             const std::vector<double>& primary_py,
                                             const std::vector<double>& primary_pz,
                                             const std::vector<double>& muonTheta,
                                             const std::vector<double>& muonPhi,
                                             const std::vector<double>& muonSlant) {
    if (!fEnablePrimariesTree || !fPrimariesTree) return;

    fEventID = eventID;
    fPrimaryPdgID = pdgIDs;
    fPrimaryEnergy = primaryEnergies;
    fPrimaryX = primary_x;
    fPrimaryY = primary_y;
    fPrimaryZ = primary_z;
    fPrimaryPx = primary_px;
    fPrimaryPy = primary_py;
    fPrimaryPz = primary_pz;
    fPrimaryTheta = muonTheta;
    fPrimaryPhi = muonPhi;
    fPrimarySlant = muonSlant;

    fPrimariesTree->Fill();
}

void PaleoSimOutputManager::WriteAndClose() {
    if (fFile) {
        fFile->cd();
        if (fEnablePrimariesTree && fPrimariesTree) fPrimariesTree->Write("", TFile::kOverwrite); // Use kOverwrite to avoid duplicate trees
        fFile->Close();
        delete fFile;
        fFile = nullptr;
    }
    fPrimariesTree = nullptr;
}
