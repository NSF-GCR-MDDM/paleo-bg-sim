#ifndef PALEOSIMEVENTINFO_HH
#define PALEOSIMEVENTINFO_HH

#include "G4UserEventInformation.hh"
#include <deque>
#include "globals.hh"

class PaleoSimEventInfo : public G4VUserEventInformation {
  public:
      PaleoSimEventInfo() = default;
  
      // CUSTOM_GENERATOR_HOOK
      // If you need information passed from your primary generation to your PrimariesTree, put that here
      //
      // Mei & Hime muon generator
      deque<G4double> muonTheta = {};
      deque<G4double> muonPhi = {};
      deque<G4double> muonSlantDepth = {};
  };