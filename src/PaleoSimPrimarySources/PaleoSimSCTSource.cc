#include "PaleoSimPrimarySources/PaleoSimSCTSource.hh"

#include <iostream>
#include <fstream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"

void PaleoSimSCTPool::LoadPool(const std::string& filename,
                               PaleoSimSCTSource* SCTSourceObject) {
    std::lock_guard<std::mutex> lock(fInitMutex);
    if (fIsLoaded) {
        return;
    }

    std::ifstream testFile(filename);
    if (!testFile.good()) {
        G4Exception("PaleoSimSCTPool::LoadPool", "SCTPool001", FatalException,
                    ("Cannot open SCT file: " + filename).c_str());
    }
    testFile.close();

    TFile* file = TFile::Open(filename.c_str());
    if (!file || file->IsZombie()) {
        G4Exception("PaleoSimSCTPool::LoadPool", "SCTPool002", FatalException,
                    ("Failed to open SCT ROOT file: " + filename).c_str());
    }

    TTree* tree = dynamic_cast<TTree*>(file->Get("secondaryCaptureTree"));
    if (!tree) {
        G4Exception("PaleoSimSCTPool::LoadPool", "SCTPool003", FatalException,
                    "Tree 'secondaryCaptureTree' not found in ROOT file.");
    }

    std::vector<int>*   sct_pdgcode = nullptr;
    std::vector<double>* sct_energy  = nullptr;
    std::vector<double>* sct_u       = nullptr;
    std::vector<double>* sct_v       = nullptr;
    std::vector<double>* sct_w       = nullptr;
    std::vector<double>* sct_x       = nullptr;
    std::vector<double>* sct_y       = nullptr;

    tree->SetBranchAddress("pdgCode", &sct_pdgcode);
    tree->SetBranchAddress("energy",  &sct_energy);
    tree->SetBranchAddress("entry_u", &sct_u);
    tree->SetBranchAddress("entry_v", &sct_v);
    tree->SetBranchAddress("entry_w", &sct_w);
    tree->SetBranchAddress("entry_x", &sct_x);
    tree->SetBranchAddress("entry_y", &sct_y);

    Long64_t totalEntries = tree->GetEntries();
    std::cout << "[Shared SCT Pool] Reading " << totalEntries << " raw capture entries..." << std::endl;

    G4ThreeVector randomPosition = SCTSourceObject->GetSampledWorldTop();
    G4double overburdenZ = randomPosition.z();

    fPool.reserve(totalEntries);

    for (Long64_t i = 0; i < totalEntries; i++) {
        tree->GetEntry(i);

        if (i % 1000000 == 0 && i > 0) {
            std::cout << "[Shared SCT Pool] Processed " << i << " / " << totalEntries << " entries..." << std::endl;
        }

        if (!sct_pdgcode || sct_pdgcode->empty()) {
            continue;
        }

        G4ThreeVector entryPosition(sct_x->at(0), sct_y->at(0), overburdenZ);
        if (!SCTSourceObject->CheckTopSurface(entryPosition)) {
            continue;
        }

        if (sct_w->at(0) > 0) {
            continue; // Particle cluster is going away from the crystal
        }

        SCTEvent evt;
        size_t nParticles = sct_pdgcode->size();
        evt.particles.reserve(nParticles);

        for (size_t i = 0; i < nParticles; i++) {
            evt.particles.push_back({
                sct_pdgcode->at(i),
                sct_energy->at(i),
                sct_u->at(i),
                sct_v->at(i),
                sct_w->at(i),
                sct_x->at(i),
                sct_y->at(i)
            });
        }
        fPool.push_back(evt);
    }

    TTree* headerTree = dynamic_cast<TTree*>(file->Get("headerTree"));
    if (headerTree) {
        double norm = 0.0;
        headerTree->SetBranchAddress("meiHimeFluxNormalization_per_cm2_per_s", &norm);
        headerTree->GetEntry(0);
        fNorm = norm;
    }

    file->Close();
    delete file;

    fIsLoaded = true;
    std::cout << "[Shared SCT Pool]: " << fPool.size()
              << " events ready in shared memory." << std::endl;
}

PaleoSimSCTSource::PaleoSimSCTSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimSCTSource::~PaleoSimSCTSource() {
}

