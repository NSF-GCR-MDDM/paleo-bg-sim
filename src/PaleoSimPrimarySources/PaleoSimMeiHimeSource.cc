

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "PaleoSimUserEventInformation.hh"

#include "PaleoSimPrimarySources/PaleoSimMeiHimeSource.hh"

PaleoSimMeiHimeSource::PaleoSimMeiHimeSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimMeiHimeSource::~PaleoSimMeiHimeSource() {
}

void PaleoSimMeiHimeSource::InitializeSource() {
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

void PaleoSimMeiHimeSource::GeneratePrimaries(G4Event* anEvent) {
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
