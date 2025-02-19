//Detector construction class

#ifndef MiniBooNEBeamlineConstruction_h
#define MiniBooNEBeamlineConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4FieldManager.hh"
#include "globals.hh"

class HornMagneticField;

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
        virtual void ConstructSDandField();
        
    private:
        G4GenericMessenger* fMessenger;
        static G4ThreadLocal HornMagneticField* fMagneticField;
        static G4ThreadLocal G4FieldManager* fFieldMgr;
    
        G4LogicalVolume* logicAir;
};

#endif

