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
    // Mei & Hime muon generator internals
    TF1* fMuonThetaDist = nullptr;
    TF1* fMuonEnergyDist = nullptr;
    void InitializeMuons();
    void GenerateMuonPrimaries(G4Event*);
    G4ThreeVector SamplePointOnTopOfOverburden() const;
};

#endif
