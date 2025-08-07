#include "PaleoSimGeometryMessenger.hh"
#include "PaleoSimMaterialManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"
#include <unordered_set>

PaleoSimGeometryMessenger::PaleoSimGeometryMessenger(PaleoSimMessenger* messenger)
    : fMessenger(messenger)
{
    PaleoSimVolumeDefinition defDefaults;  // uses default values from volume definition class

    fVolumeDirectory = new G4UIdirectory("/volume/");
    fVolumeDirectory->SetGuidance("Holds properties of specific geometry volumes.");

    //Volume properties
    fNewCmd = new G4UIcmdWithAString("/volume/new", this);
    fNewCmd->SetGuidance("Define a new volume with the passed in (case-sensitive) name.");    
    fNewCmd->SetParameterName("name", false); //true = optional, false = required
    fNewCmd->SetDefaultValue(defDefaults.name.c_str()); // Purely for documentation

    fSetParentCmd = new G4UIcmdWithAString("/volume/setParent", this);
    fSetParentCmd->SetGuidance("Set the parent volume by name. 'None' if this is the world volume");
    fSetParentCmd->SetParameterName("parentName", false);
    fSetParentCmd->SetDefaultValue(defDefaults.parentName.c_str());

    fSetShapeCmd = new G4UIcmdWithAString("/volume/setShape", this);
    fSetShapeCmd->SetGuidance("Set the shape: box, cylinder, sphere");
    fSetShapeCmd->SetParameterName("shape", false);
    fSetShapeCmd->SetDefaultValue(defDefaults.shape.c_str());

    fSetMaterialCmd = new G4UIcmdWithAString("/volume/setMaterial", this);
    fSetMaterialCmd->SetGuidance("Set the material name. Case-sensitive name must be defined in PaleoSimMaterialManager.");
    fSetMaterialCmd->SetParameterName("materialName", false);
    fSetMaterialCmd->SetDefaultValue(defDefaults.materialName.c_str());

    fSetVolumeNumberCmd = new G4UIcmdWithAnInteger("/volume/setVolumeNumber",this);
    fSetVolumeNumberCmd->SetGuidance("Set the volume number. Non-duplicates required, will be used in output trees to identify this volume.");
    fSetVolumeNumberCmd->SetParameterName("volumeNumber", false);
    fSetVolumeNumberCmd->SetDefaultValue(defDefaults.volumeNumber);

    //Positioning
    fSetPositionCmd = new G4UIcmdWith3VectorAndUnit("/volume/setPosition", this);
    fSetPositionCmd->SetGuidance("Set position in parent frame, with units!");
    fSetPositionCmd->SetParameterName("x","y","z",false);
    fSetPositionCmd->SetDefaultValue(defDefaults.userPosition);
    fSetPositionCmd->SetDefaultUnit("mm"); //Just for documentation

    fSetPositionTypeCmd = new G4UIcmdWithAString("/volume/setPositionType", this);
    fSetPositionTypeCmd->SetGuidance("Set whether provided position is 'relative' to parent coordinates or 'absolute'");
    fSetPositionTypeCmd->SetParameterName("positionType", false);
    fSetPositionTypeCmd->SetDefaultValue(defDefaults.positionType.c_str());

    //Visual plotting only:
    fSetRGBCmd = new G4UIcmdWith3Vector("/volume/setRGB", this);
    fSetRGBCmd->SetGuidance("Set r g b, only for visualization");
    fSetRGBCmd->SetParameterName("r","g","b", true);
    fSetRGBCmd->SetDefaultValue(defDefaults.rgb);

    //Visual plotting only
    fSetAlphaCmd = new G4UIcmdWithADouble("/volume/setAlpha", this);
    fSetAlphaCmd->SetGuidance("Set alpha only for visualization");
    fSetAlphaCmd->SetParameterName("alpha", true);
    fSetAlphaCmd->SetDefaultValue(defDefaults.alpha);

    //For rectangles
    fBoxDirectory = new G4UIdirectory("/volume/box/");
    fBoxDirectory->SetGuidance("Holds box half-lengths");

    fSetHalfLengthsCmd = new G4UIcmdWith3VectorAndUnit("/volume/box/setHalfLengths", this);
    fSetHalfLengthsCmd->SetGuidance("Set the half-length 3-vector of the box, with units");
    fSetHalfLengthsCmd->SetParameterName("xHalfLen","yHalfLen","zHalfLen", true);
    fSetHalfLengthsCmd->SetDefaultValue(defDefaults.halfLengths);
    fSetHalfLengthsCmd->SetDefaultUnit("mm");

    //For cylinders and spheres
    fCylinderDirectory = new G4UIdirectory("/volume/cylinder/");
    fCylinderDirectory->SetGuidance("Holds cylinder radius, half-height");

    fSetRadiusCmd = new G4UIcmdWithADoubleAndUnit("/volume/cylinder/setRadius", this);
    fSetRadiusCmd->SetGuidance("Set the radius of the cylinder, with units");
    fSetRadiusCmd->SetParameterName("radius", true); 
    fSetRadiusCmd->SetRange("radius > 0.");
    fSetRadiusCmd->SetDefaultValue(defDefaults.radius);
    fSetRadiusCmd->SetDefaultUnit("mm");

    //For cylinders
    fSetHalfHeightCmd = new G4UIcmdWithADoubleAndUnit("/volume/cylinder/setHalfHeight", this);
    fSetHalfHeightCmd->SetGuidance("Set the halfHeight of the cylinder, with units! Must be > 0");
    fSetHalfHeightCmd->SetParameterName("halfHeight", true); 
    fSetHalfHeightCmd->SetRange("halfHeight > 0.");
    fSetHalfHeightCmd->SetDefaultValue(defDefaults.halfHeight);
    fSetHalfHeightCmd->SetDefaultUnit("mm");

    fSetCylinderAxisCmd = new G4UIcmdWith3Vector("/volume/cylinder/setCylinderAxis", this);
    fSetCylinderAxisCmd->SetGuidance("Set the cylinder axis 3-vector in ABSOLUTE coords");
    fSetCylinderAxisCmd->SetParameterName("xVec","yVec","zVec", true);
    fSetCylinderAxisCmd->SetDefaultValue(defDefaults.absoluteCylinderAxis);

    //Finalize the defition - stop accepting commands
    fFinalizeCmd = new G4UIcmdWithoutParameter("/volume/finalize", this);
    fFinalizeCmd->SetGuidance("Finalize volume and add to list, no args.");
}

