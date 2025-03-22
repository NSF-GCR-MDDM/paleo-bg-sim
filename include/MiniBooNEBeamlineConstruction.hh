//Detector construction class

#ifndef MiniBooNEBeamlineConstruction_h
#define MiniBooNEBeamlineConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4FieldManager.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4GenericMessenger;

/// Detector construction class to define materials and geometry.
class MiniBooNEBeamlineConstruction : public G4VUserDetectorConstruction
{
    public:
        MiniBooNEBeamlineConstruction();
        virtual ~MiniBooNEBeamlineConstruction();

        virtual G4VPhysicalVolume* Construct();
        void SetOverburdenSideLength(G4double val);
        void SetAirCavitySideLength(G4double val);
        void SetTargetSideLength(G4double val);
        void SetTargetMaterial(const G4String& val);
        
    private:
        G4GenericMessenger* fMessenger;
        G4double fOverburdenSideLength = 10.0 * m;
        G4double fAirCavitySideLength = 0.0 * m;
        G4double fTargetSideLength = 1.0 * cm;
        G4String fTargetMaterial = "Norite";   
    
};

#endif

