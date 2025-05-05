#ifndef PaleoSimDetectorConstruction_h
#define PaleoSimDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "PaleoSimMessenger.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

class PaleoSimDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        PaleoSimDetectorConstruction(PaleoSimMessenger& messenger);
        virtual ~PaleoSimDetectorConstruction() override;

        virtual G4VPhysicalVolume* Construct();

    private:
        PaleoSimMessenger& fMessenger;
        std::map<std::string, G4LogicalVolume*> fLogicalVolumes;
        //G4RotationMatrix RotationFromCylinderAxis(const G4ThreeVector& axis);
};

#endif