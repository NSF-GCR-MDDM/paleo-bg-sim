#ifndef PaleoSimSCTSource_h
#define PaleoSimSCTSource_h 1

#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimSCTSource : public PaleoSimPrimarySource {
    public:
      PaleoSimSCTSource(PaleoSimMessenger& messenger);
      virtual ~PaleoSimSCTSource();
    
      virtual void InitializeSource();
      virtual void GeneratePrimaries(G4Event*);
    private:
    
        TFile* SCTFile = nullptr;
        bool SCTFileLoaded = false;
        TTree* SCTTree = nullptr;
        int nSCTEntries = 0;
        std::string SCTBootstrap = "standard";

        std::vector<int>* sct_pdgcode = nullptr;
        std::vector<float>* sct_energy = nullptr;
        std::vector<float>* sct_u = nullptr;
        std::vector<float>* sct_v = nullptr;
        std::vector<float>* sct_w = nullptr;
        std::vector<float>* sct_x = nullptr;
        std::vector<float>* sct_y = nullptr;
        
        std::vector<std::vector<int>> all_sct_pdgcodes;
        std::vector<std::vector<float>> all_sct_energy;
        std::vector<std::vector<float>> all_sct_u;
        std::vector<std::vector<float>> all_sct_v;
        std::vector<std::vector<float>> all_sct_w;
        std::vector<std::vector<float>> all_sct_x;
        std::vector<std::vector<float>> all_sct_y;
    };

#endif