#ifndef PALEO_SIM_GEOMETRY_MESSENGER_HH
#define PALEO_SIM_GEOMETRY_MESSENGER_HH

#include "PaleoSimMessenger.hh"
#include "PaleoSimVolumeDefinition.hh"

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include <string>
#include <vector>

class PaleoSimGeometryMessenger : public G4UImessenger {
public:
    explicit PaleoSimGeometryMessenger(PaleoSimMessenger* messenger);
    ~PaleoSimGeometryMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

    //Geometry checks
    void ValidateGeometry();
    void SortVolumesByHierarchy(); 
    void ComputeMissingCoordinates(); 
    void ComputeCumulativeRotationMatrix();
    void PopulateChildrenVector();

private:
    PaleoSimMessenger* fMessenger;                         
    PaleoSimVolumeDefinition* fCurrentVolume = nullptr;
    
    // Directories
    G4UIdirectory* fVolumeDirectory = nullptr;
    G4UIdirectory* fBoxDirectory = nullptr;
    G4UIdirectory* fCylinderDirectory = nullptr;
    G4UIdirectory* fSphereDirectory = nullptr;

    // Commands
    //Init/finalize
    G4UIcmdWithAString* fNewCmd = nullptr;
    G4UIcmdWithoutParameter* fFinalizeCmd = nullptr;
    //Properties
    G4UIcmdWithAString* fSetParentCmd = nullptr;
    G4UIcmdWithAString* fSetShapeCmd = nullptr;
    G4UIcmdWithAString* fSetMaterialCmd = nullptr;
    G4UIcmdWithAnInteger* fSetVolumeNumberCmd = nullptr; 
    //Positions/orientation
    G4UIcmdWith3VectorAndUnit* fSetPositionCmd = nullptr;
    G4UIcmdWithAString* fSetPositionTypeCmd = nullptr;
    //Geometry
    G4UIcmdWith3VectorAndUnit* fSetHalfLengthsCmd = nullptr;       // for boxes
    G4UIcmdWithADoubleAndUnit* fSetRadiusCmd = nullptr;            // for cylinders/spheres
    G4UIcmdWithADoubleAndUnit* fSetHalfHeightCmd = nullptr;        // for cylinders
    G4UIcmdWith3Vector* fSetCylinderAxisCmd = nullptr;             // for cylinders
    //Visualization only
    G4UIcmdWith3Vector* fSetRGBCmd = nullptr;     
    G4UIcmdWithADouble* fSetAlphaCmd = nullptr;
    
    // Valid shape names - These are all we are coding messenger/builders for (for now)
    std::vector<std::string> fAllowedShapes = {"box", "cylinder", "sphere"};
};

#endif
