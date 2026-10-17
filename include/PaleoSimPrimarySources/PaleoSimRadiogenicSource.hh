#ifndef PaleoSimRadiogenicSource_h
#define PaleoSimRadiogenicSource_h 1

#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimRadiogenicSource : public PaleoSimPrimarySource {
    public:
        PaleoSimRadiogenicSource(PaleoSimMessenger& messenger);
        virtual ~PaleoSimRadiogenicSource();
        
        virtual void InitializeSource();
        virtual void GeneratePrimaries(G4Event*);
    private:
        TFile* decayChainFile = nullptr;
        bool decayChainFileLoaded = false;
        TTree* decayTree = nullptr;
        int crystalTime = 0;
        int nDecayChainEntries = 0;
        int decayChainEventIdx = 0;
    
        std::vector<int>* product_pdgcode = nullptr;
        std::vector<float>* product_energy = nullptr;
        std::vector<float>* product_u = nullptr;
        std::vector<float>* product_v = nullptr;
        std::vector<float>* product_w = nullptr;
        std::vector<float>* product_t = nullptr;
        std::vector<float>* cry_y = nullptr;
        
        std::vector<std::vector<int>> all_cry_pdgcodes;
        std::vector<std::vector<float>> all_cry_energy;
        std::vector<std::vector<float>> all_cry_u;
        std::vector<std::vector<float>> all_cry_v;
        std::vector<std::vector<float>> all_cry_w;
        std::vector<std::vector<float>> all_cry_x;
        std::vector<std::vector<float>> all_cry_y;  
    
        double overburden_z=0;
    };
    
#endif