#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "PaleoSimPrimarySources/PaleoSimDiskSource.hh"

PaleoSimDiskSource::PaleoSimDiskSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimDiskSource::~PaleoSimDiskSource() {
  if (diskSourceSpectrumFileLoaded) {
    diskSourceSpectrumFile->Close();
    delete diskSourceSpectrumFile;
  }
}

void PaleoSimDiskSource::InitializeSource() {
    //Try to get histogram if requested
  if (fMessenger.GetDiskSourceType() == "hist") {
    //Load file
    G4String filename = fMessenger.GetDiskSourceSpectrumFilename();
  
    //Check and open root file
    std::ifstream testFile(filename);
    if (!testFile.good()) {
      G4Exception("InitializeDiskSourceGenerator", "DISK001", FatalException,
                  ("Cannot open disk source spectrum file: " + filename).c_str());
    }
    testFile.close();
    diskSourceSpectrumFile = TFile::Open(filename);
    if (!diskSourceSpectrumFile || diskSourceSpectrumFile->IsZombie()) {
      G4Exception("InitializeDiskSourceGenerator", "DISK002", FatalException,
                  ("Failed to open Disk source file: " + filename).c_str());
    }
  
    //Read in hist
    G4String histName = fMessenger.GetDiskSourceSpectrumHistName();
    diskSourceSpectrumHist = dynamic_cast<TH1D*>(diskSourceSpectrumFile->Get(histName));
    if (!diskSourceSpectrumHist) {
      G4Exception("InitializeDiskSourceGenerator", "DISK003", FatalException,
                  ("Hist '" + histName + "' was not found in ROOT file.").c_str());
    }
    diskSourceSpectrumFileLoaded = true;
  }
}

void PaleoSimDiskSource::GeneratePrimaries(G4Event* anEvent) {
  //Set particle type
  int pdgCode = fMessenger.GetDiskSourcePDGCode();
  G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
  if (!particleDef) {
    G4String msg = "Unknown PDG code in Disk Source Generator: " + std::to_string(pdgCode);
    G4Exception("PaleoSimPrimaryGeneratorAction::GenerateDiskSourcePrimaries",
                "DiskSource001", FatalException, msg.c_str());
  }

  //Sample random position on disk, set position
  G4ThreeVector pos = SamplePointOnDisk(fMessenger.GetDiskSourceRadius(),fMessenger.GetDiskSourcePosition(),fMessenger.GetDiskSourceAxis());

  //Set energy
  double energy=0;
  if (fMessenger.GetDiskSourceType()=="hist") {
    energy = diskSourceSpectrumHist->GetRandom()*MeV;
  }
  else {
    energy = fMessenger.GetDiskSourceMonoEnergy();
  }

  //Set direction
  G4ThreeVector direction = fMessenger.GetDiskSourceDirection();
  direction = direction.unit();

  //Compute momentum
  double mass = particleDef->GetPDGMass();
  double totalEnergy = energy + mass;
  double momentumMag = std::sqrt(totalEnergy*totalEnergy - mass*mass);
  G4ThreeVector momentum = momentumMag * direction;

  //Generate vertex
  G4PrimaryParticle* particle = new G4PrimaryParticle(particleDef, momentum.x(), momentum.y(), momentum.z());
  particle->SetMass(mass);
  particle->SetKineticEnergy(energy);

  G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, 0.0); // time = 0
  vertex->SetPrimary(particle);

  anEvent->AddPrimaryVertex(vertex);
}
