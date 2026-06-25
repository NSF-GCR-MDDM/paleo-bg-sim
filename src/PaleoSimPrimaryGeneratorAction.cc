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
#include <chrono>

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


  if (sourceType == "meiHimeMuonGenerator") {
    fMeiHimeSource = new PaleoSimMeiHimeSource(fMessenger);
    fMeiHimeSource->InitializeSource();
  }
  //else if (sourceType == "muteGenerator") {
  //  InitializeMuteMuons();
  //}
  else if (sourceType == "CRYGenerator") {
    fCrySource = new PaleoSimCrySource(fMessenger);
    fCrySource->InitializeSource();
  }
  else if (sourceType == "volumetricSource") {
    fVolumetricSource = new PaleoSimVolumetricSource(fMessenger);
    fVolumetricSource->InitializeSource();
  }
  else if (sourceType == "diskSource") {
    fDiskSource = new PaleoSimDiskSource(fMessenger);
    fDiskSource->InitializeSource();
  }
  else {
    G4Exception("PaleoSimPrimaryGeneratorAction::PaleoSimPrimaryGeneratorAction", "001", FatalException,
                ("Source not valid: " + sourceType).c_str());
  }
}

PaleoSimPrimaryGeneratorAction::~PaleoSimPrimaryGeneratorAction() {
    delete fMeiHimeSource;
    delete fVolumetricSource;
    delete fDiskSource;
    delete fCrySource;
}

void PaleoSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  // CUSTOM_GENERATOR_HOOK
  // Add dispatch logic here for new generators
	//
	//Mei & Hime muon generator
  G4String sourceType = fMessenger.GetSourceType();
  if (sourceType == "meiHimeMuonGenerator") {
    fMeiHimeSource->GeneratePrimaries(anEvent);
  }
  //MUTE
  //else if (sourceType == "muteGenerator") {
  //  GenerateMutePrimaries(anEvent);
  //}
  //CRY
  else if (sourceType == "CRYGenerator") {
    fCrySource->GeneratePrimaries(anEvent);
  }
  //Volumetric source
  else if (sourceType == "volumetricSource") {
    fVolumetricSource->GeneratePrimaries(anEvent);
  }
  //Disk source
  else if (sourceType == "diskSource") {  
    fDiskSource->GeneratePrimaries(anEvent);
  }
  else {
    G4Exception("PaleoSimPrimaryGeneratorAction::GeneratePrimaries", "GeneratePrimaries001", FatalException,
                ("Invalid source: " + sourceType).c_str());
  }
}

//////////////////
//Mute generator//
//////////////////
/*
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

*/
