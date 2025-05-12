#ifndef PaleoSimPrimaryGeneratorAction_h
#define PaleoSimPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include <vector>
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "TF1.h"
#include "TH2D.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"

class PaleoSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PaleoSimPrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                            PaleoSimOutputManager& manager);
    virtual ~PaleoSimPrimaryGeneratorAction() override;

    virtual void GeneratePrimaries(G4Event*);

private:
    G4GeneralParticleSource* fGPS;
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fManager;

    //Helper functions - could be used by any class
    G4ThreeVector SamplePointOnTopOfWorldVolume();

    //CUSTOM_GENERATOR_HOOK
    // Add private state and methods for your generator implementation here
    //
    // TF1 version of Mei & Hime muon generator internals
    TF1* fMuonThetaDist = nullptr;
    TF1* fMuonEnergyDist = nullptr;
    void InitializeMeiHimeMuons();
    void GenerateMeiHimeMuonPrimaries(G4Event*);
    //
    //Mute generator
    TH2D* fMuteHist = nullptr;
    void InitializeMuteMuons();
    void GenerateMutePrimaries(G4Event*);
};

#endif







