#ifndef PaleoSimDiskSource_h
#define PaleoSimDiskSource_h 1

#include "G4ThreeVector.hh"
#include "TH1D.h"
#include "TFile.h"

#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimDiskSource : public PaleoSimPrimarySource {
public:
  PaleoSimDiskSource(PaleoSimMessenger& messenger);
  virtual ~PaleoSimDiskSource();

  virtual void InitializeSource();
  virtual void GeneratePrimaries(G4Event*);


private:  
  TFile* diskSourceSpectrumFile = nullptr;
  bool diskSourceSpectrumFileLoaded = false;
  TH1D* diskSourceSpectrumHist = nullptr;
  int diskSourcePDGCode;
  G4ThreeVector diskSourcePosition;
  G4String diskSourceType;
};

#endif