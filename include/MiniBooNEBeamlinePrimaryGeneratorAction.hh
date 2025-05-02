#ifndef MiniBooNEBeamlinePrimaryGeneratorAction_h
#define MiniBooNEBeamlinePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include <vector>
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "TF1.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"

class MiniBooNEBeamlinePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    MiniBooNEBeamlinePrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                            PaleoSimOutputManager& manager);
    virtual ~MiniBooNEBeamlinePrimaryGeneratorAction() override;

    virtual void GeneratePrimaries(G4Event*);

    //CUSTOM_GENERATOR_HOOK
    //Add public setter methods for your generator parameters here
    //
    // Mei & Hime muon generator setters
    void SetMuonEffectiveDepth(G4double depth);

private:
    G4GeneralParticleSource* fGPS;
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fManager;
    
    std::vector<G4String> fValidSourceTypes;

    //CUSTOM_GENERATOR_HOOK
    // Add private state and methods for your generator implementation here
    //
    // TF1 version of Mei & Hime muon generator internals
    TF1* fMuonThetaDist = nullptr;
    TF1* fMuonEnergyDist = nullptr;
    void InitializeMuons();
    void GenerateMuonPrimaries(G4Event*);
    G4ThreeVector SamplePointOnTopOfOverburden();
    //
    // C++ version of Mei & Hime muon generator internals (from Alex)
    void InitializeAngularDistribution(); // Initializes the angular distribution
    void InitializeEnergyIntervals(); // Initializes e_intervals
	  std::vector<double> GetEnergyDistribution(double theta); // Gets energy distribution from angle
    double SampleCDF(std::vector<double> cdf, std::vector<double> intervals); // Samples energy from the distribution
    // Data members for energy/angular distributions
    std::vector<double> e_intervals; // Energy intervals
    std::vector<double> theta_intervals; // Angle intervals
    std::vector<double> theta_cdf; // Cumulative distribution function for angles
	  std::map<double, std::vector<double> > e_cdfs;
	  void GenerateMuonPrimariesCPP(G4Event*);
};

#endif