void PaleoSimSCTSource::InitializeSource() {
    G4String filename = fMessenger.GetSCTFilename();
    G4String userBootstrap = fMessenger.GetSCTBootstrap();

    if (userBootstrap == "standard" || userBootstrap == "rrs") {
        SCTBootstrap = userBootstrap;
    } else {
        SCTBootstrap = "standard";
    }

    PaleoSimSCTPool::Instance().LoadPool(filename, this);

    nSCTEntries = PaleoSimSCTPool::Instance().GetPoolSize();
    fMessenger.SetSCTNorm(PaleoSimSCTPool::Instance().GetNorm());
    fMessenger.SetSCTCapturedParticles(static_cast<int>(nSCTEntries));
}

void PaleoSimSCTSource::GeneratePrimaries(G4Event* anEvent) {
    if (nSCTEntries == 0) {
        G4Exception("GenerateSCTPrimaries", "SCTGenerator004", FatalException,
                    "No valid entries found in shared SCT pool.");
    }

    Long64_t entry = G4RandFlat::shootInt(static_cast<Long64_t>(nSCTEntries));
    const SCTEvent& evt = PaleoSimSCTPool::Instance().GetEvent(entry);

    const auto& anchor = evt.particles[0];
    G4double anchorParticleX = anchor.x * mm;
    G4double anchorParticleY = anchor.y * mm;

    if (SCTBootstrap == "standard") {
        G4ThreeVector randomPosition = SamplePointOnTopOfWorldVolume();
        G4double globalX = randomPosition.x() * mm;
        G4double globalY = randomPosition.y() * mm;
        G4double globalZ = randomPosition.z() * mm;

        for (const auto& particle : evt.particles) {
            G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(particle.pdgCode);
            if (!particleDef) {
                G4cerr << "Unknown PDG code in SCT: " << particle.pdgCode << G4endl;
                continue;
            }

            double Ekin = particle.energy * MeV;
            double mass = particleDef->GetPDGMass();
            double Etot = Ekin + mass;
            double p = std::sqrt(Etot * Etot - mass * mass);

            G4double localOffsetX = (particle.x * mm) - anchorParticleX;
            G4double localOffsetY = (particle.y * mm) - anchorParticleY;

            G4ThreeVector position(globalX + localOffsetX, globalY + localOffsetY, globalZ);
            if (IsWithinTopSurface(position)) {
                G4ThreeVector direction(particle.u, particle.v, particle.w);
                G4ThreeVector momentum = direction * p;

                G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef,
                                                                  momentum.x(),
                                                                  momentum.y(),
                                                                  momentum.z());

                G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
                vertex->SetPrimary(primary);
                anEvent->AddPrimaryVertex(vertex);
            }
        }
    }
    else if (SCTBootstrap == "rrs") {
        G4ThreeVector randomPosition = SamplePointOnTopOfWorldVolume();
        G4double globalZ = randomPosition.z() * mm;

        G4double globalX = anchor.x * mm;
        G4double globalY = anchor.y * mm;

        int randomRefRot = G4RandFlat::shootInt(8);
        RandomReflectionRotation(globalX, globalY, randomRefRot);
        LocalSquareSmear(globalX, globalY, nSCTEntries);

        for (const auto& particle : evt.particles) {
            G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(particle.pdgCode);
            if (!particleDef) {
                G4cerr << "Unknown PDG code in SCT: " << particle.pdgCode << G4endl;
                continue;
            }

            double Ekin = particle.energy * MeV;
            double mass = particleDef->GetPDGMass();
            double Etot = Ekin + mass;
            double p = std::sqrt(Etot * Etot - mass * mass);

            G4double localOffsetX = (particle.x * mm) - anchorParticleX;
            G4double localOffsetY = (particle.y * mm) - anchorParticleY;

            G4double particleU = particle.u;
            G4double particleV = particle.v;
            RandomReflectionRotation(particleU, particleV, randomRefRot);

            G4ThreeVector position(globalX + localOffsetX, globalY + localOffsetY, globalZ);
            if (IsWithinTopSurface(position)) {
                G4ThreeVector direction(particleU, particleV, particle.w);
                G4ThreeVector momentum = direction * p;

                G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef,
                                                                  momentum.x(),
                                                                  momentum.y(),
                                                                  momentum.z());

                G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
                vertex->SetPrimary(primary);
                anEvent->AddPrimaryVertex(vertex);
            }
        }
    }
}