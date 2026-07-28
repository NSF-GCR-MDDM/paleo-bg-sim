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
        mountainProfile.AddMountainPoint(mountainZenith, mountainAzimuthal, mountainSlantDepth);
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

  double E_GeV = -1, surfaceTheta = -1, slant_kmwe = -1, theta = -1, phi = -1;
  if (overburdenType == "flat") {
    //Get random energy, theta from fMuteHist
    muteHist->GetRandom3(slant_kmwe, E_GeV, surfaceTheta);
    theta = std::acos(fluxDepth/slant_kmwe);  

    //Get random phi
    phi = (2.0 * M_PI - 0.001) * G4UniformRand();
  }

  if (overburdenType == "mountain") {
    //Get random energy, theta, slant depth from fMuteHist
    muteHist->GetRandom3(slant_kmwe, E_GeV, theta);

    //Find/sample corresponding phi
    phi = mountainProfile.FindOrSampleAzimuthal(theta*180/M_PI, slant_kmwe);

    //Convert to radians
    phi *= M_PI / 180;
  }

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
  info->muonSlantDepth=slant_kmwe;
  anEvent->SetUserInformation(info); //G4 takes ownership, no need to delete
}

void MountainProfile::AddMountainPoint(double theta, double phi, double X) {
  if ((theta < 0 || theta > 90) || (phi < 0 || phi > 360)) {
    G4Exception("SetMountainSlantDepth", "MuteGenerator006", FatalException, 
                "Invalid theta and phi read from MUTE mountain profile!");
    return;
  }
  profileMap[theta].push_back({phi, X});

  return;
}

std::vector<double> MountainProfile::FindPhiOnRing(const std::vector<TerrainPoint>& mountainRing, double targetX) const {
  std::vector<double> matchingPhis;
  if (mountainRing.empty()) {
    return matchingPhis;
  }

  for (size_t i = 0; i < mountainRing.size(); i++) {
    size_t j = (i + 1) % mountainRing.size();

    double phi1 = mountainRing[i].phi;
    double phi2 = mountainRing[j].phi;
    double X1 = mountainRing[i].slantDepth;
    double X2 = mountainRing[j].slantDepth;

    // Full rotation safeguard
    if ((j == 0) && (phi2 < phi1)) {
      phi2 += 360;
    }

    // Check if targetX is in range and interpolate
    if (((targetX >= X1) && (targetX <= X2)) || ((targetX <= X1) && (targetX >= X2))) {

      // To prevent division by zero
      if (std::abs(X2 - X1) < 1e-6) {
        matchingPhis.push_back(phi1);
        continue;
      }

      double phiDiff = phi2 - phi1;
      double fractionalXDiff = (targetX - X1) / (X2 - X1);
      double finalPhi = phi1 + (fractionalXDiff * phiDiff);

      // Check for edge cases
      if (finalPhi < 0) {
        finalPhi += 360;
      }
      if (finalPhi > 360) {
        finalPhi -= 360;
      }

      matchingPhis.push_back(finalPhi);
    }
  }

  return matchingPhis;
}

std::vector<TerrainPoint> MountainProfile::CreateInterpolatedRing(const std::vector<TerrainPoint>& mountainRing1,
                                                                  const std::vector<TerrainPoint>& mountainRing2,
                                                                  double fraction) const {
  std::vector<TerrainPoint> interpolatedRing;
  interpolatedRing.reserve(mountainRing1.size());

  if (mountainRing1.size() != mountainRing2.size()) {
    G4Exception("CreateInterpolatedRing", "MuteGenerator007", FatalException,
                "Mountain profile azimuthals aren't uniform across zenith angles!");
  }
  
  for (size_t i = 0; i < mountainRing1.size(); i++) {
    double phi = mountainRing1[i].phi;
    double X1 = mountainRing1[i].slantDepth;
    double X2 = mountainRing2[i].slantDepth;

    double interpolatedX = X1 + fraction*(X2 - X1);
    interpolatedRing.push_back({phi, interpolatedX});
  }

  return interpolatedRing;
}

double MountainProfile::FindOrSampleAzimuthal(double targetTheta, double targetX) const {
  if (targetTheta > 90 || targetTheta < 0) {
    G4Exception("findOrSampleAzimuthal", "MuteGenerator005", FatalException,
                "Geant4 sampled particle with invalid Zenith angle from muonHist");
  }

  // Empty/missing mountain profile safeguard
  if (profileMap.empty()) {
    return G4RandFlat::shoot(0.000, 359.999);
  }

  // Get nearest thetas and check for edge cases
  auto it_above = profileMap.lower_bound(targetTheta);

  if (it_above == profileMap.begin()) {
    std::vector<double> phis = FindPhiOnRing(it_above->second, targetX);
    if (phis.empty()) {
      return G4RandFlat::shoot(0.000, 359.999);
    }
    return phis[G4RandFlat::shootInt(phis.size())];
  }

  if (it_above == profileMap.end()) {
    std::vector<double> phis = FindPhiOnRing(std::prev(it_above)->second, targetX);
    if (phis.empty()) {
      return G4RandFlat::shoot(0.000, 359.999);
    }
    return phis[G4RandFlat::shootInt(phis.size())];
  }

  // Interpolate 
  auto it_below = std::prev(it_above);
  double theta_below = it_below->first;
  double theta_above = it_above->first;
  double theta_fraction = (targetTheta - theta_below) / (theta_above - theta_below);

  std::vector<TerrainPoint> interpolatedRing = CreateInterpolatedRing(it_below->second, it_above->second, theta_fraction);
  std::vector<double> azimuthalPool = FindPhiOnRing(interpolatedRing, targetX);

  if (azimuthalPool.empty()) {
    return G4RandFlat::shoot(0.000, 359.999);
  }

  return azimuthalPool[G4RandFlat::shootInt(azimuthalPool.size())];
}