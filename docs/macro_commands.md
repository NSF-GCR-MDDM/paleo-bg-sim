# Macro Command Options

Automatically generated from PaleoSimMessenger.hh and PaleoSimMessenger.cc with genMacroDoc.py

Last generated on: 2025-05-12 20:25

## /geom/

- **/geom/setGeometry** (optional) — `filepath`
  - Description: Set full path to geometry macro file to load.
  - Default: `""`

## /output/

- **/output/setPrimariesTreeStatus** (optional) — `fPrimariesTreeStatus`
  - Description: Enable tracking of primary generation properties
  - Default: `true`

- **/output/setMINTreeStatus** (optional) — `fMINTreeStatus`
  - Description: Enable tracking of muon-induced neutrons
  - Default: `false`

- **/output/setNeutronTallyTreeVolume** (optional) — `fNeutronTallyTreeVolume`
  - Description: Set to track particles entering volume in neutronTallyTree
  - Default: `""`

- **/output/setRecoilTreeVolume** (optional) — `fRecoilTreeVolume`
  - Description: Set to track heavy secondaries in volume in recoilTree
  - Default: `""`

- **/output/setVRMLOutputStatus** (optional) — `fVRMLStatus`
  - Description: If true, will save vrml file of geometry in current folder with same name as geometry macro.
  - Default: `false`

## /generator/

- **/generator/setSourceType** (optional) — `fSourceType`
  - Description: Set the source type
  - Default: `"meiHimeMuonGenerator"`

- **/generator/setNPS** (optional) — `fNPS`
  - Description: Set number of events to throw
  - Default: `100`

### /generator/meiHimeMuonGenerator/

- **/generator/meiHimeMuonGenerator/setMuonEffectiveDepth** (optional) — `fMeiHimeMuonDepth`
  - Description: Set the effective muon depth for generation
  - Default: `6 * km`
  - Units: `km`
  - Valid range: `fMeiHimeMuonDepth >= 0.`

### /generator/muteGenerator/

- **/generator/muteGenerator/setMuteHistFilename** (optional) — `muteHistFilename`
  - Description: File containing 'muonHist' TH2D with GeV on x and Theta (rad) on y
  - Default: `""`

### /generator/cry/

- **/generator/cry/setCRYFilename** (optional) — `fCRYFilename`
  - Description: Pass in output of cryGenerator code (root file)
  - Default: `""`