PaleoSimGeometryMessenger::~PaleoSimGeometryMessenger() {
    delete fNewCmd;
    delete fSetParentCmd;
    delete fSetShapeCmd;
    delete fSetMaterialCmd;
    delete fSetVolumeNumberCmd;
    delete fSetPositionCmd;
    delete fSetPositionTypeCmd;
    delete fFinalizeCmd;
    delete fSetRGBCmd;
    delete fSetAlphaCmd;
    delete fSetHalfLengthsCmd;
    delete fSetRadiusCmd;
    delete fSetHalfHeightCmd;
    delete fSetCylinderAxisCmd;
}

void PaleoSimGeometryMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {
    //New volume!
    if (cmd == fNewCmd) {
        fCurrentVolume = new PaleoSimVolumeDefinition();
        fCurrentVolume->name = val;
    }
    //Otherwise we need to make sure we have a volume defined!
    else if (!fCurrentVolume) {
        G4Exception("PaleoSimGeometryMessenger", "NoVolume", FatalException, "Must call /volume/new first");
    }

    //Setting properties
    //Set parent
    else if (cmd == fSetParentCmd) {
        if (val.empty()) {
            G4Exception("PaleoSimGeometryMessenger", "ParentNameError", FatalException, 
                "Parent name must be defined or 'None' if world volume");
        }
        fCurrentVolume->parentName = val;
    }
    //Set shape
    else if (cmd == fSetShapeCmd) {
        if (std::find(fAllowedShapes.begin(), fAllowedShapes.end(), val) == fAllowedShapes.end()) {
            G4Exception("PaleoSimGeometryMessenger", "BadShape", FatalException,
                        ("Unknown shape: " + val).c_str());
        }
        fCurrentVolume->shape = val;
    }
    //Set material
    else if (cmd == fSetMaterialCmd) {
        if (!PaleoSimMaterialManager::Instance()->HasMaterial(val)) {
            G4cerr << "Material not found: " << val << "\n";
            G4cerr << "Known materials are:\n";
            PaleoSimMaterialManager::Instance()->PrintKnownMaterials();
            G4Exception("PaleoSimGeometryMessenger", "BadMaterial", FatalException,
                        ("Material not found: " + val).c_str());

        }
        fCurrentVolume->materialName = val;
    }
    else if (cmd == fSetVolumeNumberCmd) {
        int num = fSetVolumeNumberCmd->GetNewIntValue(val);
        if (num < 0) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidVolumeNumber", FatalException,
                        ("Volume number must be non-negative. You gave: " + val).c_str());
        }
        fCurrentVolume->volumeNumber = num;
    }
    //Positioning
    else if (cmd == fSetPositionCmd) {
        auto vec = fSetPositionCmd->GetNew3VectorValue(val);
        fCurrentVolume->userPosition = vec;
    }
    else if (cmd == fSetPositionTypeCmd) {
        std::string positionType = static_cast<std::string>(val);
        std::transform(positionType.begin(), positionType.end(), positionType.begin(), ::tolower);
        if (positionType!="relative" && positionType!="absolute") {
            G4Exception("PaleoSimGeometryMessenger", "InvalidPositionType", FatalException,
                ("setPositionType must be 'relative' or 'absolute'. You put "+val).c_str());
        }
        fCurrentVolume->positionType = positionType;
    }

    //Visual
    //Set RGB
    else if (cmd == fSetRGBCmd) {
        fCurrentVolume->rgb = fSetRGBCmd->GetNew3VectorValue(val);
    }
    //Set Alpha
    else if (cmd == fSetAlphaCmd) {
        G4double doubleVal = fSetAlphaCmd->GetNewDoubleValue(val);
        if ((doubleVal>1)||(doubleVal<0)) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidAlpha", FatalException,
            ("Alpha must be between 0 and 1. Set value was " + val).c_str());
        }
        fCurrentVolume->alpha = doubleVal;
    }

    //Shape-specific parameters
    //Set HalfLengths
    else if (cmd == fSetHalfLengthsCmd) {
        G4ThreeVector halfLengths = fSetHalfLengthsCmd->GetNew3VectorValue(val);
        if (halfLengths.x() <= 0 || halfLengths.y() <= 0 || halfLengths.z() <= 0) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidHalfLengths", FatalException,
                        ("All HalfLengths must be > 0"));
        }
        fCurrentVolume->halfLengths = halfLengths;
    }
    //Set radius
    else if (cmd == fSetRadiusCmd) {
        G4double doubleVal = fSetRadiusCmd->GetNewDoubleValue(val);
        if (doubleVal<=0) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidRadius", FatalException,
            ("Radius must be >0. Set value was " + val).c_str());
        }
        fCurrentVolume->radius = doubleVal;
    }
    //Set half-height
    else if (cmd == fSetHalfHeightCmd) {
        G4double doubleVal = fSetHalfHeightCmd->GetNewDoubleValue(val);
        if (doubleVal<=0) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidHalfHeight", FatalException,
                        ("HalfHeight must be > 0. Set value was" + val).c_str());
        }
        fCurrentVolume->halfHeight = doubleVal;
    }
    //Set cylinder axis
    else if (cmd == fSetCylinderAxisCmd) {
        G4ThreeVector axis = fSetCylinderAxisCmd->GetNew3VectorValue(val);
        std::cout<<"Passed in axis: "<<axis.x()<<','<<axis.y()<<','<<axis.z()<<std::endl;
        if (axis.mag2() < 1e-12) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidCylinderAxis", FatalException,
                        "Cylinder axis must be a non-zero vector.");
        }
    
        axis = axis.unit(); // normalize
        fCurrentVolume->absoluteCylinderAxis = axis;
    }

    //Finalize  command
    else if (cmd == fFinalizeCmd) {
        //Check world is correct:
        if (fCurrentVolume->parentName == "None") {
            if (fCurrentVolume->positionType != "absolute") {
                G4Exception("PaleoSimGeometryMessenger", "WorldPositionTypeError", FatalException,
                            "World volume must have absolute positioning.");
            }
            if (fCurrentVolume->userPosition.mag2() > 1e-12) {
                G4Exception("PaleoSimGeometryMessenger", "WorldPositionError", FatalException,
                            "World volume must be placed at (0,0,0) in absolute coordinates.");
            }
        }


        //Set either the absolutePosition or relative position from userPosition and positionType
        if (fCurrentVolume->positionType == "relative") {
            fCurrentVolume->relativePosition = fCurrentVolume->userPosition;
        }
        else {
            fCurrentVolume->absolutePosition = fCurrentVolume->userPosition;
        }
        fMessenger->AddVolume(fCurrentVolume);
        fCurrentVolume = nullptr;
    }
}

