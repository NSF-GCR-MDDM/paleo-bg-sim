#include "PaleoSimSteppingAction.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "G4EventManager.hh"
#include "G4VProcess.hh"
#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimEventAction.hh"

PaleoSimSteppingAction::PaleoSimSteppingAction(PaleoSimMessenger& messenger, 
                                               PaleoSimOutputManager& manager)
    : fMessenger(messenger), fOutputManager(manager) 
{}

PaleoSimSteppingAction::~PaleoSimSteppingAction() {}

void PaleoSimSteppingAction::UserSteppingAction(const G4Step* step) {

            
    const G4Track* track = step->GetTrack();
    const G4StepPoint* preStepPoint = step->GetPreStepPoint();
    const G4VPhysicalVolume* preStepVolume = preStepPoint->GetPhysicalVolume();
    const G4StepPoint* postStepPoint = step->GetPostStepPoint();
    const G4VPhysicalVolume* postStepVolume = postStepPoint->GetPhysicalVolume();

    //If first track in step, reset prevVolume--this is the prev step point of the previous step
    if (track->GetCurrentStepNumber() == 1) {
        prevVolume = nullptr;
    }

    G4ParticleDefinition* particleDef = track->GetDefinition(); //incident particle

    auto* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    G4int eventID = event->GetEventID();

    auto* info = dynamic_cast<const PaleoSimUserEventInformation*>(event->GetUserInformation());

    //////////////
    // MIN TREE //
    //////////////
    if (fMessenger.GetMINTreeStatus()) {
        if ((fMessenger.GetSourceType()=="meiHimeMuonGenerator") || (fMessenger.GetSourceType()=="muteGenerator") || (fMessenger.GetSourceType()=="CRYGenerator")) { 
            G4int trackID = track->GetTrackID();
            G4int parentID = track->GetParentID();
            G4int particlePDG = particleDef->GetPDGEncoding();

            //Add event ID
            fOutputManager.PushMINEventID(eventID);

            // Check for secondary neutrons and tally zenith angle (in radians) and
            // increment multiplicity.
            auto secondaries = step->GetSecondaryInCurrentStep();
            for (const auto& sec : *secondaries) {
                G4int secPDGCode = sec->GetParticleDefinition()->GetPDGEncoding();
                if (secPDGCode == 2112) {
                    // Zenith angle:
                    G4ThreeVector muDir = info->primaryDirection;
                    G4ThreeVector neutronDir = sec->GetMomentumDirection();
                    fOutputManager.PushMINEventAngleRelMuon(neutronDir.angle(muDir));

                    // Multiplicity:
                    fOutputManager.IncrementMINEventMultiplicity();

                    // Energy:
                    G4double MINKinE = sec->GetKineticEnergy();
                    fOutputManager.PushMINEventEnergy(MINKinE);

                    // Distance from muon track:
                    G4ThreeVector muPos = info->primaryGenerationPosition;
                    G4ThreeVector MINPos = sec->GetPosition();
                    G4ThreeVector MINDisplacement = MINPos - muPos;
                    G4double MINDistToTrack = (MINDisplacement.cross(muDir)).mag();
                    fOutputManager.PushMINEventDistanceToMuonTrack(MINDistToTrack);
                }
            }
        }
    }

    ////////////////////////
    // NEUTRON TALLY TREE //
    ////////////////////////
    //Updated based on https://geant4.web.cern.ch/documentation/dev/faq_html/FAQ/tracksAndSteps.html
    //
    if (fMessenger.GetNeutronTallyTreeStatus()) {
        // Check if the particle is a neutron
        if (particleDef->GetPDGEncoding() == 2112) {

            if (!preStepVolume || !postStepVolume) return;  // Safety check for particles leaving the world volume

            //This means particle just entered a cell
            if (preStepPoint->GetStepStatus() == fGeomBoundary) {

                //Get the name of the current volume
                const G4TouchableHandle& preStepTouch = preStepPoint->GetTouchableHandle();
                const G4VPhysicalVolume* volume = preStepTouch->GetVolume();
                const G4String& name = volume->GetName();

                //Get our list of volumes to check if we should track this particle
                const auto& tallyVolumes = fMessenger.GetNeutronTallyTreeVolumes();

                //See if we should track this particle
                if (std::find(tallyVolumes.begin(), tallyVolumes.end(), name) != tallyVolumes.end()) {

                    //If so, get the volume number, and push that to the tree
                    PaleoSimVolumeDefinition* vol = fMessenger.GetVolumeByName(name);
                    fOutputManager.PushNeutronTallyVolumeNumber(vol->volumeNumber);

                    //Push the event ID 
                    fOutputManager.PushNeutronTallyEventID(eventID);

                    G4double energy = preStepPoint->GetKineticEnergy();
                    fOutputManager.PushNeutronTallyEventEntryEnergy(energy);

                    G4ThreeVector position = preStepPoint->GetPosition();
                    fOutputManager.PushNeutronTallyEventEntryX(position.x());
                    fOutputManager.PushNeutronTallyEventEntryY(position.y());
                    fOutputManager.PushNeutronTallyEventEntryZ(position.z());

                    G4ThreeVector momentumDirection = preStepPoint->GetMomentumDirection();
                    fOutputManager.PushNeutronTallyEventEntryU(momentumDirection.x());
                    fOutputManager.PushNeutronTallyEventEntryV(momentumDirection.y());
                    fOutputManager.PushNeutronTallyEventEntryW(momentumDirection.z());

                    if (prevVolume) {
                        const G4String& prevName = prevVolume->GetName();
                        //TODO check not null
                        PaleoSimVolumeDefinition* prevVol = fMessenger.GetVolumeByName(prevName);
                        fOutputManager.PushPrevNeutronTallyVolumeNumber(prevVol->volumeNumber);
                    }
                    else {
                        fOutputManager.PushPrevNeutronTallyVolumeNumber(-1); //To avoid de-syncing
                    }

                    // Zenith angle:
                    if ((fMessenger.GetSourceType()=="meiHimeMuonGenerator") || (fMessenger.GetSourceType()=="muteGenerator") || (fMessenger.GetSourceType()=="CRYGenerator")) { 
                        const G4ThreeVector& muDir = info->primaryDirection;
                        const G4ThreeVector& neutronDir = preStepPoint->GetMomentumDirection();
                        fOutputManager.PushNeutronTallyEventAngleRelMuon(neutronDir.angle(muDir));

                        // Distance from muon track:
                        const G4ThreeVector& muPos = info->primaryGenerationPosition;
                        const G4ThreeVector& displacement = position - muPos;
                        const G4double& distToTrack = (displacement.cross(muDir)).mag();
                        fOutputManager.PushNeutronTallyEventDistanceToMuonTrack(distToTrack);
                    }
                    fOutputManager.IncrementNeutronTallyEventMultiplicity();
                }
            }
        }
    }

    /////////////////
    // RECOIL TREE //
    /////////////////
    if (fMessenger.GetRecoilTreeStatus()) {

        if (!postStepVolume) return;

        G4String postStepVolumeName = postStepVolume->GetName();
        const auto& recoilVolumes = fMessenger.GetRecoilTreeVolumes();

        if (std::find(recoilVolumes.begin(), recoilVolumes.end(), postStepVolumeName) != recoilVolumes.end()) {
            
            /////////////////
            //Deal with ERs//
            /////////////////
            G4double emEnergy = step->GetTotalEnergyDeposit();
            G4int pdgCode = track->GetParticleDefinition()->GetPDGEncoding();
            if ((emEnergy > 0) && (std::abs(pdgCode) <= 2112)) {
                const G4ThreeVector& position  = 0.5*(preStepPoint->GetPosition() + postStepPoint->GetPosition());
                G4double ERTime = preStepPoint->GetGlobalTime();
                const G4ThreeVector& momentumDirection = track->GetMomentumDirection();
        
                PaleoSimVolumeDefinition* vol = fMessenger.GetVolumeByName(postStepVolumeName);
                fOutputManager.PushRecoilVolumeNumber(vol->volumeNumber);

                fOutputManager.PushRecoilEventID(eventID);
                fOutputManager.PushRecoilEventPDG(11);
                fOutputManager.PushRecoilEventParentPDG(pdgCode);
                fOutputManager.PushRecoilEventEnergy(emEnergy);
                fOutputManager.PushRecoilEventTime(ERTime);
                fOutputManager.PushRecoilEventCode(-1); //TODO
                fOutputManager.PushRecoilEventX(position.x());
                fOutputManager.PushRecoilEventY(position.y());
                fOutputManager.PushRecoilEventZ(position.z());
                fOutputManager.PushRecoilEventU(momentumDirection.x());
                fOutputManager.PushRecoilEventV(momentumDirection.y());
                fOutputManager.PushRecoilEventW(momentumDirection.z());
                fOutputManager.IncrementNRecoils();
            }

            /////////////////
            //Deal with NRs//
            /////////////////
            //NOTE: WARNING: NR recoil is KE of NR, NOT DEPOSITED. These are different if the ion ranges out, but we 
            //are using TRIM for actual tracks and handling ranging out there.
            auto secondaries = step->GetSecondaryInCurrentStep();
            if (!(secondaries && !secondaries->empty())) return;

            // Now record all secondaries
            for (const auto& sec : *secondaries) {

                G4int parentPdg = track->GetParticleDefinition()->GetPDGEncoding();
                G4int pdgCode = sec->GetParticleDefinition()->GetPDGEncoding(); //NR target nucleus

                if (std::abs(pdgCode) <= 2112) continue; //We only record secondaries whose PDG is greater than a neutron, i.e. Nuclear Recoils
                
                const G4ThreeVector& position = sec->GetPosition();
                const G4ThreeVector& momentumDirection = sec->GetMomentumDirection();
                G4double energy = sec->GetKineticEnergy();
                G4double NRTime = sec->GetGlobalTime();
        
                const G4VProcess* proc = sec->GetCreatorProcess();
                G4int mtCode = -1;
                if (proc) {
                    G4int type = proc->GetProcessType();
                    G4int subtype = proc->GetProcessSubType();
                    mtCode = MapProcessToMT(type, subtype);
                }
                
                PaleoSimVolumeDefinition* vol = fMessenger.GetVolumeByName(postStepVolumeName);
                fOutputManager.PushRecoilVolumeNumber(vol->volumeNumber);

                fOutputManager.PushRecoilEventID(eventID);
                fOutputManager.PushRecoilEventPDG(pdgCode);
                fOutputManager.PushRecoilEventParentPDG(parentPdg);
                fOutputManager.PushRecoilEventEnergy(energy);
                fOutputManager.PushRecoilEventTime(NRTime);
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

    prevVolume = preStepPoint->GetPhysicalVolume();
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
