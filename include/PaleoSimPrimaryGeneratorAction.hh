#ifndef PaleoSimPrimaryGeneratorAction_h
#define PaleoSimPrimaryGeneratorAction_h 1

#include <vector>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"

#include "TF1.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"

#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimVolumeDefinition.hh"
#include "PaleoSimPrimarySources/PaleoSimDiskSource.hh"
#include "PaleoSimPrimarySources/PaleoSimVolumetricSource.hh"
#include "PaleoSimPrimarySources/PaleoSimMeiHimeSource.hh"
#include "PaleoSimPrimarySources/PaleoSimCrySource.hh"

class PaleoSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PaleoSimPrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                            PaleoSimOutputManager& manager);
    virtual ~PaleoSimPrimaryGeneratorAction() override;

    void InitializeSource();
    virtual void GeneratePrimaries(G4Event*);

private:
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fManager;

    //Mute generator
    //TH2D* fMuteHist = nullptr;
    //void InitializeMuteMuons();
    //void GenerateMutePrimaries(G4Event*);
    //
    //CRY generator
    //
    //Volumetric source generator
    PaleoSimDiskSource* fDiskSource = nullptr;
    PaleoSimVolumetricSource* fVolumetricSource = nullptr;
    PaleoSimMeiHimeSource* fMeiHimeSource = nullptr;
    PaleoSimCrySource* fCrySource = nullptr;
};

#endif
