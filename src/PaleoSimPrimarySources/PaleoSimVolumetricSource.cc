#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"

#include "PaleoSimPrimarySources/PaleoSimVolumetricSource.hh"

PaleoSimVolumetricSource::PaleoSimVolumetricSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimVolumetricSource::~PaleoSimVolumetricSource() {
  if (fVolumetricSourceSpectrumFileLoaded) {
    fVolumetricSourceSpectrumFile->Close();
    delete fVolumetricSourceSpectrumFile;
  }
}

void PaleoSimVolumetricSource::InitializeSource() {

  //Try to get histogram if requested
  if (fMessenger.GetVolumetricSourceType() == "hist") {
    //Load file
    G4String filename = fMessenger.GetVolumetricSourceSpectrumFilename();
  
    //Check and open root file
    std::ifstream testFile(filename);
    if (!testFile.good()) {
      G4Exception("InitializeVolumetricSourceGenerator", "Volumetric001", FatalException,
                  ("Cannot open Volumetric source spectrum file: " + filename).c_str());
    }
    testFile.close();
    fVolumetricSourceSpectrumFile = TFile::Open(filename.c_str());
    if (!fVolumetricSourceSpectrumFile || fVolumetricSourceSpectrumFile->IsZombie()) {
      G4Exception("InitializeVolumetricSourceGenerator", "Volumetric002", FatalException,
                  ("Failed to open Volumetric source file: " + filename).c_str());
    }
  
    //Read in hist
    G4String histName = fMessenger.GetVolumetricSourceSpectrumHistName();
    fVolumetricSourceSpectrumHist = dynamic_cast<TH1D*>(fVolumetricSourceSpectrumFile->Get(histName));
    if (!fVolumetricSourceSpectrumHist) {
      G4Exception("InitializeVolumetricSourceGenerator", "Volumetric003", FatalException,
                  ("Hist '" + histName + "' was not found in ROOT file.").c_str());
    }
    fVolumetricSourceSpectrumFileLoaded=true;
  }
}


void PaleoSimVolumetricSource::GeneratePrimaries(G4Event* anEvent) {

  // Set particle type
  int pdgCode = fMessenger.GetVolumetricSourcePDGCode();
  G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
  if (!particleDef) {
    G4String msg = "Unknown PDG code in Volumetric Source Generator: " + std::to_string(pdgCode);
    G4Exception("PaleoSimPrimaryGeneratorAction::GenerateVolumetricSourcePrimaries",
                "VolumetricSource001", FatalException, msg.c_str());
  }

  // Initialize volume definition (once)
  if (!fSourceVolumeDefinition) {
    G4String volumeName = fMessenger.GetVolumetricSourceVolumeName();
    fSourceVolumeDefinition = fMessenger.GetVolumeByName(volumeName);

    if (!fSourceVolumeDefinition) {
      G4Exception("PaleoSimPrimaryGeneratorAction::GenerateVolumetricSourcePrimaries",
                  "Volumetric004", FatalException,
                  ("PaleoSimVolumeDefinition '" + volumeName + "' not found!").c_str());
    }
  }

  // Sample a position using the geometry-aware method
  G4ThreeVector pos = fSourceVolumeDefinition->GenerateRandomPointInside();
  
  // Set energy
  double energy = 0;
  if (fMessenger.GetVolumetricSourceType() == "hist") {
    energy = fVolumetricSourceSpectrumHist->GetRandom() * MeV;
  } else {
    energy = fMessenger.GetVolumetricSourceMonoEnergy();
  }

  // Compute momentum
  double mass = particleDef->GetPDGMass();
  double Etot = energy + mass;
  double p = std::sqrt(Etot * Etot - mass * mass);

  G4ThreeVector direction = G4RandomDirection();
  G4ThreeVector momentum = direction * p;

  // Create primary particle and vertex
  G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef, momentum.x(), momentum.y(), momentum.z());
  G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, 0.0);
  vertex->SetPrimary(primary);
  anEvent->AddPrimaryVertex(vertex);
}
