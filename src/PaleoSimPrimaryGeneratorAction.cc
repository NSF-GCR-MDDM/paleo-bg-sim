#include "PaleoSimPrimaryGeneratorAction.hh"
#include "PaleoSimOutputManager.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Exception.hh"
#include "PaleoSimUserEventInformation.hh"

#include "TF1.h"
#include <algorithm>
#include <vector>

PaleoSimPrimaryGeneratorAction::PaleoSimPrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                                                                 PaleoSimOutputManager& manager)
: G4VUserPrimaryGeneratorAction(), 
  fGPS(new G4GeneralParticleSource()), 
  fMessenger(messenger),
  fManager(manager)
{

  //Check whether we were passed a valid source type
  G4String sourceType = fMessenger.GetSourceType();
  std::vector<G4String> validSourceTypes = fMessenger.GetValidSourceTypes();
  if (std::find(validSourceTypes.begin(), validSourceTypes.end(), sourceType) == validSourceTypes.end()) {
      G4Exception("PaleoSimPrimaryGeneratorAction", "InvalidSource", FatalException,
                  ("Invalid source type: " + sourceType).c_str());
  }

  // CUSTOM_GENERATOR_HOOK
  // Call source initialization logic here if needed for new generators
	//
	//Mei & Hime muon generator
  if (sourceType == "meiHimeMuonGenerator") {
    InitializeMeiHimeMuons();
  }
  else if (sourceType == "muteGenerator") {
    InitializeMuteMuons();
  }
  else if (sourceType == "CRYGenerator") {
    InitializeCRYGenerator();
  }

}

PaleoSimPrimaryGeneratorAction::~PaleoSimPrimaryGeneratorAction() {
    delete fGPS;
    if (cryFileLoaded) {
      cryFile->Close();
      delete cryFile;
    }
}

void PaleoSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  // CUSTOM_GENERATOR_HOOK
  // Add dispatch logic here for new generators
	//
	//Mei & Hime muon generator
  G4String sourceType = fMessenger.GetSourceType();
  if (sourceType == "meiHimeMuonGenerator") {
    GenerateMeiHimeMuonPrimaries(anEvent);
  }
  //MUTE
  else if (sourceType == "muteGenerator") {
    GenerateMutePrimaries(anEvent);
  }
  //CRY
  else if (sourceType == "CRYGenerator") {
    GenerateCRYPrimaries(anEvent);
  }
}

// CUSTOM_GENERATOR_HOOK
// Your initialization methods go here
//
///////////////////////////////
// Mei & Hime Muon Generator //
///////////////////////////////
// Mei & Hime Muon Generator
void PaleoSimPrimaryGeneratorAction::InitializeMeiHimeMuons() {
  G4double h0 = fMessenger.GetMeiHimeMuonEffectiveDepth();
  G4double h0_km = h0 / km;

  // Flux normalization (Mei & Hime Eq. 4)
  G4double I0 = 67.97e-6 * std::exp(-h0_km / 0.285) + 2.071e-6 * std::exp(-h0_km / 0.698);
  fMessenger.SetMeiHimeFluxNormalization(I0);

  fMuonThetaDist = new TF1("fMuonThetaDist",
                        "( [0]*exp(-[2]/([3]*cos(x))) + [1]*exp(-[2]/([4]*cos(x))) ) / cos(x)",
                        0, CLHEP::pi / 2. - 0.001);
                          
  fMuonThetaDist->SetParameter(0, 0.0000086); //I1
  fMuonThetaDist->SetParameter(1, 0.00000044); //I2
  fMuonThetaDist->SetParameter(2, h0_km);
  fMuonThetaDist->SetParameter(3, 0.45); //1/lambda1
  fMuonThetaDist->SetParameter(4, 0.87); //1/lambda2
  fMuonThetaDist->SetNpx(1000);

  fMuonEnergyDist = new TF1("fMuonEnergyDist",
      "exp(-[0]*[1]*([2]-1)) * (x + [3]*(1-exp(-[0]*[1])))^(-[2])", 0.5, 1e8);
  fMuonEnergyDist->SetParameter(0, 0.4);
  //Par 1 set after sampling theta and calculating slant depth
  fMuonEnergyDist->SetParameter(2, 3.77);
  fMuonEnergyDist->SetParameter(3, 693);
  fMuonEnergyDist->SetNpx(1000);
}

void PaleoSimPrimaryGeneratorAction::GenerateMeiHimeMuonPrimaries(G4Event* anEvent) {
  G4double h0 = fMessenger.GetMeiHimeMuonEffectiveDepth();
  G4double h0_km = h0 / km;

  G4ParticleDefinition* muonDef = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  fGPS->SetParticleDefinition(muonDef);

  G4double theta = fMuonThetaDist->GetRandom();
  G4double phi = (2.0 * M_PI - 0.001) * G4UniformRand();

  G4double h_km = h0_km / std::cos(theta);
  fMuonEnergyDist->SetParameter(1, h_km);

  G4double E_GeV = fMuonEnergyDist->GetRandom();
  fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E_GeV*GeV);

  G4ThreeVector position = SamplePointOnTopOfWorldVolume();
  G4ThreeVector direction(std::sin(theta) * std::cos(phi),
                          std::sin(theta) * std::sin(phi),
                          -std::cos(theta)); // downward
  fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);
  fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

  fGPS->GeneratePrimaryVertex(anEvent);

  //We store custom variables in a UserEventInfo to pass them to beginningOfEventAction to load into trees
  auto* info = new PaleoSimUserEventInformation();
  info->muonTheta=theta;
  info->muonPhi=phi;
  info->muonSlantDepth=h_km*km;
  anEvent->SetUserInformation(info); 
}

