#ifndef PaleoSimBeamSource_h
#define PaleoSimBeamSource_h 1

#include "G4ThreeVector.hh"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

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
  TH1* energyHist = nullptr; //1D energy hist passed in
  TH2* energyCosThetaHist = nullptr; //2D energy vs. cosTheta hist passed in samples from that.
  G4bool needs1DHist = false;
  G4bool needs2DHist = false;

  G4String spectrumType = "mono"; //Mono or hist
  G4String directionType = "isotropic"; //fixed, isotropic, cosine, or hist2D
  G4double monoEnergy_MeV = 1; //No hist passed in uses mono energy

  int beamSourcePDGCode = 2112;
  G4double sourceRadius_mm = 10.;

  G4ThreeVector sourcePosition;
  G4ThreeVector sourceAxis;
};

#endif