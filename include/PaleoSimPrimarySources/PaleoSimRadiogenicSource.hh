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
        double crystalAge = 0;
        Long64_t currentTreeEntry = 0;
        Long64_t nDecayChainEntries = 0;
        int recoilsInCluster = 0;
    
        // ROOT Tree Branch scalar buffers 
        Int_t b_event_id = 0;
        Int_t b_pdg = 0;
        Double_t b_KE_MeV = 0.0;
        Double_t b_u = 0.0;
        Double_t b_v = 0.0;
        Double_t b_w = 0.0;
        Double_t b_time_sec = 0.0;

        // Local struct to assemble one complete decay cascade
        struct DecayParticle {
            Int_t pdg;
            Double_t KE_MeV;
            Double_t u, v, w;
            Double_t time_sec;
        };

        PaleoSimVolumeDefinition* fSourceVolumeDefinition = nullptr;

    };
    
#endif