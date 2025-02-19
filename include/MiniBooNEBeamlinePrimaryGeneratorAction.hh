//Primary Generator

#ifndef MiniBooNEBeamlinePrimaryGeneratorAction_h
#define MiniBooNEBeamlinePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class MiniBooNEBeamlinePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    MiniBooNEBeamlinePrimaryGeneratorAction();    
    virtual ~MiniBooNEBeamlinePrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);         
  
    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  
  private:
    G4ParticleGun* fParticleGun; // pointer a to G4 gun class

  //ALEX_NOV_25_2024
    void InitializeAngularDistribution(); // Initializes the angular distribution
	// Initializes energy distribution from angular distribution
	void InitializeEnergyDistribution(double theta);
	// Samples energy from the distribution
    double SampleCDF(std::vector<double> cdf, std::vector<double> intervals);

    // Data members for energy distribution
    std::vector<double> e_intervals; // Energy intervals
    std::vector<double> theta_intervals; // Angle intervals
    std::vector<double> theta_cdf; // Cumulative distribution function for angles
    std::vector<double> e_cdf; // Cumulative distribution function for energy
  //ALEX

};

#endif
