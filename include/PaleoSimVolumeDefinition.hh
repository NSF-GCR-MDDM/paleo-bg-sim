#ifndef PALEO_SIM_VOLUME_DEFINITION_HH
#define PALEO_SIM_VOLUME_DEFINITION_HH

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include <string>

struct VolumeDefinition {
    //Defining volume properties
    std::string name = "";
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

    // Visualization (optional)
    G4ThreeVector rgb = G4ThreeVector(1.,1.,1.);
    double alpha = 1.; 
};

#endif

