//Detector construction class

#ifndef MiniBooNEBeamlineConstruction_h
#define MiniBooNEBeamlineConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4FieldManager.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "PaleoSimMaterialManager.hh"

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


        // Setters 
        inline void SetOverburdenSideLength(G4double val) { fOverburdenSideLength = val; }
        inline void SetOverburdenMaterial(G4String material) { fOverburdenMaterial = material; }
        inline void SetAirCavitySideLength(G4double val) { fAirCavitySideLength = val; }
        inline void SetTargetSideLength(G4double val) { fTargetSideLength = val; }
        inline void SetTargetMaterial(G4String material) { fTargetMaterial = material; }
        
        // Getter (inline)
        inline G4double GetAirCavitySideLength() const { return fAirCavitySideLength; }
        inline G4double GetOverburdenSideLength() const {return fOverburdenSideLength;}
        inline G4double GetTargetSideLength() const { return fTargetSideLength; }

        
    private:
        PaleoSimMaterialManager* materials;
        
        G4GenericMessenger* fMessenger;
        G4double fOverburdenSideLength = 10.0 * m;
        G4double fAirCavitySideLength = 0.0 * m;
        G4double fTargetSideLength = 0.0 * cm;
        G4String fTargetMaterial = "Norite";   
        G4String fOverburdenMaterial = "Norite";   
    
};

#endif

