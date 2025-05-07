#include "PaleoSimSteppingAction.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "G4EventManager.hh"
#include "G4VProcess.hh"
#include "PaleoSimUserEventInformation.hh"
#include "MiniBooNEBeamlineEventAction.hh"

PaleoSimSteppingAction::PaleoSimSteppingAction(PaleoSimMessenger& messenger, 
                                               PaleoSimOutputManager& manager)
    : fMessenger(messenger), fOutputManager(manager) 
{}

PaleoSimSteppingAction::~PaleoSimSteppingAction() {}

void PaleoSimSteppingAction::UserSteppingAction(const G4Step* step) {

    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particleDef = track->GetDefinition();
    auto* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();

    /////////////
    //MIN TREE //
    /////////////

    if (fMessenger.GetMINTreeStatus()) {
        G4int trackID = track->GetTrackID();
        G4int parentID = track->GetParentID();
        G4int particlePDG = particleDef->GetPDGEncoding();
    
        // For primary muons
        if (particlePDG == 13 || particlePDG == -13) {
            G4int eventID = event->GetEventID();
            fOutputManager.PushMINEventID(eventID);

            // Check for secondary neutrons and tally zenith angle (in radians) and
			// increment multiplicity.
            auto secondaries = step->GetSecondaryInCurrentStep();
            for (const auto& sec : *secondaries) {
                G4int secPDGCode = sec->GetParticleDefinition()->GetPDGEncoding();
                if (secPDGCode == 2112) {
					// Zenith angle:
                    G4ThreeVector muDir = track->GetMomentumDirection();
                    G4ThreeVector neutronDir = sec->GetMomentumDirection();
                    fOutputManager.PushMINEventAngleRelMuon(neutronDir.angle(muDir));

					// Multiplicity:
					fOutputManager.IncrementMINEventMultiplicity();

					// Energy:
					G4double MINKinE = sec->GetKineticEnergy();
					fOutputManager.PushMINEventEnergy(MINKinE);
                }
            }
        }
    }

    //////////////////////
    //NEUTRON TALLY TREE//
    //////////////////////

    if (fMessenger.GetNeutronTallyTreeStatus()) {
        // Check if the particle is a neutron
        if (particleDef->GetPDGEncoding() == 2112) {
            G4StepPoint* preStepPoint = step->GetPreStepPoint();
            G4StepPoint* postStepPoint = step->GetPostStepPoint();

            G4VPhysicalVolume* preVol = preStepPoint->GetPhysicalVolume();
            G4VPhysicalVolume* postVol = postStepPoint->GetPhysicalVolume();
        
            if (!preVol || !postVol) return;  // Safety check for particles leaving the world volume

            if ((preVol != postVol)
				&& (postVol->GetName() == fMessenger.GetNeutronTallyTreeVolume())) { 
                G4int eventID = event->GetEventID();
                fOutputManager.PushNeutronTallyEventID(eventID);

                G4double energy = track->GetKineticEnergy();
                fOutputManager.PushNeutronTallyEventEntryEnergy(energy);

                G4ThreeVector position = track->GetPosition();
                fOutputManager.PushNeutronTallyEventEntryX(position.x());
                fOutputManager.PushNeutronTallyEventEntryY(position.y());
                fOutputManager.PushNeutronTallyEventEntryZ(position.z());

                G4ThreeVector momentumDirection = track->GetMomentumDirection();
                fOutputManager.PushNeutronTallyEventEntryU(momentumDirection.x());
                fOutputManager.PushNeutronTallyEventEntryV(momentumDirection.y());
                fOutputManager.PushNeutronTallyEventEntryW(momentumDirection.z());

                fOutputManager.IncrementNeutronTallyEventMultiplicity();
            }
        }
    }

    ////////////////
    // RECOIL TREE//
    ////////////////

    if (fMessenger.GetRecoilTreeStatus()) {

        G4StepPoint* stepPoint = step->GetPreStepPoint();
        G4VPhysicalVolume* stepVolume = stepPoint->GetPhysicalVolume();
        if (!stepVolume) return;
    
        if (stepVolume->GetName() == fMessenger.GetRecoilTreeVolume())  {
            auto secondaries = step->GetSecondaryInCurrentStep();
            if (secondaries->empty()) return;
    
            // Check that the parent is a neutron or gamma
            const G4Track* parentTrack = step->GetTrack();
            G4int parentPdg = parentTrack->GetParticleDefinition()->GetPDGEncoding();
            if (!(parentPdg == 2112 || parentPdg == 22 || parentPdg == 13 || parentPdg == -13)) return;
                
            auto* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
            G4int eventID = event->GetEventID();
            
            // Now record all secondaries
            for (const auto& sec : *secondaries) {

                G4int pdgCode = sec->GetParticleDefinition()->GetPDGEncoding();
                //Ignore gamma, electron, and neutron, 
                if (std::abs(pdgCode) == 11 || pdgCode == 22 || pdgCode == 2112 ) continue;

                G4ThreeVector position = sec->GetPosition();
                G4ThreeVector momentumDirection = sec->GetMomentumDirection();
                G4double energy = sec->GetKineticEnergy();
                G4double secTime = sec->GetGlobalTime();
        
                const G4VProcess* proc = sec->GetCreatorProcess();
                G4int mtCode = -1;
                if (proc) {
                    G4int type = proc->GetProcessType();
                    G4int subtype = proc->GetProcessSubType();
                    mtCode = MapProcessToMT(type, subtype);
                }
                
                fOutputManager.PushRecoilEventID(eventID);
                fOutputManager.PushRecoilEventPDG(pdgCode);
                fOutputManager.PushRecoilEventParentPDG(parentPdg);
                fOutputManager.PushRecoilEventEnergy(energy);
                fOutputManager.PushRecoilEventTime(secTime);
                fOutputManager.PushRecoilEventCode(mtCode);
                fOutputManager.PushRecoilEventX(position.x());
                fOutputManager.PushRecoilEventY(position.y());
                fOutputManager.PushRecoilEventZ(position.z());
                fOutputManager.PushRecoilEventU(momentumDirection.x());
                fOutputManager.PushRecoilEventV(momentumDirection.y());
                fOutputManager.PushRecoilEventW(momentumDirection.z());
                fOutputManager.IncrementNRecoils();
            }
        }
    }
}

//Maps G4 processes to MT codes. Needs checking
G4int PaleoSimSteppingAction::MapProcessToMT(G4int processType, G4int processSubType)
{
    if (processType == 1) { // Transportation
        return -1;
    }
    else if (processType == 2) { // Electromagnetic
        if (processSubType == 2) return 501;
        if (processSubType == 3) return 5;
        if (processSubType == 4) return 502;
        if (processSubType == 5) return 504;
        if (processSubType == 10) return 601;
    }
    else if (processType == 4) { // Hadronic
        if (processSubType == 111) return 2;
        if (processSubType == 121) return 4;
        if (processSubType == 131) return 102;
        if (processSubType == 151) return 18;
    }
    else if (processType == 6) { // Decay
        return 0;
    }

    return -1;
}
