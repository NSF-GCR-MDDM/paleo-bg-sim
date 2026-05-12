#ifndef PaleoSimCrySource_h
#define PaleoSimCrySource_h 1

#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimCrySource : public PaleoSimPrimarySource {
public:
  PaleoSimCrySource(PaleoSimMessenger& messenger);
  virtual ~PaleoSimCrySource();

  virtual void InitializeSource();
  virtual void GeneratePrimaries(G4Event*);
private:

    TFile* cryFile = nullptr;
    bool cryFileLoaded = false;
    TTree* cryTree = nullptr;
    int nCryEntries = 0;

    std::vector<int>* cry_pdgcode = nullptr;
    std::vector<float>* cry_energy = nullptr;
    std::vector<float>* cry_u = nullptr;
    std::vector<float>* cry_v = nullptr;
    std::vector<float>* cry_w = nullptr;
    std::vector<float>* cry_x = nullptr;
    std::vector<float>* cry_y = nullptr;
    
    std::vector<std::vector<int>> all_cry_pdgcodes;
    std::vector<std::vector<float>> all_cry_energy;
    std::vector<std::vector<float>> all_cry_u;
    std::vector<std::vector<float>> all_cry_v;
    std::vector<std::vector<float>> all_cry_w;
    std::vector<std::vector<float>> all_cry_x;
    std::vector<std::vector<float>> all_cry_y;  
};

#endif