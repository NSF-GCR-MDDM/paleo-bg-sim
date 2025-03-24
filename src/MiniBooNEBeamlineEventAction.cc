#include "PaleoSimOutputManager.hh"
#include "G4Event.hh" 
#include "MiniBooNEBeamlineEventAction.hh"

MiniBooNEBeamlineEventAction::MiniBooNEBeamlineEventAction(PaleoSimOutputManager& manager)
: G4UserEventAction(), fOutputManager(manager) {}

MiniBooNEBeamlineEventAction::~MiniBooNEBeamlineEventAction() {}

void MiniBooNEBeamlineEventAction::BeginOfEventAction(const G4Event* event) {    
    if (event->GetEventID() % 100 == 0) {
        G4cout << "Event # " << event->GetEventID() << G4endl;
    }

    //Clear vectors at start of event
    if (fOutputManager.GetPrimariesTreeOutputStatus()) {
      fOutputManager.ClearPrimariesTreeEvent();
    }

    if (fOutputManager.GetNeutronTallyTreeOutputStatus()) {
      fOutputManager.ClearNeutronTallyTreeEvent();
    }
}

void MiniBooNEBeamlineEventAction::EndOfEventAction(const G4Event* event) {

  //Fill trees at end of event
  //Data loaded into Primaries tree variables in generator
  if (fOutputManager.GetPrimariesTreeOutputStatus()) {
    fOutputManager.FillPrimariesTreeEvent();
  }

  //Data loaded into NeutronTallyTree variables in stepping action
  if (fOutputManager.GetNeutronTallyTreeOutputStatus()) {
    fOutputManager.FillNeutronTallyTreeEvent();
  }
}
