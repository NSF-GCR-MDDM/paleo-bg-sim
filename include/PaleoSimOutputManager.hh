#ifndef PALEOSIMOUTPUTMANAGER_HH
#define PALEOSIMOUTPUTMANAGER_HH

#include <vector>
#include <string>
#include "G4ThreeVector.hh"

class TFile;
class TTree;

class PaleoSimOutputManager {
public:
    static PaleoSimOutputManager& Get(); // Retrieves output manager for initializing/filling/writing

    void EnablePrimariesTree(bool enable); // Enabled unless disabled flag set in macro
    // void EnableTallyTree(bool enable);     // Enabled if "air cavity" present unless disabled in macro
    // void EnableRecoilTree(bool enable);    // Enabled if target present unless disabled in macro

    // Checks if enabled
    bool IsPrimariesTreeEnabled() const { return fEnablePrimariesTree; }
    // bool IsTallyTreeEnabled() const { return fEnableTallyTree; }
    // bool IsRecoilTreeEnabled() const { return fEnableRecoilTree; }

    // Sets up our trees
    void Initialize(bool hasTallyVolume = false, bool hasRecoilVolume = false);

    void FillPrimaryEvent(int eventID,
                         const std::vector<int>& pdgIDs,
                         const std::vector<double>& primaryEnergies,
                         const std::vector<double>& primary_x,
                         const std::vector<double>& primary_y,
                         const std::vector<double>& primary_z,
                         const std::vector<double>& primary_px,
                         const std::vector<double>& primary_py,
                         const std::vector<double>& primary_pz,
                         const std::vector<double>& muonTheta,
                         const std::vector<double>& muonPhi,
                         const std::vector<double>& muonSlant);

    /*
    void FillTallyEvent(int eventID, 
                        int pdgID, 
                        const double entry_x,
                        const double entry_y,
                        const double entry_z,
                        const double entry_px,
                        const double entry_py,
                        const double entry_pz, 
                        const double energy,
                        const double theta,
                        const double phi);

    void FillRecoilEvent(int eventID,
                         const std::vector<int>& pdgIDs,
                         const std::vector<double>& particle_energies,
                         const std::vector<double>& dep_energy,
                         const std::vector<double>& dep_x,
                         const std::vector<double>& dep_y,
                         const std::vector<double>& dep_z,
                         const std::vector<double>& dep_dirx,
                         const std::vector<double>& dep_diry,
                         const std::vector<double>& dep_dirz,
                         const std::vector<int>& intCode,
                         double totalEdep);
    */

    void WriteAndClose();
    
    void SetOutputPath(const std::string& path);

private:
    PaleoSimOutputManager() = default;
    ~PaleoSimOutputManager() = default;

    PaleoSimOutputManager(const PaleoSimOutputManager&) = delete;
    PaleoSimOutputManager& operator=(const PaleoSimOutputManager&) = delete;

    bool fEnablePrimariesTree = true;
    // bool fEnableTallyTree = false;
    // bool fEnableRecoilTree = false;

    TFile* fFile = nullptr;
    TTree* fPrimariesTree = nullptr;
    // TTree* fTallyTree = nullptr;
    // TTree* fRecoilTree = nullptr;

    std::string fOutputPath;

    // Primary Tree variables
    int fEventID;
    std::vector<int> fPrimaryPdgID;
    std::vector<double> fPrimaryEnergy;
    std::vector<double> fPrimaryX, fPrimaryY, fPrimaryZ;
    std::vector<double> fPrimaryPx, fPrimaryPy, fPrimaryPz;
    std::vector<double> fPrimaryTheta, fPrimaryPhi, fPrimarySlant;

    /*
    // Tally Tree variables
    int fTallyEventID;
    int fTallyPdgID;
    double fTallyEnergy;
    double fTallyX, fTallyY, fTallyZ;
    double fTallyPx, fTallyPy, fTallyPz;
    double fTallyTheta, fTallyPhi;

    // Recoil Tree variables
    int fRecoilEventID;
    double fRecoilTotalEdep;
    std::vector<int> fRecoilPdgID;
    std::vector<double> fRecoilParticleEnergy;
    std::vector<double> fRecoilEdep;
    std::vector<double> fRecoilX, fRecoilY, fRecoilZ;
    std::vector<double> fRecoilDirX, fRecoilDirY, fRecoilDirZ;
    std::vector<int> fRecoilInteractionCode;
    */
};

#endif // PALEOSIMOUTPUTMANAGER_HH