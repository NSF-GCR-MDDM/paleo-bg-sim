#ifndef PALEOSIMSTEPPINGACTION_HH
#define PALEOSIMSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleDefinition.hh"

#include "MiniBooNEBeamlineConstruction.hh" 
#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh" 

class MiniBooNEBeamlineEventAction;

class PaleoSimSteppingAction : public G4UserSteppingAction {
public:
    PaleoSimSteppingAction(MiniBooNEBeamlineConstruction* detector, 
                           MiniBooNEBeamlinePrimaryGeneratorAction* generator);
    virtual ~PaleoSimSteppingAction();

    virtual void UserSteppingAction(const G4Step* step) override;

private:
    friend class MiniBooNEBeamlineEventAction;  //For giving access to these in the EventAction class for filling/clearing

    MiniBooNEBeamlineConstruction* fDetector;
    MiniBooNEBeamlinePrimaryGeneratorAction* fGenerator;

    //For tracking neutrons entering the air cavity
    std::vector<double> fNeutronEnergies;
    std::vector<double> fNeutronPositionsX;
    std::vector<double> fNeutronPositionsY;
    std::vector<double> fNeutronPositionsZ;
};

#endif