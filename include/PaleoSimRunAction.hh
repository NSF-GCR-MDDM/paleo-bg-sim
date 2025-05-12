#ifndef PaleoSimRunAction_h
#define PaleoSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "PaleoSimOutputManager.hh"

class PaleoSimRunAction : public G4UserRunAction
{
  public:
    PaleoSimRunAction(PaleoSimOutputManager& manager);
    virtual ~PaleoSimRunAction() = default;

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
  private:
    PaleoSimOutputManager& fOutputManager;
};

#endif

