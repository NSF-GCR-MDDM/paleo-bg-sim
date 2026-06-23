#ifndef PaleoSimMuteSource_h
#define PaleoSimMuteSource_h 1

#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

class PaleoSimMuteSource : public PaleoSimPrimarySource {
    public:
        PaleoSimMuteSource(PaleoSimMessenger& messenger);
        virtual ~PaleoSimMuteSource();

        virtual void InitializeSource();
        virtual void GeneratePrimaries(G4Event*);
    private:
        TFile* muteFile = nullptr;
        bool muteFileLoaded = false;
        TH3D* muteHist = nullptr;
        bool muteHistLoaded = false;
        bool mountainProfileLoaded = false;

        double mountainSlantDepth;
        double mountainAzimuthal;
        double mountainZenith;

        std::vector<double> allMountainSlantDepths;
        std::vector<double> allMountainAzimuthals;
        std::vector<double> allMountainZeniths;
};

#endif