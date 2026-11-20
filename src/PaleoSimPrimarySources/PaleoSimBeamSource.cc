#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"


#include "PaleoSimPrimarySources/PaleoSimBeamSource.hh"

PaleoSimBeamSource::PaleoSimBeamSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimBeamSource::~PaleoSimBeamSource() {
  if (spectrumFileLoaded) {
    spectrumFile->Close();
    delete spectrumFile;
  }
}


//Copies values from the messenger to the object and validates them, so problems in the
//macro are caught before the run starts. Every setting is read here and nowhere else.
void PaleoSimBeamSource::InitializeSource() {

  //Settings used in every mode
  //Check particle is valid and understood?
  beamSourcePDGCode = fMessenger.GetBeamSourcePDGCode();
  G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(beamSourcePDGCode);
  if (!particleDef) {
    G4String msg = "Unknown PDG code in Beam Generator: " + std::to_string(beamSourcePDGCode);
    G4Exception("InitializeBeamSourceGenerator",
                "BEAM001", FatalException, msg.c_str());
  }

  directionType = fMessenger.GetBeamSourceDirectionType();
  spectrumType = fMessenger.GetBeamSourceType();
  sourceRadius_mm = fMessenger.GetBeamSourceRadius();
  sourcePosition = fMessenger.GetBeamSourcePosition();
  sourceAxis = fMessenger.GetBeamSourceAxis();
  sourceAxis = sourceAxis.unit();

  if (directionType == "hist2D") {
    needs2DHist = true;
  }
  else if (spectrumType == "hist") {
    needs1DHist = true;
  }

  //If we're drawing beam information from a histogram
  if (needs1DHist || needs2DHist) {
    G4String filename = fMessenger.GetBeamSourceSpectrumFilename();
    G4String histName = fMessenger.GetBeamSourceSpectrumHistName();

    //Check if passed in spectrum file exists and is valid
    std::ifstream testFile(filename);
    if (!testFile.good()) {
      G4Exception("InitializeBeamSourceGenerator", "BEAM002", FatalException,
                  ("Cannot open beam source spectrum file: " + filename).c_str());
    }
    testFile.close();
    spectrumFile = TFile::Open(filename.c_str());
    if (!spectrumFile || spectrumFile->IsZombie()) {
      G4Exception("InitializeBeamSourceGenerator", "BEAM003", FatalException,
                  ("Failed to open beam source spectrum file: " + filename).c_str());
    }
    spectrumFileLoaded = true;

    //Check the hist exists and is valid.
    TObject* obj = spectrumFile->Get(histName.c_str());
    if (!obj) {
      G4Exception("InitializeBeamSourceGenerator", "BEAM004", FatalException,
                  ("Hist '" + histName + "' was not found in " + filename).c_str());
    }

    if (needs2DHist) {
      energyCosThetaHist = dynamic_cast<TH2*>(obj);
      if (!energyCosThetaHist) {
        G4Exception("InitializeBeamSourceGenerator", "BEAM005", FatalException,
                    ("hist2D mode needs a 2D histogram, but '" + histName + "' is not one.").c_str());
      }
    }
    else {
      if (obj->InheritsFrom(TH2::Class())) {
        G4Exception("InitializeBeamSourceGenerator", "BEAM006", FatalException,
                    ("Spectrum type 'hist' needs a 1D histogram, but '" + histName + "' is 2D.").c_str());
      }
      energyHist = dynamic_cast<TH1*>(obj);
      if (!energyHist) {
        G4Exception("InitializeBeamSourceGenerator", "BEAM007", FatalException,
                    ("'" + histName + "' is not a histogram.").c_str());
      }
    }
  }
  //Otherwise mono energy beam
  else {
    monoEnergy_MeV = fMessenger.GetBeamSourceMonoEnergy();
  }

  //Summary, also useful for checking the messenger reads the macro correctly
  std::cout << "Beam source: PDG " << beamSourcePDGCode
         << ", direction mode " << directionType
         << ", spectrum " << (needs2DHist ? G4String("hist2D") : spectrumType)
         << ", radius " << sourceRadius_mm / mm << " mm"
         << ", position " << sourcePosition / mm << " mm"
         << ", axis " << sourceAxis << G4endl;
}



void PaleoSimBeamSource::GeneratePrimaries(G4Event* anEvent) {
  ReseedRootRNG();

  //Set particle type
  G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(beamSourcePDGCode);

  //Sample random position on disk, set position
  G4ThreeVector pos = SamplePointOnDisk(sourceRadius_mm,sourcePosition,sourceAxis);

  //Sample direction 
  G4ThreeVector direction;
  if (directionType=="fixed") {
    direction = sourceAxis;
  }
  else if (directionType=="isotropic") {
    direction = G4RandomDirection();
  }
  else if (directionType=="cosine") {
    double cosTheta = std::sqrt(G4UniformRand());
    cosTheta = std::max(-1., std::min(1., cosTheta));
    double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
    double phi = 2.*CLHEP::pi*G4UniformRand();
    G4ThreeVector dir(sinTheta*std::cos(phi), sinTheta*std::sin(phi), cosTheta);
    dir.rotateUz(sourceAxis);
    direction = dir;
  }
  

  //Sample energy
  double energy=0;
  if (spectrumType == "mono") {
    energy = monoEnergy_MeV;
  }
  else {
    if (needs1DHist) {
      energy = energyHist->GetRandom(&fRootRNG)*MeV;
    }
    else {
      double energyMeV;
      double cosTheta;
      energyCosThetaHist->GetRandom2(cosTheta, energyMeV, &fRootRNG);
      energy = energyMeV*MeV;
      
      cosTheta = std::max(-1., std::min(1., cosTheta));
      double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
      double phi = 2.*CLHEP::pi*G4UniformRand();
      G4ThreeVector dir(sinTheta*std::cos(phi), sinTheta*std::sin(phi), cosTheta);
      dir.rotateUz(sourceAxis);
      direction = dir;
    }
  }

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
