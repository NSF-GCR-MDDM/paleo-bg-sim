//Run Action

#ifndef MiniBooNEBeamlineRunAction_h
#define MiniBooNEBeamlineRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class

class MiniBooNEBeamlineRunAction : public G4UserRunAction
{
  public:
    MiniBooNEBeamlineRunAction();
    virtual ~MiniBooNEBeamlineRunAction();

    // virtual G4Run* GenerateRun();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

};

#endif

