#ifndef PaleoSimPrimaryGeneratorAction_h
#define PaleoSimPrimaryGeneratorAction_h 1

#include <vector>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "TF1.h"
#include "TH2D.h"
#include "TTree.h"

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
    G4bool IsWithinTopSurface(const G4ThreeVector& point);

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
    //
    //CRY generator
    TFile* cryFile = nullptr;
    bool cryFileLoaded = false;
    TTree* cryTree = nullptr;
    int nCryEntries = 0;
    std::vector<int>* cry_pdgcode = nullptr;
    std::vector<float>* cry_energy = nullptr;
    std::vector<float>* cry_u = nullptr;
    std::vector<float>* cry_v = nullptr;
    std::vector<float>* cry_w = nullptr;
    std::vector<float>* cry_x = nullptr;
    std::vector<float>* cry_y = nullptr;
    void InitializeCRYGenerator();
    void GenerateCRYPrimaries(G4Event*);
    //
    //Disk source generator
    TFile* diskSourceSpectrumFile = nullptr;
    bool diskSourceSpectrumFileLoaded = false;
    TH1D* diskSourceSpectrumHist = nullptr;
    int diskSourcePDGCode;
    G4ThreeVector diskSourcePosition;
    G4String diskSourceType;
    void InitializeDiskSourceGenerator();
    void GenerateDiskSourcePrimaries(G4Event*);
    G4ThreeVector SamplePointOnDisk(double radius,const G4ThreeVector& position, const G4ThreeVector& axis);

};

#endif







