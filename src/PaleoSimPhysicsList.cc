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

PaleoSimPhysicsList::PaleoSimPhysicsList() {
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
  G4LogicalVolume* target = G4LogicalVolumeStore::GetInstance()->GetVolume("Target", false);
  if (target) {
      auto* targetRegion = new G4Region("TargetRegion");
      targetRegion->AddRootLogicalVolume(target);

      auto* cuts = new G4ProductionCuts();
      cuts->SetProductionCut(10*nanometer, "proton");
      cuts->SetProductionCut(10*nanometer, "alpha");

      targetRegion->SetProductionCuts(cuts);
  }

  // Add muon-nuclear process
  auto* muNuclearModel = new G4MuonVDNuclearModel(); // or G4MuonNuclearInteractionModel
  auto* muNuclearProc  = new G4MuonNuclearProcess(); 
  muNuclearProc->RegisterMe(muNuclearModel);

  G4ProcessManager* muPlusPM  = G4MuonPlus::MuonPlus()->GetProcessManager();
  G4ProcessManager* muMinusPM = G4MuonMinus::MuonMinus()->GetProcessManager();

  muPlusPM->AddDiscreteProcess(muNuclearProc);
  muMinusPM->AddDiscreteProcess(muNuclearProc);
}
