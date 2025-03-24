#include "MiniBooNEBeamlineEventAction.hh"
#include "MiniBooNEBeamlineRunAction.hh"
#include "MiniBooNEBeamlineAnalysis.hh"
#include "PaleoSimSteppingAction.hh"
#include "PaleoSimOutputManager.hh"
#include "G4Event.hh" 

MiniBooNEBeamlineEventAction::MiniBooNEBeamlineEventAction(MiniBooNEBeamlineRunAction* runAction, 
    MiniBooNEBeamlinePrimaryGeneratorAction* generator, 
    PaleoSimSteppingAction* steppingAction)
: G4UserEventAction(), fRunAction(runAction), fGenerator(generator), fSteppingAction(steppingAction) {}

MiniBooNEBeamlineEventAction::~MiniBooNEBeamlineEventAction() {}

void MiniBooNEBeamlineEventAction::BeginOfEventAction(const G4Event* event) {    
    if (event->GetEventID() % 100 == 0) {
        std::cout << "Event # " << event->GetEventID() << std::endl;
    }

    // Clear vectors of primaries tree for this event
    if (PaleoSimOutputManager::Get().IsPrimariesTreeEnabled()) {
        fGenerator->fPrimaryPDGIDs.clear();
        fGenerator->fPrimaryEnergies.clear();
        fGenerator->fPrimary_x.clear(); 
        fGenerator->fPrimary_y.clear(); 
        fGenerator->fPrimary_z.clear();
        fGenerator->fPrimary_px.clear(); 
        fGenerator->fPrimary_py.clear(); 
        fGenerator->fPrimary_pz.clear();
        // CUSTOM_GENERATOR_HOOK
        // Clear any custom output vectors here
        //
        //Mei & Hime muon generator
        fGenerator->fMuonTheta.clear();
        fGenerator->fMuonPhi.clear();
        fGenerator->fMuonSlant.clear();
    }

    //Clear neutron tallys
    if (PaleoSimOutputManager::Get().IsNeutronTallyTreeEnabled()) {
        fSteppingAction->fNeutronEnergies.clear();
        fSteppingAction->fNeutronPositionsX.clear();
        fSteppingAction->fNeutronPositionsY.clear();
        fSteppingAction->fNeutronPositionsZ.clear();
    }
}

void MiniBooNEBeamlineEventAction::EndOfEventAction(const G4Event* event) {

    //Fill primaries tree
    if (PaleoSimOutputManager::Get().IsPrimariesTreeEnabled()) {
        PaleoSimOutputManager::Get().FillPrimaryEvent(
            event->GetEventID(),
            fGenerator->fPrimaryPDGIDs, 
            fGenerator->fPrimaryEnergies, 
            fGenerator->fPrimary_x, 
            fGenerator->fPrimary_y, 
            fGenerator->fPrimary_z, 
            fGenerator->fPrimary_px, 
            fGenerator->fPrimary_py, 
            fGenerator->fPrimary_pz,
            // CUSTOM_GENERATOR_HOOK
            // Clear any custom output vectors here
            //
            //Mei & Hime muon generator
            fGenerator->fMuonTheta, 
            fGenerator->fMuonPhi, 
            fGenerator->fMuonSlant
        );
    }

    //Fill NeutronTallyTree
    if (PaleoSimOutputManager::Get().IsNeutronTallyTreeEnabled()) {

        if (!fSteppingAction->fNeutronEnergies.empty()) {
            PaleoSimOutputManager::Get().FillNeutronTallyEvent(
                event->GetEventID(),
                fSteppingAction->fNeutronEnergies,
                fSteppingAction->fNeutronPositionsX,
                fSteppingAction->fNeutronPositionsY,
                fSteppingAction->fNeutronPositionsZ
            );
        }
    }
}
