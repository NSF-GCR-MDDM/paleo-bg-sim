#ifndef PaleoSimPrimarySource_h
#define PaleoSimPrimarySource_h 1

#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4PhysicalConstants.hh"

#include "PaleoSimMessenger.hh"

class G4Event;

class PaleoSimPrimarySource {
public:
  PaleoSimPrimarySource(PaleoSimMessenger& messenger);
  virtual ~PaleoSimPrimarySource();

  virtual void InitializeSource();
  virtual void GeneratePrimaries(G4Event*);
  
protected:
  PaleoSimMessenger& fMessenger;
  
  G4ThreeVector SamplePointOnDisk(double radius,const G4ThreeVector& position, const G4ThreeVector& axis);
  G4ThreeVector SamplePointOnTopOfWorldVolume();
  G4bool IsWithinTopSurface(const G4ThreeVector& point);
};

#endif