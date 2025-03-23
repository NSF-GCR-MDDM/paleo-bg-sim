//Primary Generator
#ifndef MiniBooNEBeamlinePrimaryGeneratorAction_h
#define MiniBooNEBeamlinePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include <vector>
#include "G4SystemOfUnits.hh"

#include "TF1.h"
#include "G4ThreeVector.hh"

class MiniBooNEBeamlinePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    MiniBooNEBeamlinePrimaryGeneratorAction();
    virtual ~MiniBooNEBeamlinePrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

    // Method to set source type from macro
    void SetSourceType(const G4String& sourceType);

    //Muon generator setters
    void SetMuonEffectiveDepth(G4double depth);

private:
    G4GeneralParticleSource* fGPS = nullptr;
    G4String fSourceType = ""; // For choosing source via macro command

    std::vector<G4String> fValidSourceTypes;

    //Muon generator private functions and variables
    G4double h0 = 6000 * km;
    TF1* fMuonThetaDist = nullptr;
    TF1* fMuonPhiDist = nullptr;
    TF1* fMuonEnergyDist = nullptr;
    void InitializeMuons();
    void GenerateMuonPrimaries(G4Event*);
    G4ThreeVector SamplePointOnTopOfOverburden() const;
};

#endif
