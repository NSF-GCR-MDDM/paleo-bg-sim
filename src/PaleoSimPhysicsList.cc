#include "PaleoSimPhysicsList.hh"

#include "G4SystemOfUnits.hh"

// Electromagnetic and decay physics
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"  // muon-nuclear, gamma-nuclear, synchrotron

// Hadronic physics
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonPhysics.hh"

//Decay physics
#include "G4RadioactiveDecayPhysics.hh"
#include "G4ProcessTable.hh"
#include "G4RadioactiveDecay.hh"

//For volume-specific tracking cuts:
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4EmParameters.hh"

#include "G4RegionStore.hh"

PaleoSimPhysicsList::PaleoSimPhysicsList(PaleoSimMessenger& messenger): fMessenger(messenger)  {

  SetVerboseLevel(0);

  // Core EM physics
  RegisterPhysics(new G4EmStandardPhysics());
  RegisterPhysics(new G4EmExtraPhysics());

  // Decay physics
  RegisterPhysics(new G4DecayPhysics());

  // Hadronic physics
  //RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
  RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP()); //Pranav's study showed this may be more accurate, but slower
  RegisterPhysics(new G4HadronElasticPhysicsHP());
  
  RegisterPhysics(new G4StoppingPhysics());
  RegisterPhysics(new G4IonElasticPhysics());
  RegisterPhysics(new G4IonPhysics());

  // Radioactive decay physics
  auto* radioactiveDecayPhysics = new G4RadioactiveDecayPhysics();
  RegisterPhysics(radioactiveDecayPhysics);

  // Force decay at rest 
  G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
  G4RadioactiveDecay* rDecay = (G4RadioactiveDecay*)processTable->FindProcess("RadioactiveDecay", "GenericIon");
  if (rDecay) {
    rDecay->SetARM(true);
  }

  //Single Coulomb scattering in recoil regions: every elastic scatter of every
  //charged particle (e+-, muons, hadrons) is simulated individually, so nuclear
  //recoils are produced down to the region's "proton" production cut.
  auto* emParams = G4EmParameters::Instance();
  for (const auto& name : fMessenger.GetRecoilTreeVolumes()) {
    emParams->AddPhysics(name+"Region", "G4EmStandardSS");
  }
}

void PaleoSimPhysicsList::SetCuts() {
  SetCutsWithDefault();

  // Specialized cuts for tracking volumes
  for (auto name: fMessenger.GetRecoilTreeVolumes()) {
    G4Region* trackingRegion =
        G4RegionStore::GetInstance()->GetRegion(name+"Region", false);

    if (trackingRegion) {
      G4cout << "Applying production cuts to volume: "
             << name << G4endl;

      auto* cuts = new G4ProductionCuts();
      cuts->SetProductionCut(50*nanometer, "proton"); //5 eV

      trackingRegion->SetProductionCuts(cuts);
    }
    else {
      G4cout << "WARNING: Could not find region: "
             << name+"Region" << G4endl;
    }
  }
}