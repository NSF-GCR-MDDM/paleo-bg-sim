#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"

#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimPrimarySources/PaleoSimMuteSource.hh"

PaleoSimMuteSource::PaleoSimMuteSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimMuteSource::~PaleoSimMuteSource() {
    if (muteFileLoaded) {
        muteFile->Close();
        delete muteFile;
    }
    if (muteHist) {
        delete muteHist;
    }
}

void PaleoSimMuteSource::InitializeSource() {
  //Load file
  G4String filename = fMessenger.GetMuteHistFilename();
  G4String overburdenType = fMessenger.GetMuteOverburdenType();

  //Check and open root file
  std::ifstream testFile(filename);
  if (!testFile.good()) {
    G4Exception("InitializeMuteGenerator", "MuteGenerator001", FatalException,
                ("Cannot open mute muonHist file: " + filename).c_str());
  }
  testFile.close();
  muteFile = TFile::Open(filename);
  if (!muteFile || muteFile->IsZombie()) {
    G4Exception("InitializeMuteGenerator", "MuteGenerator002", FatalException,
                ("Failed to open mute ROOT file: " + filename).c_str());
  }
  muteFileLoaded = true;
  
  //Read in TH3D
  muteHist = dynamic_cast<TH3D*>(muteFile->Get("muonHist"));
  if (!muteHist) {
    G4Exception("InitializeMuteGenerator", "MuteGenerator003", FatalException,
                "Histogram 'muonHist' not found or wrong type in file.");
  }
  muteHist->SetDirectory(0); //Store in memory
  muteFile->Close();
  delete muteFile;

  //Read mountain profile
  if (overburdenType == "mountain") {
    G4String mountainFilename = fMessenger.GetMuteMountainProfileFilename();
    std::ifstream mountainFile(mountainFilename);
    if (!mountainFile.good()) {
      G4Exception("InitializeMuteGenerator", "MuteGenerator004", FatalException,
                  ("Failed to open mute mountain profile file: " + mountainFilename).c_str());
    }
    mountainProfileLoaded = true;

    std::string mountainProfileLine;
    while (std::getline(mountainFile, mountainProfileLine)) {
      std::stringstream mountainProfileLinestream(mountainProfileLine);

      if (mountainProfileLinestream >> mountainZenith >> mountainAzimuthal >> mountainSlantDepth) {
        allMountainZeniths.push_back(mountainZenith);
        allMountainAzimuthals.push_back(mountainAzimuthal);
        allMountainSlantDepths.push_back(mountainSlantDepth);
      }
      else {
        std::cout << "ALERT: mountain profile has invalid line.";
      }
    }
    mountainFile.close();
  }
}

void PaleoSimMuteSource::GeneratePrimaries(G4Event* anEvent) {
  G4double fluxDepth = fMessenger.GetMuteFluxDepth();
  G4String overburdenType = fMessenger.GetMuteOverburdenType();
  G4ParticleDefinition* muonDef = G4ParticleTable::GetParticleTable()->FindParticle("mu-");

  if (overburdenType == "flat") {
    //Get random energy, theta from fMuteHist
    double E_GeV = -1, surfaceTheta = -1, slant_kmwe = -1;
    muteHist->GetRandom3(slant_kmwe, E_GeV, surfaceTheta);
    double theta = std::acos(fluxDepth/slant_kmwe);  

    //Get random phi
    G4double phi = (2.0 * M_PI - 0.001) * G4UniformRand();

    //Kinematics
    double Ekin = E_GeV * GeV;
    double mass = muonDef->GetPDGMass();
    double Etot = Ekin + mass;
    double p = std::sqrt(Etot * Etot - mass * mass);

    //Calculate position, direction, set those
    G4ThreeVector position = SamplePointOnTopOfWorldVolume();
    G4ThreeVector direction(std::sin(theta) * std::cos(phi),
                            std::sin(theta) * std::sin(phi),
                            -std::cos(theta)); // downward
    G4ThreeVector momentum = direction * p;

    //Create primary particle and vertex
    G4PrimaryParticle* primary = new G4PrimaryParticle(muonDef,
                                                        momentum.x(),
                                                        momentum.y(),
                                                        momentum.z());
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
    vertex->SetPrimary(primary);
    anEvent->AddPrimaryVertex(vertex);

    //Pass custom variables to event info
    auto* info = new PaleoSimUserEventInformation();
    info->muonTheta=theta;
    info->muonPhi=phi;
    anEvent->SetUserInformation(info); //G4 takes ownership, no need to delete
  }
}
