#ifndef PALEO_SIM_VOLUME_DEFINITION_HH
#define PALEO_SIM_VOLUME_DEFINITION_HH

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Types.hh"
#include "G4Exception.hh"
#include <string>
#include <utility>

struct PaleoSimVolumeDefinition {
    //Defining volume properties
    G4String name = "";
    std::string parentName = "None";
    std::string shape = "";
    std::string materialName = "";

    // Placement/orientation
    //User passess in a position, and whether it is absolute or relative.
    //If absolute, we will translate it to relative in our code
    G4ThreeVector userPosition = G4ThreeVector(0, 0, 0); //Stores what the user inputs
    G4ThreeVector absolutePosition = G4ThreeVector(0, 0, 0);       // center in world
    G4ThreeVector relativePosition = G4ThreeVector(0, 0, 0);       // center in parent
    std::string positionType = "relative"; 
    //We don't allow for arbitrary rotations. However, cylinders in Geant4 are all 
    //instantiated oriented along the z-axis. We track rotation of subvolumes inside
    //cylinders which will have to have the cylinders rotation taken into account
    G4RotationMatrix rotationMatrix; // Starts as identity
    G4RotationMatrix cumulativeRotationMatrix; // Starts as identity

    // Shape-specific geometry
    G4ThreeVector halfLengths = G4ThreeVector(1,1,1);             // for boxes only
    double radius = 1.;                                           // for cylinders and spheres
    double halfHeight = 1.;                                       // for cylinders only
    G4ThreeVector absoluteCylinderAxis = G4ThreeVector(0,0,1);    // for cylinders only

    //For referencing instantiated geometry
    G4LogicalVolume* logical = nullptr; 
    G4VSolid* solid = nullptr;

    // Visualization (optional)
    G4ThreeVector rgb = G4ThreeVector(1.,1.,1.);
    double alpha = 1.; 

    // Return global (world-frame) bounding box min and max
    std::pair<G4ThreeVector,G4ThreeVector> GetGlobalBounds() const {
        if (!solid) {
            G4Exception("PaleoSimVolumeDefinition::GetGlobalBounds", "InvalidSolid", FatalException,
                        ("Solid not set for:" + name).c_str());
        }
        G4ThreeVector min, max;
        solid->BoundingLimits(min, max);
        return std::make_pair(cumulativeRotationMatrix * min + absolutePosition,cumulativeRotationMatrix * max + absolutePosition);
    }

    // Check if a global point is inside the volume
    bool CheckPointInside(const G4ThreeVector& point) const {
        if (!solid) {
            G4Exception("PaleoSimVolumeDefinition::CheckPointInside", "InvalidSolid", FatalException,
                        ("Solid not set for:" + name).c_str());
        }
        G4ThreeVector local = point - absolutePosition;
        local = cumulativeRotationMatrix.inverse() * local;
        return (solid->Inside(local) == kInside);
    }
};
#endif