//////////////////
//Mute generator//
//////////////////
void PaleoSimPrimaryGeneratorAction::InitializeMuteMuons() {
  //Check if fMessenger.GetMuteHistFilename() exists. If not error
  G4String filename = fMessenger.GetMuteHistFilename();
  
  std::ifstream testFile(filename);
  if (!testFile.good()) {
    G4Exception("InitializeMuteMuons", "Mute001", FatalException,
                ("Cannot open MUTE histogram file: " + filename).c_str());
  }
  testFile.close();

  //Open root file. Check if "muonHist exists". If not error
  TFile* file = TFile::Open(filename);
  if (!file || file->IsZombie()) {
    G4Exception("InitializeMuteMuons", "Mute002", FatalException,
                ("Failed to open ROOT file: " + filename).c_str());
  }
  
  //Store in fMuteHist
  fMuteHist = dynamic_cast<TH2D*>(file->Get("muonHist"));
  if (!fMuteHist) {
    G4Exception("InitializeMuteMuons", "Mute004", FatalException,
                "Histogram 'muonHist' not found or wrong type in file.");
  }
  fMuteHist->SetDirectory(0); //Store in memory
  file->Close();
  delete file;
}

void PaleoSimPrimaryGeneratorAction::GenerateMutePrimaries(G4Event* anEvent) {
  
  G4ParticleDefinition* muonDef = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  fGPS->SetParticleDefinition(muonDef);

  //Get random energy, theta from fMuteHist
  double E_GeV = -1, theta = -1;
  fMuteHist->GetRandom2(E_GeV, theta);  
  fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E_GeV*GeV);

  //Get random phi
  G4double phi = (2.0 * M_PI - 0.001) * G4UniformRand();

  //Calculate position, direction, set those
  G4ThreeVector position = SamplePointOnTopOfWorldVolume();
  G4ThreeVector direction(std::sin(theta) * std::cos(phi),
                          std::sin(theta) * std::sin(phi),
                          -std::cos(theta)); // downward
  fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);
  fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

  //Generate vertex
  fGPS->GeneratePrimaryVertex(anEvent);

  //We store custom variables in a UserEventInfo to pass them to beginningOfEventAction to load into trees
  auto* info = new PaleoSimUserEventInformation();
  info->muonTheta=theta;
  info->muonPhi=phi;
  anEvent->SetUserInformation(info); //G4 takes ownership, no need to delete
}

///////////////////
// CRY generator //
///////////////////
void PaleoSimPrimaryGeneratorAction::InitializeCRYGenerator() {
  //Load file
    G4String filename = fMessenger.GetCRYFilename();
  
    std::ifstream testFile(filename);
    if (!testFile.good()) {
      G4Exception("InitializeCRY", "CRY001", FatalException,
                  ("Cannot open CRY file: " + filename).c_str());
    }
    testFile.close();
  
    //Open root file. Check if "muonHist exists". If not error
    cryFile = TFile::Open(filename);
    if (!cryFile || cryFile->IsZombie()) {
      G4Exception("InitializeCRY", "CRY002", FatalException,
                  ("Failed to open CRY ROOT file: " + filename).c_str());
    }
    
    //Read in tree
    cryTree = dynamic_cast<TTree*>(cryFile->Get("cryTree"));
    if (!cryFile) {
      G4Exception("InitializeCRY", "CRY003", FatalException,
                  "Tree 'cryTree' not found in ROOT file.");
    }

    cryTree->SetBranchAddress("pdgCode",    &cry_pdgcode);
    cryTree->SetBranchAddress("energy_MeV", &cry_energy);
    cryTree->SetBranchAddress("u",          &cry_u);
    cryTree->SetBranchAddress("v",          &cry_v);
    cryTree->SetBranchAddress("w",          &cry_w);
    cryTree->SetBranchAddress("x_mm",       &cry_x);
    cryTree->SetBranchAddress("y_mm",       &cry_y);
    nCryEntries = cryTree->GetEntries();
    std::cout<<"Number of CRY particles to sample is "<<nCryEntries<<std::endl;
    cryFileLoaded = true;

    //Get header info
    TTree * headerTree = dynamic_cast<TTree*>(cryFile->Get("headerTree"));
    float altitude,latitude,norm;
    headerTree->SetBranchAddress("altitude", &altitude);
    headerTree->SetBranchAddress("latitude",          &latitude);
    headerTree->SetBranchAddress("primaries_per_cm2_per_s",          &norm);

    headerTree->GetEvent(0);
    fMessenger.SetCRYAltitude(static_cast<double>(altitude));
    fMessenger.SetCRYLatitude(static_cast<double>(latitude));
    fMessenger.SetCRYNorm(static_cast<double>(norm));
    delete headerTree;
}

