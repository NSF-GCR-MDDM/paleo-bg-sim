#include "PaleoSimSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "MiniBooNEBeamlineEventAction.hh"
#include "G4Neutron.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"

PaleoSimSteppingAction::PaleoSimSteppingAction(PaleoSimMessenger& messenger, 
                                               PaleoSimOutputManager& manager)
    : fMessenger(messenger), fOutputManager(manager) 
{}

PaleoSimSteppingAction::~PaleoSimSteppingAction() {}

void PaleoSimSteppingAction::UserSteppingAction(const G4Step* step) {

    // Access the track data
    G4Track* track = step->GetTrack();
	G4int trackID = track->GetTrackID();
  	G4int parentID = track->GetParentID();
    G4ParticleDefinition* particleDef = track->GetDefinition();

	MiniBooNEBeamlineEventAction* eventAction =
		static_cast<MiniBooNEBeamlineEventAction*>(
			const_cast<G4UserEventAction*>(
				G4RunManager::GetRunManager()->GetUserEventAction()));
	
	if (track->GetDefinition() == G4MuonMinus::Definition()
		|| track->GetDefinition() == G4MuonPlus::Definition()) {
    	eventAction->AddMuon(trackID, track->GetMomentumDirection());
  	}

    // Check if the particle is a neutron
    if (particleDef->GetPDGEncoding() == 2112) {
        G4StepPoint* preStepPoint = step->GetPreStepPoint();
        G4StepPoint* postStepPoint = step->GetPostStepPoint();

        G4VPhysicalVolume* preVol = preStepPoint->GetPhysicalVolume();
        G4VPhysicalVolume* postVol = postStepPoint->GetPhysicalVolume();
    
        if (!preVol || !postVol) return;  // Safety check for particles leaving the world volume

        if ((preVol != postVol) && (postVol->GetName() == "AirCavity")) {  // Use the actual name you gave the cavity
            auto* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();

            G4int eventID = event->GetEventID();
            fOutputManager.PushNeutronTallyEventID(eventID);

            double energy = track->GetKineticEnergy();
            fOutputManager.PushNeutronTallyEventEntryEnergy(energy);

            G4ThreeVector position = track->GetPosition();
            fOutputManager.PushNeutronTallyEventEntryX(position.x());
            fOutputManager.PushNeutronTallyEventEntryY(position.y());
            fOutputManager.PushNeutronTallyEventEntryZ(position.z());

            G4ThreeVector momentum = track->GetMomentum();
            fOutputManager.PushNeutronTallyEventEntryPx(momentum.x());
            fOutputManager.PushNeutronTallyEventEntryPy(momentum.y());
            fOutputManager.PushNeutronTallyEventEntryPz(momentum.z());

            //Creation energy
            fOutputManager.PushNeutronTallyEventCreationEnergy(track->GetVertexKineticEnergy());
            
            //Distance to generation point
            G4ThreeVector creationPos = track->GetVertexPosition();
            double distance = (position - creationPos).mag();
            fOutputManager.PushNeutronTallyEventDistanceToVertex(distance);

            //Zenith angle
            double zenithAngle = momentum.angle(G4ThreeVector(0, 0, 1));
            fOutputManager.PushNeutronTallyEventEntryTheta(zenithAngle);

            if (track->GetDefinition() == G4Neutron::Definition() && parentID > 0) {
				if (eventAction->IsMuon(parentID)) {
				  G4ThreeVector muonDir = eventAction->GetMuonDirection(parentID);
				  G4ThreeVector neutronDir = track->GetMomentumDirection();
			
				  G4double angle = neutronDir.angle(muonDir); // in radians
				  fOutputManager.PushNeutronTallyAngleRelMuon(angle);
				}
			}
        }
    }
}
