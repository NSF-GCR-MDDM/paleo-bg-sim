//Event action

#include "MiniBooNEBeamlineEventAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineAnalysis.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>

MiniBooNEBeamlineEventAction::MiniBooNEBeamlineEventAction(MiniBooNEBeamlineRunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction)
{} 

MiniBooNEBeamlineEventAction::~MiniBooNEBeamlineEventAction()
{}

void MiniBooNEBeamlineEventAction::BeginOfEventAction(const G4Event* event)
{    

if (event->GetEventID() % 10 == 0) {
    std::cout << "Event # " << event->GetEventID() << std::endl;
 }
    //Get the primary particle and fill the beam histograms
    G4ThreeVector vertex = event->GetPrimaryVertex()->GetPosition();
    G4PrimaryParticle* primary = event->GetPrimaryVertex()->GetPrimary();
    G4ThreeVector momentumDir = primary->GetMomentumDirection();
    G4double thetaX = asin(momentumDir.x())*rad;
    G4double thetaY = asin(momentumDir.y())*rad;

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    //Vertex units are mm by default
    analysisManager->FillH2(0, vertex.x(), vertex.y());
    //Convert to mrad for the plot
    analysisManager->FillH2(1, thetaX/mrad, thetaY/mrad);
}

void MiniBooNEBeamlineEventAction::EndOfEventAction(const G4Event*)
{   
}
