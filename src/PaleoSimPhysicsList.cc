#include "PaleoSimPhysicsList.hh"

// Electromagnetic and decay physics
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"  // muon-nuclear, gamma-nuclear, synchrotron

// Hadronic physics
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4NeutronTrackingCut.hh"

PaleoSimPhysicsList::PaleoSimPhysicsList() {
    SetVerboseLevel(0);

    // Core EM and decay physics
    RegisterPhysics(new G4EmStandardPhysics()); //option_4?
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4EmExtraPhysics());

    // Hadronic physics with high-precision neutron support
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
    RegisterPhysics(new G4HadronElasticPhysicsHP());
    RegisterPhysics(new G4NeutronTrackingCut()); //Modify

    //Add to specific volumes specialized tracking cuts

    //Add G4RadioactiveDecayPhysics, G4IonPhysics
  }
