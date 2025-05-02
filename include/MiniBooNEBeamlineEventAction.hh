#ifndef MiniBooNEBeamlineEventAction_h
#define MiniBooNEBeamlineEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimMessenger.hh"
#include <map>

class MiniBooNEBeamlineEventAction : public G4UserEventAction
{
  public:
    MiniBooNEBeamlineEventAction(PaleoSimMessenger& messenger,PaleoSimOutputManager& manager);
    virtual ~MiniBooNEBeamlineEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);
    void AddMuon(G4int trackID, const G4ThreeVector& dir);
    G4ThreeVector GetMuonDirection(G4int trackID) const;
    bool IsMuon(G4int trackID) const;
    void ClearMuonMap();

  private:
    PaleoSimOutputManager& fOutputManager;
    PaleoSimMessenger fMessenger;
    std::map<G4int, G4ThreeVector> fMuonMap;
  
};

#endif