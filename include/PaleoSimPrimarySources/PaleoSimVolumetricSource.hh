#ifndef PaleoSimVolumetricSource_h
#define PaleoSimVolumetricSource_h 1

#include "TFile.h"
#include "TH1D.h"
#include "G4ThreeVector.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"
#include "PaleoSimVolumeDefinition.hh"

class PaleoSimVolumetricSource : public PaleoSimPrimarySource {
public:
  PaleoSimVolumetricSource(PaleoSimMessenger& messenger);
  virtual ~PaleoSimVolumetricSource();

  virtual void InitializeSource();
  virtual void GeneratePrimaries(G4Event*);
private:

  TFile* fVolumetricSourceSpectrumFile = nullptr;
  bool fVolumetricSourceSpectrumFileLoaded = false;
  TH1D* fVolumetricSourceSpectrumHist = nullptr;
  int fVolumetricSourcePDGCode;
  bool fBoundingBoxInitialized = false;
  G4String fVolumetricSourceType;
  G4ThreeVector fVolumetricBoundsMin;
  G4ThreeVector fVolumetricBoundsMax;
  PaleoSimVolumeDefinition* fSourceVolumeDefinition = nullptr;
  
};

#endif