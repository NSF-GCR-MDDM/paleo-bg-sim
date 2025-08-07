# Macro Command Options

Automatically generated from PaleoSimMessenger.hh and PaleoSimMessenger.cc with genMacroDoc.py

Last generated on: 2025-08-07 12:38

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

- **/output/setNeutronTallyTreeVolumes** (optional) — `fNeutronTallyTreeVolumes`
  - Description: List of names of volumes to track neutrons entering, separated by spaces

- **/output/setRecoilTreeVolumes** (optional) — `fRecoilTreeVolumes`
  - Description: List of names of volumes to track nuclear recoils in, separated by spaces

- **/output/setVRMLOutputStatus** (optional) — `fVRMLStatus`
  - Description: If true, will save vrml file of geometry in current folder with same name as geometry macro.
  - Default: `false`

## /generator/

- **/generator/setSourceType** (optional) — `fSourceType`
  - Description: Set the source type
  - Default: `nullptr`

- **/generator/setNPS** (optional) — `fNPS`
  - Description: Set number of events to throw
  - Default: `100`

### /generator/meiHimeMuonGenerator/

- **/generator/meiHimeMuonGenerator/setMuonEffectiveDepth** (optional) — `fMeiHimeMuonDepth`
  - Description: Set the effective muon depth for generation
  - Default: `nullptr`
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

### /generator/VolumetricSource/

- **/generator/VolumetricSource/setVolumeName** (optional) — `fVolumetricSourceVolumeName`
  - Description: Set Volumetric source name, must be defined in geometry file
  - Default: `val; }`

- **/generator/VolumetricSource/setPDGCode** (optional) — `fVolumetricSourcePDGCode`
  - Description: Input PDG code of particle to throw
  - Default: `val; }`

- **/generator/VolumetricSource/setVolumetricSourceType** (optional) — `fVolumetricSourceType`
  - Description: Set Volumetric source type: 'mono' or 'hist'
  - Default: `val; }`

- **/generator/VolumetricSource/setVolumetricSourceSpectrumFilename** (optional) — `fVolumetricSourceSpectrumFilename`
  - Description: Input name of root file with spectrum TH1D or TH1F and energy in MeV
  - Default: `val; }`

- **/generator/VolumetricSource/setVolumetricSourceSpectrumHistName** (optional) — `fVolumetricSourceSpectrumHistName`
  - Description: Input name of TH1D or TH1F with energy in MeV!
  - Default: `val; }`

- **/generator/VolumetricSource/setVolumetricSourceMonoEnergy** (optional) — `fVolumetricSourceMonoEnergy`
  - Description: Set energy and unit of particles
  - Default: `val;}`
  - Units: `MeV`
  - Valid range: `fVolumetricSourceMonoEnergy >= 0.`

### /generator/diskSource/

- **/generator/diskSource/pdgCode** (optional) — `fDiskSourcePDGCode`
  - Description: Input PDG code of particle to throw
  - Default: `val; }`

- **/generator/diskSource/setDiskSourceType** (optional) — `fDiskSourceType`
  - Description: Set disk source type: 'mono' or 'hist'
  - Default: `val; }`

- **/generator/diskSource/setDiskSourceSpectrumFilename** (optional) — `fDiskSourceSpectrumFilename`
  - Description: Input name of root file with spectrum TH1D or TH1F and energy in MeV
  - Default: `val; }`

- **/generator/diskSource/setDiskSourceSpectrumHistName** (optional) — `fDiskSourceSpectrumHistName`
  - Description: Input name of TH1D or TH1F with energy in MeV!
  - Default: `val; }`

- **/generator/diskSource/setDiskSourceMonoEnergy** (optional) — `fDiskSourceMonoEnergy`
  - Description: Set energy and unit of particles
  - Default: `val;}`
  - Units: `MeV`
  - Valid range: `fDiskSourceMonoEnergy >= 0.`

- **/generator/diskSource/setDiskSourceRadius** (optional) — `fDiskSourceRadius`
  - Description: Set radius of disk
  - Default: `val;}`
  - Units: `cm`
  - Valid range: `fDiskSourceRadius >= 0.`

- **/generator/diskSource/setDiskSourcePositionCmd** (required) — ``
  - Description: Set position of center of disk
  - Default: `val;}`
  - Units: `cm`

- **/generator/diskSource/setDiskSourceAxis** (required) — ``
  - Description: Set axis of the disk.
  - Default: `val;}`

- **/generator/diskSource/setDiskSourceDirection** (required) — ``
  - Description: Set direction of particles emergying from the disk
  - Default: `val;}`

