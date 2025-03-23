#ifndef MiniBooNEBeamlinePrimaryGeneratorAction_h
#define MiniBooNEBeamlinePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include <vector>
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "TF1.h"

class MiniBooNEBeamlinePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    MiniBooNEBeamlinePrimaryGeneratorAction();
    virtual ~MiniBooNEBeamlinePrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

    // Method to set source type from macro
    void SetSourceType(const G4String& sourceType);

    //CUSTOM_GENERATOR_HOOK
    //Add public setter methods for your generator parameters here
    //
    // Mei & Hime muon generator setters
    void SetMuonEffectiveDepth(G4double depth);

private:
    G4GeneralParticleSource* fGPS = nullptr;
    G4String fSourceType = ""; // For choosing source via macro command
    std::vector<G4String> fValidSourceTypes;

    // For storing output info of primaries
    std::vector<int> fPrimaryPDGIDs;
    std::vector<double> fPrimaryEnergies;
    std::vector<double> fPrimary_x, fPrimary_y, fPrimary_z;
    std::vector<double> fPrimary_px, fPrimary_py, fPrimary_pz;
    //CUSTOM_GENERATOR_HOOK
    //If you want custom primary properties stored to the output tree, put them here
    //
    // Mei & Hime muon generator variables written to tree, set to -1 for others
    std::vector<double> fMuonTheta, fMuonPhi, fMuonSlant;

    // Primary info helpers
    void AddPrimaries(int pdgID,
                       double energy,
                       const G4ThreeVector& position,
                       const G4ThreeVector& momentum,
                       //CUSTOM_GENERATOR_HOOK
                       //If you want custom primary properties stored to the output tree, put them here (w/-1 as default)
                       //
                       // Mei & Hime muon generator vars written
                       double theta = -1,
                       double phi = -1,
                       double slant = -1);

    void FillPrimaries(int eventID);
    void ClearPrimaries();

    //CUSTOM_GENERATOR_HOOK
    // Add private state and methods for your generator implementation here
    //
    // Mei & Hime muon generator internals
    G4double h0 = 6000 * km;
    TF1* fMuonThetaDist = nullptr;
    TF1* fMuonEnergyDist = nullptr;
    G4double muonEps = 0.01;
    void InitializeMuons();
    void GenerateMuonPrimaries(G4Event*);
    G4ThreeVector SamplePointOnTopOfOverburden() const;
};

#endif