//After loading all geometry objects, this runs checks, calculates missing quantities, and generally makes things
//ready to use with the rest of the code
void PaleoSimGeometryMessenger::ValidateGeometry() {
    const auto& volumes = fMessenger->GetVolumes();

    // 1. Check that exactly one world volume is defined
    int numWorlds = 0;
    for (const auto* vol : volumes) {
        if (vol->parentName == "None") {
            numWorlds++;
        }
    }
    if (numWorlds != 1) {
        G4Exception("ValidateGeometry", "WorldVolumeError", FatalException,
                    "Exactly one world volume must be defined with parentName = 'None'");
    }

    // 2. Check for duplicate volume names
    std::unordered_set<std::string> seenNames;
    for (const auto* vol : volumes) {
        if (seenNames.count(vol->name)) {
            G4Exception("ValidateGeometry", "DuplicateName", FatalException,
                        ("Duplicate volume name found: " + vol->name).c_str());
        }
        seenNames.insert(vol->name);
    }

    // 3. Check for duplicate volume numbers
    std::unordered_set<int> seenVolumeNumbers;
    for (const auto& vol : volumes) {
        if (seenVolumeNumbers.count(vol->volumeNumber)) {
            G4Exception("ValidateGeometry", "DuplicateVolumeNumber", FatalException,
                        ("Duplicate volume number found: " + std::to_string(vol->volumeNumber)).c_str());
        }
        seenVolumeNumbers.insert(vol->volumeNumber);
    }

    // 4. Check that all parents exist
    for (const auto* vol : volumes) {
        if (vol->parentName != "None" && !fMessenger->GetVolumeByName(vol->parentName)) {
            G4Exception("ValidateGeometry", "MissingParent", FatalException,
                        ("Parent volume not found: " + vol->parentName).c_str());
        }
    }

    // 5. Sort volumes by hierarchy (parents before children)
    PaleoSimGeometryMessenger::SortVolumesByHierarchy();  // modifies fMessenger->fVolumes in place

    // 6. Compute missing coordinates--user supplies either abs or relative, we need to calculate the other.
    PaleoSimGeometryMessenger::ComputeMissingCoordinates(); 

    // 7. Compute cumulative rotation matrix
    PaleoSimGeometryMessenger::ComputeCumulativeRotationMatrix();
    
    // 8. Populate children vectors
    PaleoSimGeometryMessenger::PopulateChildrenVector();

    // 9. Check
    for (auto& vol: fMessenger->GetVolumes()) {
        std::cout<<vol->name<<" positioned at absolute coordinates ("<<vol->absolutePosition.x()<<","<<vol->absolutePosition.y()<<","<<vol->absolutePosition.z()<<",)"<<std::endl;
        std::cout<<vol->name<<" positioned at relative coordinates ("<<vol->relativePosition.x()<<","<<vol->relativePosition.y()<<","<<vol->relativePosition.z()<<",)"<<std::endl;
        std::cout<<"Shape: " << vol->shape << std::endl;
        if (vol->shape == "box") {
            std::cout << "Half-lengths: " << vol->halfLengths << std::endl;
        }
        if (vol->shape == "cylinder") {
            std::cout  << "  Radius: " << vol->radius << "  Half-height: " << vol->halfHeight << std::endl;
        }

        std::cout  << "  Cumulative Rotation Matrix:\n";
        std::cout  << "    [ " << vol->cumulativeRotationMatrix.xx() << " "
                << vol->cumulativeRotationMatrix.xy() << " "
                << vol->cumulativeRotationMatrix.xz() << " ]\n";
        std::cout  << "    [ " << vol->cumulativeRotationMatrix.yx() << " "
                << vol->cumulativeRotationMatrix.yy() << " "
                << vol->cumulativeRotationMatrix.yz() << " ]\n";
        std::cout  << "    [ " << vol->cumulativeRotationMatrix.zx() << " "
                    << vol->cumulativeRotationMatrix.zy() << " "
                    << vol->cumulativeRotationMatrix.zz() << " ]\n\n";

    }

}

