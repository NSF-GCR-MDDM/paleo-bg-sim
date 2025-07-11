# Geometry Command Options

Automatically generated from PaleoSimGeometryMessenger.hh and PaleoSimGeometryMessenger.cc with genGeomDoc.py

Last generated on: 2025-05-01 17:23

## /volume/

- **/volume/new** (optional) — `name`
  - Description: Define a new volume with the passed in (case-sensitive) name.

- **/volume/setParent** (optional) — `parentName`
  - Description: Set the parent volume by name. Omit or 'None' if this is the world volume

- **/volume/setShape** (required) — `shape`
  - Description: Set the shape: box, cylinder, sphere

- **/volume/setMaterial** (required) — `materialName`
  - Description: Set the material name. Case-sensitive name must be defined in PaleoSimMaterialManager.

- **/volume/setPosition** (optional) — `x`
  - Description: Set position in parent frame, with units!
  - Units: `mm`

- **/volume/setPositionType** (optional) — `positionType`
  - Description: Set whether provided position is 'relative' to parent coordinates or 'absolute'

- **/volume/setRGB** (optional) — `r`
  - Description: Set r g b, only for visualization

- **/volume/setAlpha** (optional) — `alpha`
  - Description: Set alpha only for visualization

### /volume/box/

- **/volume/box/setHalfLengths** (optional) — `xHalfLen`
  - Description: Set the half-length 3-vector of the box, with units
  - Units: `mm`

### /volume/cylinder/

- **/volume/cylinder/setRadius** (optional) — `radius`
  - Description: Set the radius of the cylinder, with units
  - Units: `mm`
  - Valid range: `radius > 0.`

- **/volume/cylinder/setHalfHeight** (optional) — `halfHeight`
  - Description: Set the halfHeight of the cylinder, with units! Must be > 0
  - Units: `mm`
  - Valid range: `halfHeight > 0.`

- **/volume/cylinder/setCylinderAxis** (optional) — `xVec`
  - Description: Set the cylinder axis 3-vector

- **/volume/finalize** (required) — ``
  - Description: Finalize volume and add to list, no args.

