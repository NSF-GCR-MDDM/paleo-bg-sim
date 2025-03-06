//Run Action

#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "MiniBooNEBeamlineAnalysis.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

MiniBooNEBeamlineRunAction::MiniBooNEBeamlineRunAction()
: G4UserRunAction()
{ 
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  //Create some n-tuples here
  //This n-tuple will keep track of the properties of decaying kaons
  analysisManager->CreateNtuple("kaonVectors", "Kaon Kinematics");
  analysisManager->CreateNtupleIColumn(0, "PDGCode");
  analysisManager->CreateNtupleDColumn(0, "kinE");
  analysisManager->CreateNtupleDColumn(0, "pX");
  analysisManager->CreateNtupleDColumn(0, "pY");
  analysisManager->CreateNtupleDColumn(0, "pZ");
  analysisManager->CreateNtupleDColumn(0, "x");
  analysisManager->CreateNtupleDColumn(0, "y");
  analysisManager->CreateNtupleDColumn(0, "z");
  analysisManager->CreateNtupleSColumn(0, "volume");
  analysisManager->CreateNtupleDColumn(0, "xVtx");
  analysisManager->CreateNtupleDColumn(0, "yVtx");
  analysisManager->CreateNtupleDColumn(0, "zVtx");
  analysisManager->CreateNtupleSColumn(0, "volumeVtx");
  analysisManager->FinishNtuple();

  //This n-tuple will keep track of the neutrino parent properties
  //Parent properties are saved when a neutrino parent decays
  analysisManager->CreateNtuple("MuonTree", "Parent Muon Variables Variables");
  analysisManager->CreateNtupleIColumn(1, "PDGCode");
  analysisManager->CreateNtupleDColumn(1, "kinE");
  analysisManager->CreateNtupleDColumn(1, "xMom");
  analysisManager->CreateNtupleDColumn(1, "yMom");
  analysisManager->CreateNtupleDColumn(1, "zMom");
  analysisManager->CreateNtupleDColumn(1, "xPos");
  analysisManager->CreateNtupleDColumn(1, "yPos");
  analysisManager->CreateNtupleDColumn(1, "zPos");
  analysisManager->FinishNtuple();

  //Create some histograms here
  //First some histograms of the kinematics of particles created by primary
  //proton interactions with the target. Track the momentum magnitude in MeV/c
  //and the angle w.r.t. the beam direction in mrad
  //track length is reported in um
  analysisManager->CreateH1("protonMom", "Proton Momentum", 100, 0, 5000);
  analysisManager->CreateH1("protonAngle", "Proton Angle", 100, 0, 2000);
  analysisManager->CreateH1("neutronMom", "Neutron Momentum", 100, 0, 5000);
  analysisManager->CreateH1("neutronParentAngle", "Neutron Angle Relative to Parent",
							100, -1, 1);
  analysisManager->CreateH1("neutronKin", "Neutron KE", 70, 10, 3510);
  analysisManager->CreateH1("MuonKin", "Muon KE", 1000, 1000, 3000000);
 
  //Also plots verifying the beam properties
  analysisManager->CreateH2("beamProfile2D", "beamProfile2D",
							50, -5.0, 5.0, 50, -5.0, 5.0);
  analysisManager->CreateH2("beamAngularDist2D", "beamAngularDist2D",
							50, -2.0, 2.0, 50, -2.0, 2.0);
}

MiniBooNEBeamlineRunAction::~MiniBooNEBeamlineRunAction()
{
  delete G4AnalysisManager::Instance();
}

void MiniBooNEBeamlineRunAction::BeginOfRunAction(const G4Run*)
{ 
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  //Create the output file; filename should be set in the macro
  //using the /analysis/setFileName "filename" command
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile();
}

void MiniBooNEBeamlineRunAction::EndOfRunAction(const G4Run* run)
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables 

  analysisManager->Write();
  analysisManager->CloseFile();

}
