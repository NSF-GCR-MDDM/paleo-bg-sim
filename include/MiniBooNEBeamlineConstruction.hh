//Detector construction class

#ifndef MiniBooNEBeamlineConstruction_h
#define MiniBooNEBeamlineConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "PaleoSimMaterialManager.hh"
#include "PaleoSimMessenger.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

/// Detector construction class to define materials and geometry.
class MiniBooNEBeamlineConstruction : public G4VUserDetectorConstruction
{
    public:
        //Constructor/destructor
        MiniBooNEBeamlineConstruction(PaleoSimMessenger& messenger);
        virtual ~MiniBooNEBeamlineConstruction() override;

        //Required, ran by RunManager->Initialization
        virtual G4VPhysicalVolume* Construct();
        
    private:
        PaleoSimMaterialManager* fMaterialManager;
        PaleoSimMessenger& fMessenger;
    
};

#endif

