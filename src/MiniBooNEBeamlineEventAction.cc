#include "PaleoSimOutputManager.hh"
#include "G4Event.hh" 
#include "MiniBooNEBeamlineEventAction.hh"
#include "PaleoSimUserEventInformation.hh"

MiniBooNEBeamlineEventAction::MiniBooNEBeamlineEventAction(PaleoSimMessenger &messenger,PaleoSimOutputManager& manager)
: G4UserEventAction(), fMessenger(messenger), fOutputManager(manager) {}

MiniBooNEBeamlineEventAction::~MiniBooNEBeamlineEventAction() {}

void MiniBooNEBeamlineEventAction::BeginOfEventAction(const G4Event* event) {    
  if (event->GetEventID() % 100 == 0) {
      G4cout << "Event # " << event->GetEventID() << G4endl;
  }

  //For now, we assume 1 primary vertex, but that it can contain multiple particles there
  G4PrimaryVertex* vertex = event->GetPrimaryVertex(0); //1 particle
  G4ThreeVector position = vertex->GetPosition();
  G4PrimaryParticle* particle = vertex->GetPrimary();
  auto* info = dynamic_cast<PaleoSimUserEventInformation*>(event->GetUserInformation());
  //We might want to load up other stuff into user event info here just to have access to it for later
  while (particle) {
    if (info) {
      info->primaryDirection.push_back(G4ThreeVector(particle->GetPx(), particle->GetPy(), particle->GetPz()));
    }
    particle = particle->GetNext(); // Get the next primary particle at this vertex
  }

  //////////////////////////////////
  //Only if primaries tree enabled//
  //////////////////////////////////
  if (fMessenger.GetPrimariesTreeStatus()) {
      
    //Fill primaries tree
    G4int eventID = event->GetEventID();
    fOutputManager.PushPrimaryEventID(eventID);
  
    //Loop through primary particles
    particle = vertex->GetPrimary();
    while (particle) {
      G4int pdg = particle->GetPDGcode();
      G4double energy = particle->GetTotalEnergy();
      G4ThreeVector momentum(particle->GetPx(), particle->GetPy(), particle->GetPz());

      fOutputManager.PushPrimaryEventPDG(pdg);
      fOutputManager.PushPrimaryEventEnergy(energy);
      fOutputManager.PushPrimaryEventX(position.x());
      fOutputManager.PushPrimaryEventY(position.y());
      fOutputManager.PushPrimaryEventZ(position.z());
      fOutputManager.PushPrimaryEventPx(momentum.x());
      fOutputManager.PushPrimaryEventPy(momentum.y());
      fOutputManager.PushPrimaryEventPz(momentum.z());

      //CUSTOM_GENERATOR_HOOK
      //If you've set up the user information object to store quantities for each particle, load them now.
      //
      //Mei & Hime muon generator
      if ((info) && (fMessenger.GetSourceType() == "meiHimeMuonGenerator")) {
        if (!info->muonTheta.empty()) {
            fOutputManager.PushPrimaryMuonTheta(info->muonTheta.front());
            info->muonTheta.pop_front();
        }
        if (!info->muonPhi.empty()) {
            fOutputManager.PushPrimaryMuonPhi(info->muonPhi.front());
            info->muonPhi.pop_front();
        }
        if (!info->muonSlantDepth.empty()) {
            fOutputManager.PushPrimaryMuonSlant(info->muonSlantDepth.front());
            info->muonSlantDepth.pop_front();
        }
      }
      //Mute generator
      if ((info) && (fMessenger.GetSourceType() == "muteGenerator")) {
        if (!info->muonTheta.empty()) {
            fOutputManager.PushPrimaryMuonTheta(info->muonTheta.front());
            info->muonTheta.pop_front();
        }
        if (!info->muonPhi.empty()) {
            fOutputManager.PushPrimaryMuonPhi(info->muonPhi.front());
            info->muonPhi.pop_front();
        }
      }
      particle = particle->GetNext(); // Get the next primary particle at this vertex
    }
  }
}

void MiniBooNEBeamlineEventAction::EndOfEventAction(const G4Event* event) {

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
  if ( fMessenger.GetNeutronTallyTreeStatus() ) {
    fOutputManager.FillNeutronTallyTreeEvent();
    fOutputManager.ClearNeutronTallyTreeEvent();
  }    

  //Data loaded into RecoilTree variables in stepping action
  if ( fMessenger.GetRecoilTreeStatus() ) {
    fOutputManager.FillRecoilTreeEvent();
    fOutputManager.ClearRecoilTreeEvent();
  }    
}