void PaleoSimPrimaryGeneratorAction::GenerateCRYPrimaries(G4Event* anEvent) {
  //Get random event
  Long64_t entry = G4RandFlat::shootInt(nCryEntries);
  cryTree->GetEntry(entry);

  // Sample a position on the top of the world volume
  G4ThreeVector basePosition = SamplePointOnTopOfWorldVolume();

  //Load all particles into vertices
  for (size_t i = 0; i < cry_pdgcode->size(); i++) {
    int pdgCode = cry_pdgcode->at(i);
    G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
    if (!particleDef) {
        G4cerr << "Unknown PDG code in CRY: " << pdgCode << G4endl;
        continue;
    }

    double Ekin = cry_energy->at(i) * MeV;
    double mass = particleDef->GetPDGMass();
    double Etot = Ekin + mass;
    double p = std::sqrt(Etot * Etot - mass * mass);

    G4ThreeVector position = basePosition + G4ThreeVector(cry_x->at(i) * cm, cry_y->at(i) * cm, 0);
    if (IsWithinTopSurface(position)) {
      G4ThreeVector direction(cry_u->at(i), cry_v->at(i), cry_w->at(i));
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

  //Store primary info in userEventInfo
  auto* info = new PaleoSimUserEventInformation();
  // Compute total energy
  double totalEnergy = 0.0;
  G4ThreeVector weightedDirection(0,0,0);
  for (size_t i = 0; i < cry_pdgcode->size(); ++i) {
      double energy = cry_energy->at(i);  // in MeV
      totalEnergy += energy;
      weightedDirection += G4ThreeVector(cry_u->at(i), cry_v->at(i), cry_w->at(i)) * energy;
  }
  G4ThreeVector dir = weightedDirection.unit();
  info->CRYCoreTheta = dir.theta();  // radians
  info->CRYCorePhi   = dir.phi();    // radians
  info->CRYTotalEnergy = totalEnergy;  // MeV
  info->CRYCorePosition = basePosition;
  anEvent->SetUserInformation(info); //G4 takes ownership, no need to delete
}
  
//////////////////
//Other function//
//////////////////
//Gets random point on top of world volume. Requires world to be a cylinder or box for now
G4ThreeVector PaleoSimPrimaryGeneratorAction::SamplePointOnTopOfWorldVolume() {
  //Find the world volume
  const auto& volumes = fMessenger.GetVolumes();
  const VolumeDefinition* worldDef = nullptr;
  for (const auto* vol : volumes) {
      if (vol->parentName == "None") {
          worldDef = vol;
          break;
      }
  }

  //Generate a random point on top surface if it's a box or cylinder, error out otherwise
  G4ThreeVector localTop;
  if (worldDef->shape == "box") {
      G4double x = (G4UniformRand() - 0.5) * 2 * worldDef->halfLengths.x();
      G4double y = (G4UniformRand() - 0.5) * 2 * worldDef->halfLengths.y();
      G4double z = +worldDef->halfLengths.z();
      localTop = G4ThreeVector(x, y, z);
  }
  else if (worldDef->shape == "cylinder") {
      G4double r = worldDef->radius * std::sqrt(G4UniformRand());
      G4double theta = G4UniformRand() * 2 * CLHEP::pi;
      G4double x = r * std::cos(theta);
      G4double y = r * std::sin(theta);
      G4double z = worldDef->halfHeight;
      localTop = G4ThreeVector(x, y, z);
  }
  else {
      G4Exception("SamplePointOnTopOfWorldVolume", "UnsupportedShape", FatalException,
                  ("Top surface sampling not supported for shape: " + worldDef->shape).c_str());
  }

  return worldDef->absolutePosition + localTop;
}

//Checks if a point is on the top surface or not
G4bool PaleoSimPrimaryGeneratorAction::IsWithinTopSurface(const G4ThreeVector& point) {
  const auto& volumes = fMessenger.GetVolumes();
  const VolumeDefinition* worldDef = nullptr;
  for (const auto* vol : volumes) {
      if (vol->parentName == "None") {
          worldDef = vol;
          break;
      }
  }

  if (worldDef->shape == "box") {
      if (std::abs(point.x()) <= worldDef->halfLengths.x() && std::abs(point.y()) <= worldDef->halfLengths.y()) {
        return true;
      }
  }
  else if (worldDef->shape == "cylinder") {
      double r2 = point.x()*point.x() + point.y()*point.y();
      if (r2 <= worldDef->radius * worldDef->radius) {
        return true;
      }
  }
  else {
      G4Exception("IsWithinTopSurface", "UnsupportedShape", FatalException,
                  ("Top surface sampling not supported for shape: " + worldDef->shape).c_str());
  }

  return false;
}