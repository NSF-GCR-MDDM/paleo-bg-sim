#include "MiniBooNEBeamlineEventAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineAnalysis.hh"
#include "G4Event.hh" 

MiniBooNEBeamlineEventAction::MiniBooNEBeamlineEventAction(MiniBooNEBeamlineRunAction* runAction)
: G4UserEventAction(), fRunAction(runAction) {}

MiniBooNEBeamlineEventAction::~MiniBooNEBeamlineEventAction() {}

void MiniBooNEBeamlineEventAction::BeginOfEventAction(const G4Event* event) {    
    if (event->GetEventID() % 10 == 0) {
        std::cout << "Event # " << event->GetEventID() << std::endl;
    }
}

void MiniBooNEBeamlineEventAction::EndOfEventAction(const G4Event* event) {
    // No action needed for now, can be customized
}
