#ifndef PaleoSimEventAction_h
#define PaleoSimEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimMessenger.hh"

class PaleoSimEventAction : public G4UserEventAction
{
  public:
    PaleoSimEventAction(PaleoSimMessenger& messenger,PaleoSimOutputManager& manager);
    virtual ~PaleoSimEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

  private:
  PaleoSimMessenger& fMessenger;
  PaleoSimOutputManager& fOutputManager;
  
};

#endif