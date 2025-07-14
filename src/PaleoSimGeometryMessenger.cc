#include "PaleoSimGeometryMessenger.hh"
#include "PaleoSimMaterialManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"

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
    fSetParentCmd->SetGuidance("Set the parent volume by name. Omit or 'None' if this is the world volume");
    fSetParentCmd->SetParameterName("parentName", true);
    fSetParentCmd->SetDefaultValue(defDefaults.parentName.c_str());

    fSetShapeCmd = new G4UIcmdWithAString("/volume/setShape", this);
    fSetShapeCmd->SetGuidance("Set the shape: box, cylinder, sphere");
    fSetShapeCmd->SetParameterName("shape", false);
    fSetShapeCmd->SetDefaultValue(defDefaults.shape.c_str());

    fSetMaterialCmd = new G4UIcmdWithAString("/volume/setMaterial", this);
    fSetMaterialCmd->SetGuidance("Set the material name. Case-sensitive name must be defined in PaleoSimMaterialManager.");
    fSetMaterialCmd->SetParameterName("materialName", false);
    fSetMaterialCmd->SetDefaultValue(defDefaults.materialName.c_str());

    //Positioning
    fSetPositionCmd = new G4UIcmdWith3VectorAndUnit("/volume/setPosition", this);
    fSetPositionCmd->SetGuidance("Set position in parent frame, with units!");
    fSetPositionCmd->SetParameterName("x","y","z",true);
    fSetPositionCmd->SetDefaultValue(defDefaults.userPosition);
    fSetPositionCmd->SetDefaultUnit("mm"); //Just for documentation

    fSetPositionTypeCmd = new G4UIcmdWithAString("/volume/setPositionType", this);
    fSetPositionTypeCmd->SetGuidance("Set whether provided position is 'relative' to parent coordinates or 'absolute'");
    fSetPositionTypeCmd->SetParameterName("positionType", true);
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

    //For cylinders and shperes
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
    fSetCylinderAxisCmd->SetGuidance("Set the cylinder axis 3-vector");
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
    //NVolume properties
    if (cmd == fNewCmd) {
        if (fMessenger->VolumeNameExists(val)) {
            G4Exception("PaleoSimGeometryMessenger", "DuplicateName", FatalException,
                        ("Volume with name already exists: " + val).c_str());
        }
        fCurrentVolume = new PaleoSimVolumeDefinition();
        fCurrentVolume->name = val;
    }
    //Otherwise we need to make sure we have a volume defined!
    else if (!fCurrentVolume) {
        G4Exception("PaleoSimGeometryMessenger", "NoVolume", FatalException, "Must call /volume/new first");
    }
    //Set parent
    else if (cmd == fSetParentCmd) {
        if (val.empty()) {
            G4Exception("PaleoSimGeometryMessenger", "ParentNameError", FatalException, 
                "Parent name must be defined or 'None'/command not present if world volume");
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
            ("Radisu must be >0. Set value was " + val).c_str());
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
    
        if (axis.mag2() < 1e-12) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidCylinderAxis", FatalException,
                        "Cylinder axis must be a non-zero vector.");
        }
    
        axis = axis.unit(); // normalize
        fCurrentVolume->absoluteCylinderAxis = axis;
    }

    //Finalize  command
    else if (cmd == fFinalizeCmd) {
        if (fCurrentVolume->name.empty()) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidName", FatalException,
                        ("Name missing from one of volumes! Names must be defined!"));
        }
        if (fCurrentVolume->shape.empty()) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidShape", FatalException,
                        ("Shape missing from one of volumes! Shapes must be defined!"));
        }
        if (fCurrentVolume->materialName.empty()) {
            G4Exception("PaleoSimGeometryMessenger", "InvalidMaterial", FatalException,
                        ("Material missing from one of volumes! Materials must be defined!"));
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
