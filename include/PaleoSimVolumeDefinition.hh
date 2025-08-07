#ifndef PALEO_SIM_VOLUME_DEFINITION_HH
#define PALEO_SIM_VOLUME_DEFINITION_HH

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Types.hh"
#include "G4Exception.hh"
#include "Randomize.hh"   
#include "CLHEP/Units/SystemOfUnits.h" 
#include <cmath>       
#include <string>
#include <utility>
#include <vector>


struct PaleoSimVolumeDefinition {
    //Main volume properties
    std::string name = "";
    std::string parentName = "";
    std::string shape = "";
    std::string materialName = "";
    int volumeNumber = -1;
    std::vector<const PaleoSimVolumeDefinition*> children;

    // Placement/orientation properties
    //User passess in a position, and whether it is absolute or relative.
    //If absolute, we will translate it to relative in our code
    G4ThreeVector userPosition = G4ThreeVector(0, 0, 0); //Stores what the user inputs
    std::string positionType = ""; 
    G4ThreeVector absolutePosition = G4ThreeVector(0, 0, 0);       // center in world
    G4ThreeVector relativePosition = G4ThreeVector(0, 0, 0);       // center in parent
    //We don't allow for arbitrary rotations. However, cylinders in Geant4 are all 
    //instantiated oriented along the z-axis. We track rotation of subvolumes inside
    //cylinders which will have to have the cylinders rotation taken into account
    G4RotationMatrix rotationMatrix; // Starts as identity
    G4RotationMatrix cumulativeRotationMatrix; // Starts as identity

    // Shape-specific geometry properties
    G4ThreeVector halfLengths = G4ThreeVector(0,0,0);             // for boxes only
    double radius = -1.;                                           // for cylinders and spheres
    double halfHeight = -1.;                                       // for cylinders only
    G4ThreeVector absoluteCylinderAxis = G4ThreeVector(0,0,1);    // for cylinders only

    //For referencing instantiated geometry
    G4LogicalVolume* logical = nullptr; 
    G4VSolid* solid = nullptr;

    // Visualization properties (optional)
    G4ThreeVector rgb = G4ThreeVector(1.,1.,1.);
    double alpha = 1.; 

    //Function to sample random points inside the volume
    G4ThreeVector GenerateRandomPointInside() {
        G4ThreeVector localPoint;
        G4ThreeVector globalPoint;
        int nTries=0;
        int maxTries=1000;
        while (nTries < maxTries) {
            if (shape == "box") {
                G4double x = G4UniformRand() * 2*halfLengths.x()  - halfLengths.x();
                G4double y = G4UniformRand() * 2*halfLengths.y()  - halfLengths.y();
                G4double z = G4UniformRand() * 2*halfLengths.z()  - halfLengths.z();
                localPoint = G4ThreeVector(x, y, z);
            }
            else if (shape == "sphere") {
                //Some discussion here, sampling r uniformly oversamples interior of sphere
                //https://stackoverflow.com/questions/5408276/sampling-uniformly-distributed-random-points-inside-a-spherical-volume
                G4double r = radius * std::cbrt(G4UniformRand());
                G4double costheta = 2.0 * G4UniformRand() - 1.0; //From 1 to -1
                G4double phi = 2.0 * CLHEP::pi * G4UniformRand();

                G4double sintheta = std::sqrt(1.0 - costheta * costheta);
                localPoint = G4ThreeVector(r * sintheta * std::cos(phi),r * sintheta * std::sin(phi),r * costheta);
            }
            else if (shape == "cylinder") {
                G4double r = radius * std::sqrt(G4UniformRand());
                G4double phi = 2.0 * CLHEP::pi * G4UniformRand();
                G4double z = (G4UniformRand() * 2.0 - 1.0) * halfHeight;
                localPoint = G4ThreeVector(r * std::cos(phi),r * std::sin(phi),z);
            }
            else {
                G4Exception("GenerateRandomPointInside", "UnknownShape", FatalException,
                            ("Unknown shape: " + shape).c_str());
            }

            globalPoint = cumulativeRotationMatrix * localPoint + absolutePosition;
            //Check if this is inside a child volume. If not, break
            if (children.size()==0) {
                break;
            }

            bool pointValid=true;
            for (auto child: children) {
                if (child->IsPointInside(globalPoint)) {
                    pointValid = false;
                    break;
                }
            }
            nTries++;
            if (pointValid) {
                break;
            }
        }
        if (nTries==maxTries) {
                G4Exception("GenerateRandomPointInside", "BadSampling", FatalException,
                            ("Could not sample valid point in volume: " + name).c_str());
        }
        return globalPoint;
    }

    //Check if point insidie volume
    bool IsPointInside(const G4ThreeVector& globalPoint) const {
        // Transform global point to local coordinates - We could deal with absolute coordinates
        // if it weren't for the pesky cylinder rotation
        G4ThreeVector local = cumulativeRotationMatrix.inverse() * (globalPoint - absolutePosition);

        // Analytically check if point inside object
        if (shape == "box") {
            return std::abs(local.x()) <= halfLengths.x() &&
                std::abs(local.y()) <= halfLengths.y() &&
                std::abs(local.z()) <= halfLengths.z();
        }
        else if (shape == "cylinder") {
            double r2 = local.x() * local.x() + local.y() * local.y();
            return r2 <= radius * radius &&
                std::abs(local.z()) <= halfHeight;
        }
        else if (shape == "sphere") {
            return local.mag2() <= radius * radius;
        }
        else {
            G4Exception("CheckIfPointInside", "UnknownShape", FatalException,
                        ("Unknown shape: " + shape).c_str());
            return false;
        }
    }
};
#endif

