#ifndef PALEOSIMOUTPUTMANAGER_HH
#define PALEOSIMOUTPUTMANAGER_HH

#include <vector>
#include <string>
#include <unordered_map>
#include "PaleoSimMessenger.hh" //Pass messenger in to read arguments for cavities/recoil volume, to determine what outputs to enable

class TFile;
class TTree;
class PaleoSimMessenger;

class PaleoSimOutputManager {
public:
    //Constructor/destructor
    PaleoSimOutputManager(PaleoSimMessenger& messenger);
    ~PaleoSimOutputManager() = default;

    //Tree generation/destruction
    void CreateOutputFileAndTrees();
    void WriteAndClose();

    //Setters for tree status
    void DeterminePrimariesTreeOutputStatus();         //Checks macro, sets the fPrimaryTreeStatus bool accordingly
    void DetermineNeutronTallyTreeOutputStatus();      //Checks macro, sets the fNeutronTallyTreeStatus bool accordingly

    //Getters for tree status
    bool GetPrimariesTreeOutputStatus() const { return fPrimariesTreeStatus; }
    bool GetNeutronTallyTreeOutputStatus() const { return fNeutronTallyTreeStatus; 

    // Primary event filler/clearer
    void FillPrimariesTreeEvent();
    void ClearPrimariesTreeEvent();
    void PushPrimaryEventID(int val) { fPrimaryEventID = val; };
    void PushPrimaryEventPDG(int val) { fPrimaryPdgID = val };
    void PushPrimaryEventEnergy(double val) { fPrimaryEnergy.push_back(val); };
    void PushPrimaryEventX(double val) { fPrimaryX.push_back(val); };
    void PushPrimaryEventY(double val) { fPrimaryY.push_back(val); };
    void PushPrimaryEventZ(double val) { fPrimaryZ.push_back(val); };
    void PushPrimaryEventPX(double val) { fPrimaryPX.push_back(val); };
    void PushPrimaryEventPY(double val) { fPrimaryPY.push_back(val); };
    void PushPrimaryEventPZ(double val) { fPrimaryPZ.push_back(val); };
    // CUSTOM_GENERATOR_HOOK
    // If you want to add more branches to the primaries tree, write setters here
    //
    // Mei & Hime muon generator
    void PushPrimaryMuonTheta(double val) { fPrimaryMuonTheta.push_back(val); };
    void PushPrimaryMuonPhi(double val) { fPrimaryMuonPhi.push_back(val); };
    void PushPrimaryMuonSlant(double val) { fPrimaryMuonSlant.push_back(val); };

    // Neutron tally event filler/clearer
    void FillNeutronTallyTreeEvent();
    void ClearNeutronTallyTreeEvent();
    void PushNeutronTallyEventID(int val) { fNeutronTallyEventID = val; };
    void PushNeutronTallyEventEntryEnergy(double val) { fNeutron_entryEnergy.push_back(val); };
    void PushNeutronTallyEventEntryX(double val) { fNeutron_entryX.push_back(val); };
    void PushNeutronTallyEventEntryY(double val) { fNeutron_entryY.push_back(val); };
    void PushNeutronTallyEventEntryZ(double val) { fNeutron_entryZ.push_back(val); };

private:
    PaleoSimMessenger& fMessenger; 

    bool fPrimariesTreeStatus = true;
    bool fNeutronTallyTreeStatus = false;

    TFile* fFile = nullptr;
    TTree* fPrimariesTree = nullptr;
    TTree* fNeutronTallyTree = nullptr; // Neutron tally tree

    // Primary Tree variables
    int fPrimaryEventID = -1;
    std::vector<int> fPrimaryPdgID;
    std::vector<double> fPrimaryEnergy;
    std::vector<double> fPrimaryX, fPrimaryY, fPrimaryZ;
    std::vector<double> fPrimaryPx, fPrimaryPy, fPrimaryPz;
    // CUSTOM_GENERATOR_HOOK
    // Create vectors to be stored as branches in the primaries tree for your custom generator here
    //
    //Mei & Hime Muon generator
    std::vector<double> fPrimaryMuonTheta, fPrimaryMuonPhi, fPrimaryMuonSlant;

    // Neutron Tally Tree variables (added for neutron tracking)
    int fNeutronTallyEventID = -1;
    std::vector<double> fNeutron_entryEnergy;
    std::vector<double> fNeutron_entryX, fNeutron_entryY, fNeutron_entryZ;
};

#endif
