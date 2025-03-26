#ifndef PALEOSIMOUTPUTMANAGER_HH
#define PALEOSIMOUTPUTMANAGER_HH

#include <vector>
#include <string>
#include <unordered_map>
#include "PaleoSimMessenger.hh" 
#include "TFile.h" 
#include "TTree.h"

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
    bool GetNeutronTallyTreeOutputStatus() const { return fNeutronTallyTreeStatus; }; 
    
    // Primary event filler/clearer
    void FillPrimariesTreeEvent();
    void ClearPrimariesTreeEvent();
    void GetPrimaryEventVectorSize() const { return fPrimaryPdgID.size(); };
    void PushPrimaryEventID(int val) { fPrimaryEventID = val; };
    void PushPrimaryEventPDG(int val) { fPrimaryPdgID.push_back(val); };
    void PushPrimaryEventEnergy(double val) { fPrimaryEnergy.push_back(val); };
    void PushPrimaryEventX(double val) { fPrimaryX.push_back(val); };
    void PushPrimaryEventY(double val) { fPrimaryY.push_back(val); };
    void PushPrimaryEventZ(double val) { fPrimaryZ.push_back(val); };
    void PushPrimaryEventPx(double val) { fPrimaryPx.push_back(val); };
    void PushPrimaryEventPy(double val) { fPrimaryPy.push_back(val); };
    void PushPrimaryEventPz(double val) { fPrimaryPz.push_back(val); };
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
    void GetNeutronTallyEventVectorSize() const { return fNeutron_entryEnergy.size(); };
    void PushNeutronTallyEventID(int val) { fNeutronTallyEventID = val; };
    void PushNeutronTallyEventEntryEnergy(double val) { fNeutron_entryEnergy.push_back(val); };
    void PushNeutronTallyEventCreationEnergy(double val) { fNeutron_creationEnergy.push_back(val); };
    void PushNeutronTallyEventEntryX(double val) { fNeutron_entryX.push_back(val); };
    void PushNeutronTallyEventEntryY(double val) { fNeutron_entryY.push_back(val); };
    void PushNeutronTallyEventEntryZ(double val) { fNeutron_entryZ.push_back(val); };
    void PushNeutronTallyEventEntryPx(double val) { fNeutron_entryPx.push_back(val); };
    void PushNeutronTallyEventEntryPy(double val) { fNeutron_entryPy.push_back(val); };
    void PushNeutronTallyEventEntryPz(double val) { fNeutron_entryPz.push_back(val); };
    void PushNeutronTallyEventDistanceToVertex(double val) { fNeutron_distToVertex.push_back(val); };
    void PushNeutronTallyEventEntryTheta(double val) { fNeutron_entryTheta.push_back(val); };


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
    std::vector<double> fNeutron_entryEnergy, fNeutron_creationEnergy;
    std::vector<double> fNeutron_entryX, fNeutron_entryY, fNeutron_entryZ;
    std::vector<double> fNeutron_entryPx, fNeutron_entryPy, fNeutron_entryPz;
    std::vector<double> fNeutron_distToVertex;
    std::vector<double> fNeutron_entryTheta;
};

#endif
