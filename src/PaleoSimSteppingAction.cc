#include "PaleoSimSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"

PaleoSimSteppingAction::PaleoSimSteppingAction(MiniBooNEBeamlineConstruction* detector, 
                                              MiniBooNEBeamlinePrimaryGeneratorAction* generator)
    : fDetector(detector), fGenerator(generator) 
{}

PaleoSimSteppingAction::~PaleoSimSteppingAction() {}

//STILL NEED TO FIX LOGIC TO ONLY TRACK FIRST NEUTRON ENTERING THE CAVITY
void PaleoSimSteppingAction::UserSteppingAction(const G4Step* step) {
    // Access the track data
    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particleDef = track->GetDefinition();

    // Check if the particle is a neutron
    if (particleDef->GetParticleName() == "neutron") {

        //Get position
        G4ThreeVector position = track->GetPosition();
        //Check if it's inside the air cavity
        G4double airCavityLength = fDetector->GetAirCavitySideLength();
        if ((position.x() >= -airCavityLength / 2.) && (position.x() <= airCavityLength / 2.) &&
            (position.y() >= -airCavityLength / 2.) && (position.y() <= airCavityLength / 2.) &&
            (position.z() >= -airCavityLength / 2.) && (position.z() <= airCavityLength / 2.)) {
                        
            G4cout<<"PEOPLE, WE FOUND A NEUTRON IN THE CAVITY"<<G4endl;
            G4ThreeVector momentum = track->GetMomentum();
            double energy = track->GetKineticEnergy();

            fNeutronEnergies.push_back(energy);
            fNeutronPositionsX.push_back(position.x());
            fNeutronPositionsY.push_back(position.y());
            fNeutronPositionsZ.push_back(position.z());

            //TODO:
            //fNeutronMomenta.push_back(momentum);
            //Distance to muon
            //Zenoth
        }
    }
}
