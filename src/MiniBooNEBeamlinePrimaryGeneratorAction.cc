#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineConstruction.hh"
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

MiniBooNEBeamlinePrimaryGeneratorAction::MiniBooNEBeamlinePrimaryGeneratorAction(PaleoSimMessenger& messenger,
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
  else if (sourceType == "muonGeneratorC++") {
    InitializeAngularDistribution(); // Initializes the angular distribution
    InitializeEnergyIntervals();
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
  else if (sourceType == "muonGeneratorC++") {
    GenerateMuonPrimariesCPP(anEvent);
  }
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
/////////////////////////////////////
// Mei & Hime Muon Generator (TF1s)//
/////////////////////////////////////
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
  fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E_GeV*GeV);

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

  //We store custom variables in a UserEventInfo to pass them to beginningOfEventAction to load into trees
  auto* info = new PaleoSimUserEventInformation();
  info->muonTheta.push_back(theta);
  info->muonPhi.push_back(phi);
  info->muonSlantDepth.push_back(h_km*km);
  G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->SetUserInformation(info); //G4 takes ownership, no need to delete
}

// Mei & Hime Muon Generator
G4ThreeVector MiniBooNEBeamlinePrimaryGeneratorAction::SamplePointOnTopOfOverburden() {

    G4double sideLength = fMessenger.GetUserOverburdenSideLength();
    G4double halfSideLength = sideLength / 2.0;

    G4double x = (G4UniformRand() - 0.5) * sideLength;
    G4double y = (G4UniformRand() - 0.5) * sideLength;
    G4double z = halfSideLength-0.0001;

    return G4ThreeVector(x, y, z);
}

/////////////////////////////////////////////////
// Mei & Hime muon generator in C++ (from Alex)//
/////////////////////////////////////////////////

void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeAngularDistribution() {
	// Constants:
	const double h0 = fMessenger.GetMuonEffectiveDepth();
  const double h0_km = h0/km;
	const double dtheta = 0.001;
	const double I1 = 0.0000086;
	const double I2 = 0.00000047;
	const double lambda1 = 0.45;
	const double lambda2 = 0.87;

	double theta_min = -M_PI_2 + 0.001;
	double theta_max = M_PI_2 - 0.001;
	for (double theta = theta_min; theta < theta_max; theta += dtheta) {
		theta_intervals.push_back(theta);
	}

	// Calculate the angle PDF and CDF
	double cumulative = 0.0;
	for (size_t i = 0; i < theta_intervals.size() - 1; ++i) {
		double theta_mid = 0.5 * (theta_intervals[i] + theta_intervals[i + 1]);
		double pdf_value = I1 * std::exp(-h0 / lambda1 / std::cos(theta_mid));
		pdf_value += I2 * std::exp(-h0 / lambda2 / std::cos(theta_mid));
		pdf_value /= std::cos(theta_mid);
		double probability = pdf_value * dtheta;
		cumulative += probability;
		theta_cdf.push_back(cumulative);
	}

	// Normalize the angle CDF
	for (size_t i = 0; i < theta_cdf.size(); ++i) {
		theta_cdf[i] /= cumulative;
	}

	return;
}

void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeEnergyIntervals() {
	  const double E_min = 1.0;
    const double E_max = 4000.0;
    const double dE = 0.01;

	for (double E = E_min; E < E_max; E += dE) {
		e_intervals.push_back(E);
	}
}

std::vector<double> MiniBooNEBeamlinePrimaryGeneratorAction::GetEnergyDistribution(double theta) {
	// Constants:
	const double h0 = fMessenger.GetMuonEffectiveDepth();
  const double h0_km = h0/km;
	const double b = 0.4;
	const double gamma = 3.77;
	const double epsilon = 693;
	const double dE = 0.01;

	// Compute depth for given theta
	double h = h0 * (1 / std::cos(theta));

	// Clear previous energy distributions
	std::vector<double> e_cdf;

	// Compute the energy PDF and CDF
	double cumulative = 0.0;
	for (size_t i = 0; i < e_intervals.size() - 1; ++i) {
		double x_mid = 0.5 * (e_intervals[i] + e_intervals[i + 1]);
		double pdf_value = std::exp(-b * h * (gamma - 1));
		pdf_value *= std::pow(x_mid + epsilon * (1 - std::exp(-b * h)), -gamma);
		double probability = pdf_value * dE;
		cumulative += probability;
		e_cdf.push_back(cumulative);
	}

	// Normalize the energy CDF
	for (size_t i = 0; i < e_cdf.size(); ++i) {
		e_cdf[i] /= cumulative;
	}

	return e_cdf;
}

double MiniBooNEBeamlinePrimaryGeneratorAction::SampleCDF(std::vector<double> cdf,
														  std::vector<double> intervals) {
	double random_value = G4UniformRand(); // Generates a random value between 0 and 1
	std::vector<double>::iterator it = std::lower_bound(cdf.begin(), cdf.end(), random_value);
	size_t index = std::distance(cdf.begin(), it);
	return intervals[index];
}

void MiniBooNEBeamlinePrimaryGeneratorAction::GenerateMuonPrimariesCPP(G4Event* anEvent) {
	// Generate energy from the distribution
	double sampled_theta = SampleCDF(theta_cdf, theta_intervals);
	std::vector<double> e_cdf = GetEnergyDistribution(sampled_theta);
	double sampled_energy = SampleCDF(e_cdf, e_intervals);
  fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(sampled_energy*GeV);


	// Set position and direction to match the desired angle.
	fGPS->SetParticlePosition(G4ThreeVector(0 * m, 0 * m, 0 * m));

	G4double phi = 2.0 * M_PI * G4UniformRand();
	G4double dirZ = std::cos(sampled_theta);
	G4double sin_theta = std::sin(sampled_theta);
	G4double dirX = sin_theta * std::cos(phi);
  G4double dirY = sin_theta * std::sin(phi);
  fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(dirX, dirY, dirZ));

	fGPS->GeneratePrimaryVertex(anEvent);

  //We store custom variables in a UserEventInfo to pass them to beginningOfEventAction to load into trees
  auto* info = new PaleoSimUserEventInformation();
  info->muonTheta.push_back(sampled_theta);
  info->muonPhi.push_back(phi);
  info->muonSlantDepth.push_back(fMessenger.GetMuonEffectiveDepth()/std::cos(sampled_theta));
  G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->SetUserInformation(info); //G4 takes ownership, no need to delete

	return;
}
