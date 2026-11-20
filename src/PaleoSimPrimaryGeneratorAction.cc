#include "PaleoSimPrimaryGeneratorAction.hh"
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
#include "G4RandomDirection.hh"
#include "G4LogicalVolumeStore.hh"
#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimVolumeDefinition.hh"

#include "TF1.h"
#include <algorithm>
#include <vector>
#include <chrono>

PaleoSimPrimaryGeneratorAction::PaleoSimPrimaryGeneratorAction(PaleoSimMessenger& messenger,
                                                                                 PaleoSimOutputManager& manager)
: G4VUserPrimaryGeneratorAction(),  
  fMessenger(messenger),
  fManager(manager)
{

  //Check whether we were passed a valid source type
  G4String sourceType = fMessenger.GetSourceType();
  std::vector<G4String> validSourceTypes = fMessenger.GetValidSourceTypes();
  if (std::find(validSourceTypes.begin(), validSourceTypes.end(), sourceType) == validSourceTypes.end()) {
      G4Exception("PaleoSimPrimaryGeneratorAction", "InvalidSource", FatalException,
                  ("Invalid source type: " + sourceType).c_str());
  }

  // CUSTOM_GENERATOR_HOOK
  // Add initialization calls for new generators
  //
  if (sourceType == "meiHimeMuonGenerator") {
    fMeiHimeSource = new PaleoSimMeiHimeSource(fMessenger);
    fMeiHimeSource->InitializeSource();
  }
  else if (sourceType == "muteGenerator") {
    fMuteSource = new PaleoSimMuteSource(fMessenger);
    fMuteSource->InitializeSource();
  }
  else if (sourceType == "CRYGenerator") {
    fCrySource = new PaleoSimCrySource(fMessenger);
    fCrySource->InitializeSource();
  }
  
  else if (sourceType == "SCTGenerator") {
    fSCTSource = new PaleoSimSCTSource(fMessenger);
    fSCTSource->InitializeSource();
  }
  else if (sourceType == "volumetricSource") {
    fVolumetricSource = new PaleoSimVolumetricSource(fMessenger);
    fVolumetricSource->InitializeSource();
  }
  else if (sourceType == "beamSource") {
    fBeamSource = new PaleoSimBeamSource(fMessenger);
    fBeamSource->InitializeSource();
  }
  else {
    G4Exception("PaleoSimPrimaryGeneratorAction::PaleoSimPrimaryGeneratorAction", "001", FatalException,
                ("Source not valid: " + sourceType).c_str());
  }
}

PaleoSimPrimaryGeneratorAction::~PaleoSimPrimaryGeneratorAction() {
    // CUSTOM_GENERATOR_HOOK
    // Add memory clean up here for new generators
    //
    delete fMeiHimeSource;
    delete fVolumetricSource;
    delete fMuteSource;
    delete fBeamSource;
    delete fCrySource;
    delete fSCTSource;
}

void PaleoSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  // CUSTOM_GENERATOR_HOOK
  // Add dispatch logic here for new generators
	//
	//Mei & Hime muon generator
  G4String sourceType = fMessenger.GetSourceType();
  if (sourceType == "meiHimeMuonGenerator") {
    fMeiHimeSource->GeneratePrimaries(anEvent);
  }
  //MUTE
  else if (sourceType == "muteGenerator") {
    fMuteSource->GeneratePrimaries(anEvent);
  }
  //CRY
  else if (sourceType == "CRYGenerator") {
    fCrySource->GeneratePrimaries(anEvent);
  }
  //Captured Particle Generator
  else if (sourceType == "SCTGenerator") {
    fSCTSource->GeneratePrimaries(anEvent);
  }
  //Volumetric source
  else if (sourceType == "volumetricSource") {
    fVolumetricSource->GeneratePrimaries(anEvent);
  }
  //Beam source
  else if (sourceType == "beamSource") {  
    fBeamSource->GeneratePrimaries(anEvent);
  }
  else {
    G4Exception("PaleoSimPrimaryGeneratorAction::GeneratePrimaries", "GeneratePrimaries001", FatalException,
                ("Invalid source: " + sourceType).c_str());
  }
}