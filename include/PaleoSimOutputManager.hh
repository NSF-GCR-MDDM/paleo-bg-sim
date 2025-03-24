#ifndef PALEOSIMOUTPUTMANAGER_HH
#define PALEOSIMOUTPUTMANAGER_HH

#include <vector>
#include <string>

class TFile;
class TTree;

class PaleoSimOutputManager {
public:
    static PaleoSimOutputManager& Get(); // Retrieves output manager for initializing/filling/writing

    //Setters
    void EnablePrimariesTree(bool enable);  // Enabled unless disabled flag set in macro
    void EnableNeutronTallyTree(bool enable); // Added for neutron tally tree

    //Getters
    bool IsPrimariesTreeEnabled() const { return fEnablePrimariesTree; }
    bool IsNeutronTallyTreeEnabled() const { return fEnableNeutronTallyTree; }

    // Sets up our trees
    void Initialize(bool hasTallyVolume = false, bool hasRecoilVolume = false);

    // Primary event filler
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

    // Neutron tally filler (this is the method you will use for tracking neutron info)
    void FillNeutronTallyEvent(int eventID,
                                //const std::vector<double>& neutron_genEnergy,
                                const std::vector<double>& neutron_entryEnergy,
                                const std::vector<double>& neutron_entryX,
                                const std::vector<double>& neutron_entryY,
                                const std::vector<double>& neutron_entryZ
                                //const std::vector<double>& neutron_entryPx,
                                //const std::vector<double>& neutron_entryPy,
                                //const std::vector<double>& neutron_entryPz,
                                //const std::vector<double>& neutron_thetas,
                                //const std::vector<double>& neutron_distToMuon,
                                //const std::vector<int>& neutron_type
                                );

    void WriteAndClose();
    void SetOutputPath(const std::string& path);

private:
    PaleoSimOutputManager() = default;
    ~PaleoSimOutputManager() = default;

    PaleoSimOutputManager(const PaleoSimOutputManager&) = delete;
    PaleoSimOutputManager& operator=(const PaleoSimOutputManager&) = delete;

    bool fEnablePrimariesTree = true;
    bool fEnableNeutronTallyTree = false; // Added for enabling neutron tally tree

    TFile* fFile = nullptr;
    TTree* fPrimariesTree = nullptr;
    TTree* fNeutronTallyTree = nullptr; // Neutron tally tree

    std::string fOutputPath;

    // Primary Tree variables
    int fEventID;
    std::vector<int> fPrimaryPdgID;
    std::vector<double> fPrimaryEnergy;
    std::vector<double> fPrimaryX, fPrimaryY, fPrimaryZ;
    std::vector<double> fPrimaryPx, fPrimaryPy, fPrimaryPz;
    std::vector<double> fPrimaryTheta, fPrimaryPhi, fPrimarySlant;

    // Neutron Tally Tree variables (added for neutron tracking)
    int fNeutronTallyEventID;
    std::vector<double> fNeutron_genEnergy;
    std::vector<double> fNeutron_entryEnergy;
    std::vector<double> fNeutron_entryX, fNeutron_entryY, fNeutron_entryZ;
    std::vector<double> fNeutron_entryPx, fNeutron_entryPy, fNeutron_entryPz;
    std::vector<double> fNeutron_thetas;
    std::vector<double> fNeutron_distToMuon;
    std::vector<int> fNeutron_type; // 0 for secondary, 1 for tertiary
};

#endif // PALEOSIMOUTPUTMANAGER_HH
