#include "PaleoSimDetectorConstruction.hh"
#include "PaleoSimMaterialManager.hh"
#include "PaleoSimVolumeDefinition.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

PaleoSimDetectorConstruction::PaleoSimDetectorConstruction(PaleoSimMessenger& messenger)
: G4VUserDetectorConstruction(),
  fMessenger(messenger)
{}

PaleoSimDetectorConstruction::~PaleoSimDetectorConstruction() {}

G4VPhysicalVolume* PaleoSimDetectorConstruction::Construct() {
    G4bool checkOverlaps = true;
    const auto& volumes = fMessenger.GetVolumes();

    G4VPhysicalVolume* physWorld = nullptr;

    // Create solids and logical volumes
    for (const auto& def : volumes) {
        G4Material* mat = PaleoSimMaterialManager::Instance()->GetMaterial(def->materialName);

        G4VSolid* solid = nullptr;
        if (def->shape == "box") {
            solid = new G4Box(def->name,def->halfLengths.x(), def->halfLengths.y(),def->halfLengths.z());
        } else if (def->shape == "cylinder") {
            solid = new G4Tubs(def->name, 0.0, def->radius,def->halfHeight,0.0 * deg,360.0 * deg);
        } else if (def->shape == "sphere") {
            solid = new G4Sphere(def->name,0.0, def->radius,0.0 * deg,360.0 * deg,0.0 * deg,180.0 * deg);
        } else {
            G4Exception("PaleoSimDetectorConstruction", "UnknownShape", FatalException,
                        ("Unknown shape type: " + def->shape).c_str());
        }

        G4LogicalVolume* logical = new G4LogicalVolume(solid, mat, def->name);
        fLogicalVolumes[def->name] = logical;

        def->solid = solid;
        def->logical = logical;

        // Visualization attributes
        G4VisAttributes* vis = new G4VisAttributes(G4Colour(def->rgb.x(), def->rgb.y(), def->rgb.z(), def->alpha));
        vis->SetForceSolid(true);
        logical->SetVisAttributes(vis);
    }

    // Place physical volumes
    for (const auto& def : volumes) {
        //Get parrent volume--nullptr for world volume
        G4LogicalVolume* mother = nullptr;
        if (def->parentName != "None") {
            mother = fLogicalVolumes.at(def->parentName);
        }

        //Cylinders are all created relative to the z-axis. We want to
        //rotate them to absolute axes provided by the user. Doing
        //so rotates all their children volumes, so we have to undo
        //that.
        G4RotationMatrix* rot = nullptr;
        if (def->shape == "cylinder") {
            rot = new G4RotationMatrix(def->rotationMatrix); // apply user-specified rotation
        }
        else {
            // Undo the rotation of the parent (if any)
            PaleoSimVolumeDefinition* parent = fMessenger.GetVolumeByName(def->parentName);
            if (parent) {
                G4RotationMatrix undo = parent->cumulativeRotationMatrix.inverse();
                rot = new G4RotationMatrix(undo);
            }
        }
        
        auto* placed = new G4PVPlacement(
            rot,
            def->relativePosition,
            fLogicalVolumes.at(def->name),
            def->name,
            mother,
            false,
            0,
            checkOverlaps
        );
        
        if (def->parentName == "None") {
            physWorld = placed;
        }
    }

    return physWorld;
}