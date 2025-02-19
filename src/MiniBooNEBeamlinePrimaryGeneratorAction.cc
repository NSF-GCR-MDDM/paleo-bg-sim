#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>
#include <vector>

MiniBooNEBeamlinePrimaryGeneratorAction::MiniBooNEBeamlinePrimaryGeneratorAction()
:G4VUserPrimaryGeneratorAction(),
 fParticleGun(0) {
	G4int n_particle = 1; // Number of particles you shoot at a time
	fParticleGun = new G4ParticleGun(n_particle);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle = particleTable->FindParticle(particleName="mu-");
	fParticleGun->SetParticleDefinition(particle);

	// Initialize CDF for energy sampling
	InitializeAngularDistribution();
}

MiniBooNEBeamlinePrimaryGeneratorAction::~MiniBooNEBeamlinePrimaryGeneratorAction() {
	delete fParticleGun;
}

void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeAngularDistribution() {
	// Constants:
	const double h0 = 6.011;
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
}

void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeEnergyDistribution(double theta) {
	// Constants:
	const double h0 = 6.011;
	const double b = 0.4;
	const double gamma = 3.77;
	const double epsilon = 693;
	const double dE = 0.01;

	// Compute depth for given theta
	double h = 6.011 * (1 / std::cos(theta));

	// Clear previous energy distributions
	e_intervals.clear();
	e_cdf.clear();

	// Define the energy range
	double E_min = 1;
	double E_max = 4000;
	for (double E = E_min; E < E_max; E += dE) {
		e_intervals.push_back(E);
	}

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
}

double MiniBooNEBeamlinePrimaryGeneratorAction::SampleCDF(std::vector<double> cdf,
														  std::vector<double> intervals) {
	double random_value = G4UniformRand(); // Generates a random value between 0 and 1
	std::vector<double>::iterator it = std::lower_bound(cdf.begin(), cdf.end(), random_value);
	size_t index = std::distance(cdf.begin(), it);
	return intervals[index];
}

void MiniBooNEBeamlinePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// Generate energy from the distribution
	double sampled_theta = SampleCDF(theta_cdf, theta_intervals);
	InitializeEnergyDistribution(sampled_theta);
	double sampled_energy = SampleCDF(e_cdf, e_intervals);
	fParticleGun->SetParticleEnergy(sampled_energy * GeV);

	// Set position and direction to match the desired angle.
	fParticleGun->SetParticlePosition(G4ThreeVector(0 * m, 0 * m, 0 * m));

	G4double phi = 2.0 * M_PI * G4UniformRand();
	G4double dirZ = std::cos(sampled_theta);
	G4double sin_theta = std::sin(sampled_theta);
	G4double dirX = sin_theta * std::cos(phi);
    G4double dirY = sin_theta * std::sin(phi);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dirX, dirY, dirZ));

	fParticleGun->GeneratePrimaryVertex(anEvent);
}
