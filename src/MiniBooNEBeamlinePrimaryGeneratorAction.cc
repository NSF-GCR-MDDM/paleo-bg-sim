#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "PaleoSimOutputManager.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Exception.hh"

#include "TF1.h"
#include <algorithm>

MiniBooNEBeamlinePrimaryGeneratorAction::MiniBooNEBeamlinePrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                                                                 PaleoSimOutputManager& manager)
: G4VUserPrimaryGeneratorAction(), 
  fGPS(new G4GeneralParticleSource()), 
  fMessenger(messenger),
  fManager(manager)
{
  // List of valid sources
  fValidSourceTypes = {
  // CUSTOM_GENERATOR_HOOK
  // Add new generator names to the list above
  //
  // Mei & Hime muon generator
      "muonGenerator"
  };

  //Check whether we were passed a valid source type
  G4String sourceType = fMessenger.GetSourceType();
  if (std::find(fValidSourceTypes.begin(), fValidSourceTypes.end(), sourceType) == fValidSourceTypes.end()) {
      G4Exception("MiniBooNEBeamlinePrimaryGeneratorAction", "InvalidSource", FatalException,
                  ("Invalid source type: " + sourceType).c_str());
  }

  // CUSTOM_GENERATOR_HOOK
  // Call source initialization logic here if needed for new generators
	//
	//Mei & Hime muon generator
  if (sourceType == "muonGenerator") {
    InitializeMuons();
}

}

MiniBooNEBeamlinePrimaryGeneratorAction::~MiniBooNEBeamlinePrimaryGeneratorAction() {
    delete fGPS;
    if (fMuonThetaDist) delete fMuonThetaDist;
    if (fMuonEnergyDist) delete fMuonEnergyDist;
}


//TODO move intialization of the source into its own function somewhere else
void MiniBooNEBeamlinePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  // CUSTOM_GENERATOR_HOOK
  // Add dispatch logic here for new generators
	//
	//Mei & Hime muon generator
  G4String sourceType = fMessenger.GetSourceType();
  if (sourceType == "muonGenerator") {
      GenerateMuonPrimaries(anEvent);
  }

  //TODO - if we can, it would be nice to fill the default primary channels here
}

// CUSTOM_GENERATOR_HOOK
// Your initialization methods go here
//
// Mei & Hime Muon Generator
void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeMuons() {
  G4double h0 = fMessenger.GetMuonEffectiveDepth();
  G4double h0_km = h0 / km;

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
      "exp(-[0]*[1]*([2]-1)) * (x + [3]*(1-exp(-[0]*[1])))^(-[2])", 1, 3000);
  fMuonEnergyDist->SetParameter(0, 0.4);
  //Par 1 set after sampling theta and calculating slant depth
  fMuonEnergyDist->SetParameter(2, 3.77);
  fMuonEnergyDist->SetParameter(3, 693);
  fMuonEnergyDist->SetNpx(1000);
}

// CUSTOM_GENERATOR_HOOK
// Your primary generator methods methods go here
//
// Mei & Hime Muon Generator
void MiniBooNEBeamlinePrimaryGeneratorAction::GenerateMuonPrimaries(G4Event* anEvent) {
  G4double h0 = fMessenger.GetMuonEffectiveDepth();
  G4double h0_km = h0 / km;

  //TODO: mu+ vs mu- at appropriate ratios--does it really even matter?
  G4ParticleDefinition* muonDef = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  fGPS->SetParticleDefinition(muonDef);

  G4double theta = fMuonThetaDist->GetRandom();
  G4double phi = (2.0 * M_PI - 0.001) * G4UniformRand();

  G4double h_km = h0_km / std::cos(theta);
  fMuonEnergyDist->SetParameter(1, h_km);
  G4double E_GeV = fMuonEnergyDist->GetRandom();
  fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E_GeV);

    /*
    G4cout << "[Sampled Muon]" << G4endl
           << "  Theta (zenith)     = " << theta / deg << " deg" << G4endl
           << "  Phi (azimuth)      = " << phi / deg << " deg" << G4endl
           << "  Slant depth        = " << h_km << " km.w.e" << G4endl
           << "  Energy             = " << E_GeV << " GeV" << G4endl;
    */
    
    G4ThreeVector position = SamplePointOnTopOfOverburden();
    G4ThreeVector direction(std::sin(theta) * std::cos(phi),
                            std::sin(theta) * std::sin(phi),
                            -std::cos(theta)); // downward
    fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);
    fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

    fGPS->GeneratePrimaryVertex(anEvent);

    // Record for output tree
    if (fManager.GetPrimariesTreeOutputStatus()) {
      G4double mass = muonDef->GetPDGMass();
      G4double momentumMag = std::sqrt(E_GeV * E_GeV - mass * mass);
      G4ThreeVector momentum = direction * momentumMag;
      fManager.PushPrimaryEventID(anEvent->GetEventID());
      fManager.PushPrimaryEventPDG(muonDef->GetPDGEncoding());
      fManager.PushPrimaryEventEnergy(E_GeV);
      fManager.PushPrimaryEventX(position.x());
      fManager.PushPrimaryEventY(position.y());
      fManager.PushPrimaryEventZ(position.z());
      fManager.PushPrimaryEventPX(momentum.x());
      fManager.PushPrimaryEventPY(momentum.y());
      fManager.PushPrimaryEventPZ(momentum.z());
      fManager.PushPrimaryMuonTheta(theta);
      fManager.PushPrimaryMuonPhi(phi);
      fManager.PushPrimaryMuonSlant(h_km);
    }
}

// CUSTOM_GENERATOR_HOOK
// Your initialization methods go here
//
// Mei & Hime Muon Generator
G4ThreeVector MiniBooNEBeamlinePrimaryGeneratorAction::SamplePointOnTopOfOverburden() {

    G4double sideLength = fMessenger.GetUserOverburdenSideLength();
    G4double halfSideLength = sideLength / 2.0;

    G4double x = (G4UniformRand() - 0.5) * sideLength;
    G4double y = (G4UniformRand() - 0.5) * sideLength;
    G4double z = halfSideLength;

    return G4ThreeVector(x, y, z);
}