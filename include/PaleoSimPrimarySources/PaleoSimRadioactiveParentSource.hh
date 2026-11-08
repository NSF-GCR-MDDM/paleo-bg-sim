#ifndef PaleoSimRadioactiveParentSource_h
#define PaleoSimRadioactiveParentSource_h 1

#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"
#include "PaleoSimVolumeDefinition.hh"

class G4Event;
class G4ParticleDefinition;

class PaleoSimRadioactiveParentSource : public PaleoSimPrimarySource {
public:
    PaleoSimRadioactiveParentSource(PaleoSimMessenger& messenger);
    virtual ~PaleoSimRadioactiveParentSource();

    virtual void InitializeSource() override;
    virtual void GeneratePrimaries(G4Event* anEvent) override;

private:
    PaleoSimVolumeDefinition* fSourceVolumeDefinition = nullptr;
    G4ParticleDefinition* fParentIon = nullptr;
};

#endif