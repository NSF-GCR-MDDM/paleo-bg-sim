#ifndef PaleoSimSCTSource_h
#define PaleoSimSCTSource_h 1

#include <vector>
#include <string>
#include <mutex>

#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimSCTSource : public PaleoSimPrimarySource {
  public:
      PaleoSimSCTSource(PaleoSimMessenger& messenger);
      virtual ~PaleoSimSCTSource();
  
      virtual void InitializeSource() override;
      virtual void GeneratePrimaries(G4Event* anEvent) override;
  
      bool CheckTopSurface(const G4ThreeVector& pos) { return IsWithinTopSurface(pos); }
      G4ThreeVector GetSampledWorldTop() { return SamplePointOnTopOfWorldVolume(); }
  
  private:
      std::string SCTBootstrap = "standard";
      size_t nSCTEntries = 0;
  };

struct SCTParticle {
    int pdgCode;
    double energy;
    double u, v, w;
    double x, y;
};

struct SCTEvent {
    std::vector<SCTParticle> particles;
};

class PaleoSimSCTPool {
public:
    static PaleoSimSCTPool& Instance() {
        static PaleoSimSCTPool instance;
        return instance;
    }

    void LoadPool(const std::string& filename,
                  PaleoSimSCTSource* SCTSourceObject);

    size_t GetPoolSize() const { return fPool.size(); }
    const SCTEvent& GetEvent(size_t index) const { return fPool[index]; }
    double GetNorm() const { return fNorm; }

private:
    PaleoSimSCTPool() = default;
    ~PaleoSimSCTPool() = default;
    PaleoSimSCTPool(const PaleoSimSCTPool&) = delete;
    PaleoSimSCTPool& operator=(const PaleoSimSCTPool&) = delete;

    std::vector<SCTEvent> fPool;
    double fNorm = 0.0;
    bool fIsLoaded = false;
    std::mutex fInitMutex;
};

#endif