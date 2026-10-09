#ifndef PALEOSIMTRACKINFORMATION_HH
#define PALEOSIMTRACKINFORMATION_HH

#include "G4VUserTrackInformation.hh"
#include "globals.hh"

// Per-track information attached to nuclear recoils recorded in the recoil tree.
// externallyTransported marks recoils whose subsequent transport (including their EM knock-on
// cascade) is to be simulated by an external track simulation software, like SRIM,
// so the stepping action can skip those knock-ons.
class PaleoSimTrackInformation : public G4VUserTrackInformation {
  public:
      PaleoSimTrackInformation() = default;

      G4bool externallyTransported = false;

      void Print() const override {};
};

#endif