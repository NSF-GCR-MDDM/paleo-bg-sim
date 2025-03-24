#include "PaleoSimOutputManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "G4SystemOfUnits.hh"
#include <stdexcept>

PaleoSimOutputManager& PaleoSimOutputManager::Get() {
    static PaleoSimOutputManager instance;
    return instance;
}

void PaleoSimOutputManager::SetOutputPath(const std::string& path) {
    fOutputPath = path;
}

void PaleoSimOutputManager::EnablePrimariesTree(bool enable) {
    fEnablePrimariesTree = enable;
}

void PaleoSimOutputManager::EnableNeutronTallyTree(bool enable) {
    fEnableNeutronTallyTree = enable;
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

    if (fEnableNeutronTallyTree) {
        fNeutronTallyTree = new TTree("neutronTallyTree", "Muon-induced neutrons entering cavity");

        fNeutronTallyTree->Branch("eventID", &fNeutronTallyEventID);
        //fNeutronTallyTree->Branch("gen_energy", &fNeutron_genEnergy);
        fNeutronTallyTree->Branch("entry_energy", &fNeutron_entryEnergy);
        fNeutronTallyTree->Branch("entry_x", &fNeutron_entryX);
        fNeutronTallyTree->Branch("entry_y", &fNeutron_entryY);
        fNeutronTallyTree->Branch("entry_z", &fNeutron_entryZ);
        //fNeutronTallyTree->Branch("entry_px", &fNeutron_entryPx);
        //fNeutronTallyTree->Branch("entry_py", &fNeutron_entryPy);
        //fNeutronTallyTree->Branch("entry_pz", &fNeutron_entryPz);
        //fNeutronTallyTree->Branch("theta", &fNeutron_thetas);
        //fNeutronTallyTree->Branch("distToMuon", &fNeutron_distToMuon);
        //fNeutronTallyTree->Branch("type", &fNeutron_type);
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


void PaleoSimOutputManager::FillNeutronTallyEvent(int eventID,
                                                  //const std::vector<double>& neutron_genEnergy,
                                                  const std::vector<double>& neutron_entryEnergy,
                                                  const std::vector<double>& neutron_entryX,
                                                  const std::vector<double>& neutron_entryY,
                                                  const std::vector<double>& neutron_entryZ
                                                  //const std::vector<double>& neutron_entryPx,
                                                  //const std::vector<double>& neutron_entryPy,
                                                  //const std::vector<double>& neutron_entryPz,
                                                  //const std::vector<double>& neutron_thetas,
                                                  //const std::vector<double>& neutron_distToMuon,
                                                  //const std::vector<int>& neutron_type
                                                  ) {
    if (!fEnableNeutronTallyTree || !fNeutronTallyTree) return;


    fNeutronTallyEventID = eventID;
    //fNeutron_genEnergy = neutron_genEnergy;
    fNeutron_entryEnergy = neutron_entryEnergy;
    fNeutron_entryX = neutron_entryX;
    fNeutron_entryY = neutron_entryY;
    fNeutron_entryZ = neutron_entryZ;
    //fNeutron_entryPx = neutron_entryPx;
    //fNeutron_entryPy = neutron_entryPy;
    //fNeutron_entryPz = neutron_entryPz;
    //fNeutron_thetas = neutron_thetas;
    //fNeutron_distToMuon = neutron_distToMuon;
    //fNeutron_type = neutron_type;

    fNeutronTallyTree->Fill();
}

void PaleoSimOutputManager::WriteAndClose() {
    if (fFile) {
        fFile->cd();
        if (fEnablePrimariesTree && fPrimariesTree) {
            fPrimariesTree->Write("", TFile::kOverwrite); 
        }
        if (fEnableNeutronTallyTree && fNeutronTallyTree) {
            fNeutronTallyTree->Write("", TFile::kOverwrite); 
        }
        fFile->Close();
        delete fFile;
        fFile = nullptr;
    }
    fPrimariesTree = nullptr;
    fNeutronTallyTree = nullptr;
}
