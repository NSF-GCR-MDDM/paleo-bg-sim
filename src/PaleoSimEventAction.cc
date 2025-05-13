#include "PaleoSimOutputManager.hh"
#include "G4Event.hh" 
#include "PaleoSimEventAction.hh"
#include "PaleoSimUserEventInformation.hh"

PaleoSimEventAction::PaleoSimEventAction(PaleoSimMessenger &messenger,PaleoSimOutputManager& manager)
: G4UserEventAction(), fMessenger(messenger), fOutputManager(manager) {}

PaleoSimEventAction::~PaleoSimEventAction() {}


void PaleoSimEventAction::BeginOfEventAction(const G4Event* event) {
  if (event->GetEventID() % 100 == 0) {
      G4cout << "Event # " << event->GetEventID() << " of " << fMessenger.GetNPS() << G4endl;
  }

  //We calculate the position and momentum of events. For single particle sources
  //this is easy. For multiparticle sources we calculate the weighted position/momentum
  G4double totalEnergy = 0.0;
  G4ThreeVector primaryMomentum(0, 0, 0);
  G4ThreeVector primaryPosition(0, 0, 0);

  // Loop over all vertices and all particles
  for (G4int ivert = 0; ivert < event->GetNumberOfPrimaryVertex(); ivert++) {
    G4PrimaryVertex* vertex = event->GetPrimaryVertex(ivert);
    G4ThreeVector vertexPos = vertex->GetPosition();
    G4PrimaryParticle* particle = vertex->GetPrimary();

    while (particle) {
      G4double energy = particle->GetTotalEnergy();
      G4ThreeVector p(particle->GetPx(), particle->GetPy(), particle->GetPz());
      primaryMomentum += p;
      primaryPosition += (vertexPos * energy);
      totalEnergy += energy;
      particle = particle->GetNext();
    }
  }

  //Write this to our user event info
  auto* info = dynamic_cast<PaleoSimUserEventInformation*>(event->GetUserInformation());
  if (info && totalEnergy > 0) {
      info->primaryDirection = primaryMomentum.unit();
      info->primaryGenerationPosition = (primaryPosition / totalEnergy);
  }

  /////////////////////////////////////////////
  // Fill primaries tree if enabled
  /////////////////////////////////////////////
  if (fMessenger.GetPrimariesTreeStatus()) {
    fOutputManager.PushPrimaryEventID(event->GetEventID());

    for (G4int ivert = 0; ivert < event->GetNumberOfPrimaryVertex(); ivert++) {
      G4PrimaryVertex* vertex = event->GetPrimaryVertex(ivert);
      G4ThreeVector pos = vertex->GetPosition();
      G4PrimaryParticle* particle = vertex->GetPrimary();

      while (particle) {
        G4int pdg = particle->GetPDGcode();
        G4double energy = particle->GetTotalEnergy();
        G4ThreeVector momentum(particle->GetPx(), particle->GetPy(), particle->GetPz());

        fOutputManager.PushPrimaryEventPDG(pdg);
        fOutputManager.PushPrimaryEventEnergy(energy);
        fOutputManager.PushPrimaryEventX(pos.x());
        fOutputManager.PushPrimaryEventY(pos.y());
        fOutputManager.PushPrimaryEventZ(pos.z());
        fOutputManager.PushPrimaryEventPx(momentum.x());
        fOutputManager.PushPrimaryEventPy(momentum.y());
        fOutputManager.PushPrimaryEventPz(momentum.z());

        particle = particle->GetNext();
      }
    }

    // Generator-specific metadata
    auto* info = dynamic_cast<PaleoSimUserEventInformation*>(event->GetUserInformation());
    G4String srcType = fMessenger.GetSourceType();

    if (srcType == "meiHimeMuonGenerator" || srcType == "muteGenerator") {
      fOutputManager.SetPrimaryMuonTheta(info->muonTheta);
      fOutputManager.SetPrimaryMuonPhi(info->muonPhi);
    }
    if (srcType == "meiHimeMuonGenerator") {
      fOutputManager.SetPrimaryMuonSlant(info->muonSlantDepth);
    }

    if (srcType == "CRYGenerator") {
      fOutputManager.SetCRYCoreX(info->CRYCorePosition.x());
      fOutputManager.SetCRYCoreY(info->CRYCorePosition.y());
      fOutputManager.SetCRYCoreZ(info->CRYCorePosition.z());
      fOutputManager.SetCRYCoreTheta(info->CRYCoreTheta);
      fOutputManager.SetCRYCorePhi(info->CRYCorePhi);
      fOutputManager.SetCRYTotalEnergy(info->CRYTotalEnergy);
    }
  }
}


void PaleoSimEventAction::EndOfEventAction(const G4Event* event) {

  //Fill trees at end of event, then clear
  //Data loaded into Primaries tree variables in generator
  if (fMessenger.GetPrimariesTreeStatus()) {
    fOutputManager.FillPrimariesTreeEvent();
    fOutputManager.ClearPrimariesTreeEvent();
  }

  //Data loaded onto MINTree in stepping action
  if (fMessenger.GetMINTreeStatus()) {
    fOutputManager.FillMINTreeEvent();
    fOutputManager.ClearMINTreeEvent();
  }

  //Data loaded into NeutronTallyTree variables in stepping action
  if (fMessenger.GetNeutronTallyTreeStatus()) {
    fOutputManager.FillNeutronTallyTreeEvent();
    fOutputManager.ClearNeutronTallyTreeEvent();
  }    

  //Data loaded into RecoilTree variables in stepping action
  if (fMessenger.GetRecoilTreeStatus()) {
    fOutputManager.FillRecoilTreeEvent();
    fOutputManager.ClearRecoilTreeEvent();
  }    
}
