#ifndef PALEOSIMEVENTINFO_HH
#define PALEOSIMEVENTINFO_HH

#include "G4VUserEventInformation.hh"
#include <deque>
#include "globals.hh"

class PaleoSimUserEventInformation : public G4VUserEventInformation {
  public:
      PaleoSimUserEventInformation() = default;
  
      // CUSTOM_GENERATOR_HOOK
      // If you need information passed from your primary generation to your PrimariesTree, put that here
      //
      // Mei & Hime muon generator
      std::deque<G4double> muonTheta = {};
      std::deque<G4double> muonPhi = {};
      std::deque<G4double> muonSlantDepth = {};
      
      void Print() const override {};
  };
  
  #endif
