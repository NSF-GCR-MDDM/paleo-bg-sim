#ifndef PaleoSimActionInitialization_h
#define PaleoSimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimOutputManager.hh"
#include "PaleoSimPrimaryGeneratorAction.hh"

class PaleoSimActionInitialization : public G4VUserActionInitialization
{
  public:
    PaleoSimActionInitialization(PaleoSimMessenger& messenger,PaleoSimOutputManager& manager);
    virtual ~PaleoSimActionInitialization() = default;

    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
    PaleoSimMessenger& fMessenger;
    PaleoSimOutputManager& fOutputManager;
};

#endif

    
