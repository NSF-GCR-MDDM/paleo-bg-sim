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
#include "G4NeutronTrackingCut.hh"

//Decay physics
#include "G4RadioactiveDecayPhysics.hh"
#include "G4ProcessTable.hh"
#include "G4RadioactiveDecay.hh"

//For volume-specific tracking cuts:
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4LogicalVolumeStore.hh"

// Muon nuclear interaction
#include "G4MuonNuclearProcess.hh"
#include "G4MuonVDNuclearModel.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"

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
  RegisterPhysics(new G4NeutronTrackingCut());

  // Radioactive decay physics
  auto* radioactiveDecayPhysics = new G4RadioactiveDecayPhysics();
  RegisterPhysics(radioactiveDecayPhysics);

  // Force decay at rest 
  G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
  G4RadioactiveDecay* rDecay = (G4RadioactiveDecay*)processTable->FindProcess("RadioactiveDecay", "GenericIon");
  if (rDecay) {
    rDecay->SetARM(true);
  }

  // Specialized cuts for "Target" volume
  for (auto volume: fMessenger.GetVolumes()) {
    G4String name = volume->name;
    G4LogicalVolume* trackingVolume = G4LogicalVolumeStore::GetInstance()->GetVolume(name, false);
    if (trackingVolume) {
        auto* trackingRegion = new G4Region(name+"Region");
        trackingRegion->AddRootLogicalVolume(trackingVolume);

        auto* cuts = new G4ProductionCuts();
        cuts->SetProductionCut(10*nanometer, "proton"); //Sets for all charged hadrons
        cuts->SetProductionCut(10*nanometer, "alpha");

        trackingRegion->SetProductionCuts(cuts);
    }
  }
}

void PaleoSimPhysicsList::ConstructProcess() {
  G4VModularPhysicsList::ConstructProcess(); 

  // Custom muon nuclear process
  auto* model = new G4MuonVDNuclearModel(); // or G4MuonNuclearInteractionModel
  auto* proc  = new G4MuonNuclearProcess();
  proc->RegisterMe(model);

  G4MuonPlus::MuonPlus()->GetProcessManager()->AddDiscreteProcess(proc);
  G4MuonMinus::MuonMinus()->GetProcessManager()->AddDiscreteProcess(proc);
}