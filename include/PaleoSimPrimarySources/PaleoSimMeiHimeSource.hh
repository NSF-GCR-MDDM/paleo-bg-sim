#ifndef PaleoSimMeiHimeSource_h
#define PaleoSimMeiHimeSource_h 1

#include "TF1.h"
#include "G4ThreeVector.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimMeiHimeSource : public PaleoSimPrimarySource {
public:
  PaleoSimMeiHimeSource(PaleoSimMessenger& messenger);
  virtual ~PaleoSimMeiHimeSource();

  virtual void InitializeSource();
  virtual void GeneratePrimaries(G4Event*);
  
private:

  TF1* fMuonThetaDist = nullptr;
  TF1* fMuonEnergyDist = nullptr;
};

#endif