//Make sure we build the geometry in the correct order
void PaleoSimGeometryMessenger::SortVolumesByHierarchy() {
    const auto& volumes = fMessenger->GetVolumes();

    std::vector<PaleoSimVolumeDefinition*> sortedVolumes;
    std::unordered_set<std::string> sortedVolumeNames;

    //Loop to get volumes in proper order. We start by finding the World volume
    //and add it to sortedVolumes. Then we add all volumes whose parents is world,
    // and so on until all objects are in sortedVolumes
    while (sortedVolumes.size() < volumes.size()) {

        //Tracking of volumes in the current layer
        std::unordered_set<std::string> currentLayerVolumeNames;

        for (auto* vol : volumes) {
            // Pass over objects already in sortedVolumeNames
            if (sortedVolumeNames.count(vol->name)) continue;

            // World volume or parent already inserted?
            if (vol->parentName == "None" || sortedVolumeNames.count(vol->parentName)) {
                sortedVolumes.push_back(vol);
                currentLayerVolumeNames.insert(vol->name);
            }
        }

        if (currentLayerVolumeNames.size()==0) {
            if (sortedVolumes.size() != volumes.size()) {
                G4Exception("PaleoSimGeometryMessenger::SortVolumesByHierarchy", "BadRelationship", FatalException,
                            ("Something went wrong with parent-child matching, please check geometry macro"));
            }
            break;
        }

        //Now insert all the currentLayerVolumeNames back into sortedVolumeNames.
        for (const auto& name : currentLayerVolumeNames) {
            sortedVolumeNames.insert(name);
        }
    }

    // Overwrite with sorted volumes
    fMessenger->OverwriteVolumes(sortedVolumes);
}

