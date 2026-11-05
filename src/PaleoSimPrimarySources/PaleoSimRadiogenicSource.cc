#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"

#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimPrimarySources/PaleoSimRadiogenicSource.hh"

PaleoSimRadiogenicSource::PaleoSimRadiogenicSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimRadiogenicSource::~PaleoSimRadiogenicSource() {
    if (decayChainFileLoaded) {
        decayChainFile->Close();
        delete decayChainFile;
    }
}

void PaleoSimRadiogenicSource::InitializeSource() {
    // Load file 
    G4String filename = fMessenger.GetRadiogenicSourceFilename();

    // Check if it exists; if yes, open
    std::ifstream testFile(filename);
    if (!testFile.good()) {
        G4Exception("PaleoSimRadiogenicSource::InitializeSource", "RadiogenicSource001", FatalException,
                    ("Failed to open radiogenic source file: " + filename).c_str());
    }
    decayChainFile = TFile::Open(filename);
    if (!decayChainFile || decayChainFile->IsZombie()) {
        G4Exception("PaleoSimRadiogenicSource::InitializeSource", "RadiogenicSource002", FatalException, 
                    ("Failed to open radiogenic source ROOT file: " + filename).c_str());
    }
    decayChainFileLoaded = true;

    // Read in tree
    decayTree = dynamic_cast<TTree*>(decayChainFile->Get("decayTree"));
    if (!decayTree) {
        G4Exception("PaleoSimRadiogenicSource::InitializeSource", "RadiogenicSource003", FatalException, 
                    ("Failed to retrieve decayTree from file: " + filename).c_str());
    }

    // Read in scalars into buffers
    decayTree->SetBranchAddress("event_id", &b_event_id);
    decayTree->SetBranchAddress("pdg",      &b_pdg);
    decayTree->SetBranchAddress("KE_MeV",   &b_KE_MeV);
    decayTree->SetBranchAddress("u",        &b_u);
    decayTree->SetBranchAddress("v",        &b_v);
    decayTree->SetBranchAddress("w",        &b_w);
    decayTree->SetBranchAddress("time_sec", &b_time_sec);

    nDecayChainEntries = decayTree->GetEntries();
    std::cout<<"Number of radiogenic source particles to sample from is "<<nDecayChainEntries<<std::endl;

    crystalAge = fMessenger.GetRadiogenicSourceAgeYrs() * 31536000; // years to seconds
}

void PaleoSimRadiogenicSource::GeneratePrimaries(G4Event* anEvent) {
    if (currentTreeEntry >= nDecayChainEntries) {
        currentTreeEntry = 0; 
    }

    // Determine the target cascade event_id
    decayTree->GetEntry(currentTreeEntry);
    Int_t activeEventID = b_event_id;

    std::vector<DecayParticle> cascade;

    // Read in all consecutive particle rows belonging to this cascade
    while (currentTreeEntry < nDecayChainEntries) {
        decayTree->GetEntry(currentTreeEntry);
        if (b_event_id != activeEventID) {
            break; // Finished current decay cascade
        }

        cascade.push_back({b_pdg, b_KE_MeV, b_u, b_v, b_w, b_time_sec});
        currentTreeEntry++;
    }

    // Count heavy daughter recoils that occurred within the crystal recording age
    G4int recoilsInCluster = 0;
    for (const auto& particle : cascade) {
        // Ignoring alphas for now; count only heavy recoils
        if (particle.time_sec <= crystalAge && particle.pdg > 1000020040) {
            recoilsInCluster++;
        }
    }

    auto* eventInfo = new PaleoSimUserEventInformation();
    eventInfo->recoilsInCluster = recoilsInCluster;
    anEvent->SetUserInformation(eventInfo);

    // Initialize volume definition once
    if (!fSourceVolumeDefinition) {
        G4String volumeName = fMessenger.GetRadiogenicSourceVolumeName();
        fSourceVolumeDefinition = fMessenger.GetVolumeByName(volumeName);
        if (!fSourceVolumeDefinition) {
            G4Exception("PaleoSimRadiogenicSource::GeneratePrimaries", "RadiogenicSource004", FatalException,
                        ("PaleoSimVolumeDefinition '" + volumeName + "' not found!").c_str());
        }
    }

    G4ThreeVector position =  fSourceVolumeDefinition->GenerateRandomPointInside();
    for (const auto& particle : cascade) {
        if (particle.time_sec > crystalAge) {
            continue;
        }

        int pdgCode = particle.pdg;
        G4ParticleDefinition* particleDef = nullptr;
        if (pdgCode <= 1000020040) {
            // Alphas and lighter particles
            particleDef = G4ParticleTable::GetParticleTable()->FindParticle("alpha");
        } 
        else if (pdgCode > 1000020040) {
            // Heavy ions, decode PDG 
            G4int Z = (pdgCode / 10000) % 1000;
            G4int A = (pdgCode / 10) % 1000;
            particleDef = G4IonTable::GetIonTable()->GetIon(Z, A, 0.0);
        }
        if (!particleDef) {
            G4cerr << "Unknown PDG code in decayTree: " << pdgCode << G4endl;
            continue;
        }
        double Ekin = particle.KE_MeV * MeV;
        double mass = particleDef->GetPDGMass();
        double Etot = Ekin + mass;
        double p = std::sqrt(Etot * Etot - mass * mass);
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