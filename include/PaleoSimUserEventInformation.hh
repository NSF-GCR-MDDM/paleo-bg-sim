#ifndef PALEOSIMEVENTINFO_HH
#define PALEOSIMEVENTINFO_HH

#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include <deque>
#include "globals.hh"

class PaleoSimUserEventInformation : public G4VUserEventInformation {
  public:
      PaleoSimUserEventInformation() = default;
  
      //For multiparticle events, these are mass-energy weighted
      G4ThreeVector primaryDirection;
      G4ThreeVector primaryGenerationPosition;

      // CUSTOM_GENERATOR_HOOK
      // If you need information passed from your primary generation to your PrimariesTree, put that here
      //
      // Mei & Hime muon generator (and same for mute for all except slant)
      G4double muonTheta;
      G4double muonPhi;
      G4double muonSlantDepth;
      //
      //CRY
      G4ThreeVector CRYCorePosition;
      G4double CRYCoreTheta,CRYCorePhi;
      G4double CRYTotalEnergy;
      
      void Print() const override {};
  };
  
  #endif
