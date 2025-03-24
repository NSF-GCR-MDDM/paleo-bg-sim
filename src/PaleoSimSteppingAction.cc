#include "PaleoSimSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "G4EventManager.hh"

PaleoSimSteppingAction::PaleoSimSteppingAction(PaleoSimMessenger& messenger, 
                                               PaleoSimOutputManager& manager)
    : fMessenger(messenger), fOutputManager(manager) 
{}

PaleoSimSteppingAction::~PaleoSimSteppingAction() {}

void PaleoSimSteppingAction::UserSteppingAction(const G4Step* step) {
    // Access the track data
    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particleDef = track->GetDefinition();

    // Check if the particle is a neutron
    if (particleDef->GetPDGEncoding() == 2112) {

        //Get position
        G4ThreeVector position = track->GetPosition();

        //Get air cavity side length
        G4double airCavityHalfLength = fMessenger.GetAirCavitySideLength()/2.;

        //Check if it's inside the air cavity
        if ((position.x() >= -airCavityHalfLength) && (position.x() <= airCavityHalfLength) &&
            (position.y() >= -airCavityHalfLength) && (position.y() <= airCavityHalfLength) &&
            (position.z() >= -airCavityHalfLength) && (position.z() <= airCavityHalfLength)) {
                        
            G4cout<<"PEOPLE, WE FOUND A NEUTRON IN THE CAVITY"<<G4endl;
            auto* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
            if (event) {
                G4int eventID = event->GetEventID();
                fOutputManager.PushNeutronTallyEventID(eventID);
            }

            double energy = track->GetKineticEnergy();
            fOutputManager.PushNeutronTallyEventEntryEnergy(energy);

            fOutputManager.PushNeutronTallyEventEntryX(position.x());
            fOutputManager.PushNeutronTallyEventEntryY(position.y());
            fOutputManager.PushNeutronTallyEventEntryZ(position.z());

            //TODO:
            //G4ThreeVector momentum = track->GetMomentum();
            //fNeutronMomenta.push_back(momentum);
            //Distance to muon
            //Zenith
        }
    }
}
