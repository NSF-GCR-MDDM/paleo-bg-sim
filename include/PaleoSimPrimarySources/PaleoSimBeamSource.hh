#ifndef PaleoSimBeamSource_h
#define PaleoSimBeamSource_h 1

#include "G4ThreeVector.hh"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TRandom3.h"

#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimBeamSource : public PaleoSimPrimarySource {
public:
  PaleoSimBeamSource(PaleoSimMessenger& messenger);
  virtual ~PaleoSimBeamSource();

  virtual void InitializeSource();
  virtual void GeneratePrimaries(G4Event*);

private:  
  TFile* spectrumFile = nullptr;
  bool spectrumFileLoaded = false;
  G4double monoEnergy_MeV = 1; //No hist passed in generates an isotropic source of mono energy
  TH1* energyHist = nullptr; //1D energy hist passed in generates an isotropic source
  TH2* energyCosThetaHist = nullptr; //2D energy vs. cosTheta hist passed in samples from that.
  int beamSourcePDGCode = 2112;
  G4ThreeVector sourcePosition;
  G4ThreeVector sourceDirection; //Only for 2D case
  G4double sourceRadius_mm = 10.;
  TRandom3 rng;
};

#endif