//Assumes already sorted by hierarchy
void PaleoSimGeometryMessenger::ComputeMissingCoordinates() {
    auto& volumes = fMessenger->GetVolumes();

    for (auto* vol : volumes) {
        //World has requirement than positioning system must be absolute, centered at 0,0,0. Set relative position so defined.
        if (vol->parentName == "None") {
            vol->relativePosition = G4ThreeVector(0, 0, 0);
        }
        //If not the world, get the parent. Determine whether we need to calcualte abs or relative coordinates for this volume.
        //Calculate that using the parents coords.
        else {
            auto* parent = fMessenger->GetVolumeByName(vol->parentName);
            if (vol->positionType == "relative") {
                vol->absolutePosition = parent->absolutePosition + vol->relativePosition;
            } 
            else {
                vol->relativePosition = vol->absolutePosition - parent->absolutePosition;
            }
        }
    }
}

//Assumes already sorted by hierarchy
void PaleoSimGeometryMessenger::ComputeCumulativeRotationMatrix() {
    auto& volumes = fMessenger->GetVolumes();

    for (auto* vol : volumes) {
        // World volume: no rotation
        if (vol->parentName == "None") {
            vol->rotationMatrix = G4RotationMatrix();             // Identity
            vol->cumulativeRotationMatrix = G4RotationMatrix();   // Identity
        }
        //If not the world, get the parent. Determine whether we need to modify the cumulative 
        //rotation matrix. Objects inside objects inherit their parents rotation. For cylinders,
        //we have to use rotation to align them along specified axes, which can mess up all their
        //children. So instead, we calculate the inverse transform needed to align the children
        //in their unrotated coordinate system.
        else {
            PaleoSimVolumeDefinition* parent = fMessenger->GetVolumeByName(vol->parentName);
            const G4RotationMatrix& parentCumRot = parent->cumulativeRotationMatrix;

            if (vol->shape == "cylinder") {
                // Align z-axis to desired absoluteCylinderAxis
                const G4ThreeVector zAxis(0, 0, 1);
                G4ThreeVector target = vol->absoluteCylinderAxis.unit();
                G4ThreeVector rotationAxis = zAxis.cross(target);
                double angle = std::acos(zAxis.dot(target));
                if (angle < 1e-12) {
                    vol->rotationMatrix = G4RotationMatrix();
                } else if (std::abs(angle - CLHEP::pi) < 1e-12) {
                    vol->rotationMatrix = G4RotationMatrix();
                    vol->rotationMatrix.rotateX(CLHEP::pi);
                } else {
                    rotationAxis = rotationAxis.unit();
                    vol->rotationMatrix.rotate(angle, rotationAxis);
                }

                // Cylinder uses its own alignment
                vol->cumulativeRotationMatrix = vol->rotationMatrix * parentCumRot;
            } else {
                // For non-rotated volumes, undo parent's rotation to keep alignment global
                G4RotationMatrix parentInverse = parentCumRot.inverse();
                vol->rotationMatrix = parentInverse;
            }
        }
    }
}

// Populates vectors of parents immediate children
void PaleoSimGeometryMessenger::PopulateChildrenVector() {
    auto& volumes = fMessenger->GetVolumes();

    for (auto* vol : volumes) {
        if (vol->parentName != "None") {
            auto* parent = fMessenger->GetVolumeByName(vol->parentName);
            parent->children.push_back(vol); 
        }
    }
}