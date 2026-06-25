#ifndef PaleoSimMuteSource_h
#define PaleoSimMuteSource_h 1

#include <vector>
#include <map>
#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"
#include "PaleoSimMessenger.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

/*
* MUTE mountain profiles contain three columns of thetas, phis, and slant depths. 
* This class intends to store the profile as sets of "rings" of the mountain at 
* each zenith angle provided in the file.
*/

struct TerrainPoint {
    double phi;
    double slantDepth;
};

class MountainProfile {
    public:
    MountainProfile() {};
    void AddMountainPoint(double theta, double phi, double X);
    double FindOrSampleAzimuthal(double targetTheta, double targetX) const;

    private:
    std::map<double, std::vector<TerrainPoint>> profileMap;
    std::vector<double> FindPhiOnRing(const std::vector<TerrainPoint>& mountainRing, double targetX) const;
    std::vector<TerrainPoint> CreateInterpolatedRing(const std::vector<TerrainPoint>& mountainRing1,
                                                     const std::vector<TerrainPoint>& mountainRing2,
                                                     double fraction) const;
};

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

        MountainProfile mountainProfile;
};

#endif