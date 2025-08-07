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
#include "G4RandomDirection.hh"
#include "G4LogicalVolumeStore.hh"
#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimVolumeDefinition.hh"

#include "TF1.h"
#include <algorithm>
#include <vector>

PaleoSimPrimaryGeneratorAction::PaleoSimPrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                                                                 PaleoSimOutputManager& manager)
: G4VUserPrimaryGeneratorAction(),  
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
  else if (sourceType == "volumetricSource") {
    InitializeVolumetricSourceGenerator();
  }
  else if (sourceType == "diskSource") {
    InitializeDiskSourceGenerator();
  }
  else {
    G4Exception("PaleoSimPrimaryGeneratorAction::PaleoSimPrimaryGeneratorAction", "001", FatalException,
                ("Source not valid: " + sourceType).c_str());
  }
}

PaleoSimPrimaryGeneratorAction::~PaleoSimPrimaryGeneratorAction() {
    if (cryFileLoaded) {
      cryFile->Close();
      delete cryFile;
    }
    if (fVolumetricSourceSpectrumFileLoaded) {
      fVolumetricSourceSpectrumFile->Close();
      delete fVolumetricSourceSpectrumFile;
    }
    if (diskSourceSpectrumFileLoaded) {
      diskSourceSpectrumFile->Close();
      delete diskSourceSpectrumFile;
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
  //Volumetric source
  else if (sourceType == "volumetricSource") {
    GenerateVolumetricSourcePrimaries(anEvent);
  }
  //Disk source
  else if (sourceType == "diskSource") {
    GenerateDiskSourcePrimaries(anEvent);
  }
  else {
    G4Exception("PaleoSimPrimaryGeneratorAction::GeneratePrimaries", "GeneratePrimaries001", FatalException,
                ("Invalid source: " + sourceType).c_str());
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

  G4double theta = fMuonThetaDist->GetRandom();
  G4double phi = (2.0 * M_PI - 0.001) * G4UniformRand();

  G4double h_km = h0_km / std::cos(theta);

  G4double E_GeV = fMuonEnergyDist->GetRandom();
  G4double Ekin = E_GeV * GeV;
  G4double mass = muonDef->GetPDGMass();
  G4double Etot = Ekin + mass;
  G4double p = std::sqrt(Etot*Etot - mass*mass);

  G4ThreeVector position = SamplePointOnTopOfWorldVolume();

  G4ThreeVector direction(std::sin(theta) * std::cos(phi),
                          std::sin(theta) * std::sin(phi),
                          -std::cos(theta)); // downward
  G4ThreeVector momentum = direction * p;

  G4PrimaryParticle* primary = new G4PrimaryParticle(muonDef, momentum.x(), momentum.y(), momentum.z());
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, /*time=*/0.0);
  vertex->SetPrimary(primary);
  anEvent->AddPrimaryVertex(vertex);

  auto* info = new PaleoSimUserEventInformation();
  info->muonTheta = theta;
  info->muonPhi = phi;
  info->muonSlantDepth = h_km*km;
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

  //Get random energy, theta from fMuteHist
  double E_GeV = -1, theta = -1;
  fMuteHist->GetRandom2(E_GeV, theta);  

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
  
    //Check and open root file
    std::ifstream testFile(filename);
    if (!testFile.good()) {
      G4Exception("InitializeCRY", "CRY001", FatalException,
                  ("Cannot open CRY file: " + filename).c_str());
    }
    testFile.close();
    cryFile = TFile::Open(filename);
    if (!cryFile || cryFile->IsZombie()) {
      G4Exception("InitializeCRY", "CRY002", FatalException,
                  ("Failed to open CRY ROOT file: " + filename).c_str());
    }
    
    //Read in tree
    cryTree = dynamic_cast<TTree*>(cryFile->Get("cryTree"));
    if (!cryTree) {
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

///////////////////////////////
//Volumetric source generator//
///////////////////////////////
void PaleoSimPrimaryGeneratorAction::InitializeVolumetricSourceGenerator() {
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
  }
}

void PaleoSimPrimaryGeneratorAction::GenerateVolumetricSourcePrimaries(G4Event* anEvent) {
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

///////////////
//Disk source//
///////////////

///////////////////////////
// Disk source generator //
///////////////////////////
void PaleoSimPrimaryGeneratorAction::InitializeDiskSourceGenerator() {
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
  }
}

void PaleoSimPrimaryGeneratorAction::GenerateDiskSourcePrimaries(G4Event* anEvent) {

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


//////////////////
//Other function//
//////////////////
//Gets random point on top of world volume. Requires world to be a cylinder or box for now
G4ThreeVector PaleoSimPrimaryGeneratorAction::SamplePointOnTopOfWorldVolume() {
  //Find the world volume
  const auto& volumes = fMessenger.GetVolumes();
  const PaleoSimVolumeDefinition* worldDef = nullptr;
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
  const PaleoSimVolumeDefinition* worldDef = nullptr;
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

//Sample point on disk via MC.
G4ThreeVector PaleoSimPrimaryGeneratorAction::SamplePointOnDisk(double radius, const G4ThreeVector& center, const G4ThreeVector& axis) {
    G4ThreeVector n = axis.unit();
    //If almost completely oriented along z-axis, use z-axis
    G4ThreeVector temp = (std::fabs(n.z()) < 0.99999) ? G4ThreeVector(0,0,1) : G4ThreeVector(1,0,0);
    G4ThreeVector u = n.cross(temp).unit();
    G4ThreeVector v = n.cross(u).unit();
    
    double r = radius * std::sqrt(G4UniformRand());
    double theta = 2 * CLHEP::pi * G4UniformRand();
    double x_local = r * std::cos(theta);
    double y_local = r * std::sin(theta);

    // Transform to global coordinates
    return center + x_local * u + y_local * v;
}