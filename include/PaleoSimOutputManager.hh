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

    //PRIMARY TREE
    void FillPrimariesTreeEvent();
    void ClearPrimariesTreeEvent();
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
    void SetPrimaryMuonTheta(double val) { fPrimaryMuonTheta = val; };
    void SetPrimaryMuonPhi(double val) { fPrimaryMuonPhi = val; };
    void SetPrimaryMuonSlant(double val) { fPrimaryMuonSlant = val; };
    //
    //CRY generator
    void SetCRYCoreX(double val) { fCRYCoreX = val;};
    void SetCRYCoreY(double val) { fCRYCoreY = val;};
    void SetCRYCoreZ(double val) { fCRYCoreZ = val;};
    void SetCRYCoreTheta(double val) { fCRYCoreTheta = val;};
    void SetCRYCorePhi(double val) { fCRYCorePhi = val;};
    void SetCRYTotalEnergy(double val) { fCRYTotalEnergy = val; };

    //MUON-INDUCED NEUTRON TREE
    void FillMINTreeEvent();
    void ClearMINTreeEvent();
    void PushMINEventID(int val) { fMINEventID = val; };
    void PushMINEventMultiplicity(int val) { fMINEventMultiplicity = val; };
	void IncrementMINEventMultiplicity() { fMINEventMultiplicity++; };
    void PushMINEventAngleRelMuon(double val) { fMINEventAngleRelMuon.push_back(val); };
    void PushMINEventEnergy(double val) { fMINEventEnergy.push_back(val); };
    void PushMINEventDistanceToMuonTrack(double val) { fMINEventDistanceToMuonTrack.push_back(val); };

    //NEUTRON TALLY TREE
    void FillNeutronTallyTreeEvent();
    void ClearNeutronTallyTreeEvent();
    void PushNeutronTallyEventID(int val) { fNeutronTallyEventID = val; };
    void PushNeutronTallyEventEntryEnergy(double val) { fNeutron_entryEnergy.push_back(val); };
    void PushNeutronTallyEventEntryX(double val) { fNeutron_entryX.push_back(val); };
    void PushNeutronTallyEventEntryY(double val) { fNeutron_entryY.push_back(val); };
    void PushNeutronTallyEventEntryZ(double val) { fNeutron_entryZ.push_back(val); };
    void PushNeutronTallyEventEntryU(double val) { fNeutron_entryU.push_back(val); };
    void PushNeutronTallyEventEntryV(double val) { fNeutron_entryV.push_back(val); };
    void PushNeutronTallyEventEntryW(double val) { fNeutron_entryW.push_back(val); };
    void PushNeutronTallyEventAngleRelMuon(double val) {fNeutron_angle.push_back(val); };
    void PushNeutronTallyEventDistanceToMuonTrack(double val) {fNeutron_distance.push_back(val); };
    void IncrementNeutronTallyEventMultiplicity() { fNeutronEntryMultiplicity++;};

    //RECOIL TREE
    void FillRecoilTreeEvent();
    void ClearRecoilTreeEvent();
    void PushRecoilEventID(int val) { fRecoilEventID = val; };
    void PushRecoilEventPDG(int val) { fRecoilEventPDGCode.push_back(val); };
    void PushRecoilEventParentPDG(int val) { fRecoilEventParentPDGCode.push_back(val); };
    void PushRecoilEventEnergy(double val) { fRecoilEventEnergy.push_back(val); };
    void PushRecoilEventX(double val) { fRecoilEventX.push_back(val); };
    void PushRecoilEventY(double val) { fRecoilEventY.push_back(val); };
    void PushRecoilEventZ(double val) { fRecoilEventZ.push_back(val); };
    void PushRecoilEventU(double val) { fRecoilEventU.push_back(val); };
    void PushRecoilEventV(double val) { fRecoilEventV.push_back(val); };
    void PushRecoilEventW(double val) { fRecoilEventW.push_back(val); };
    void PushRecoilEventTime(double val) { fRecoilEventTime.push_back(val); };
    void PushRecoilEventCode(double val) { fRecoilEventCode.push_back(val); };
    void IncrementNRecoils() { fNRecoils++;};

    //Writing output
    void WriteVRMLGeometry(const G4String& vrmlFilename);

private:
    PaleoSimMessenger& fMessenger; 

    TFile* fFile = nullptr;
    TTree* fHeaderTree = nullptr;
    TTree* fPrimariesTree = nullptr;
    TTree* fMINTree = nullptr;
    TTree* fNeutronTallyTree = nullptr;
    TTree* fRecoilTree = nullptr;

    // Primary Tree variables
    int fPrimaryEventID = -1;
    std::vector<int> fPrimaryPdgID;
    std::vector<double> fPrimaryEnergy;
    std::vector<double> fPrimaryX, fPrimaryY, fPrimaryZ;
    std::vector<double> fPrimaryPx, fPrimaryPy, fPrimaryPz;
    // CUSTOM_GENERATOR_HOOK
    // Create vectors to be stored as branches in the primaries tree for your custom generator here
    //
    //Mei & Hime Muon generator (and MUTE except for Slant)
    double fPrimaryMuonTheta, fPrimaryMuonPhi, fPrimaryMuonSlant;
    // 
    // CRY
    double fCRYCoreX, fCRYCoreY, fCRYCoreZ;
    double fCRYCoreTheta, fCRYCorePhi;
    double fCRYTotalEnergy;

    //Muon-induced neutron tree
    int fMINEventID = -1;
    int fMINEventMultiplicity = 0;
    std::vector<double> fMINEventAngleRelMuon;
    std::vector<double> fMINEventEnergy;
    std::vector<double> fMINEventDistanceToMuonTrack;

    // Neutron Tally Tree variables (added for neutron tracking)
    int fNeutronTallyEventID = -1;
    int fNeutronEntryMultiplicity = 0; //How many neutrons entered the cavity for this event
    std::vector<double> fNeutron_entryEnergy;
    std::vector<double> fNeutron_entryX, fNeutron_entryY, fNeutron_entryZ;
    std::vector<double> fNeutron_entryU, fNeutron_entryV, fNeutron_entryW;
    std::vector<double> fNeutron_angle, fNeutron_distance;

    // Recoil Tree variables
    int fRecoilEventID = -1;
    std::vector<int> fRecoilEventPDGCode, fRecoilEventParentPDGCode;
    int fNRecoils = 0;
    std::vector<double> fRecoilEventEnergy;
    std::vector<double> fRecoilEventTime;
    std::vector<double> fRecoilEventX, fRecoilEventY, fRecoilEventZ;
    std::vector<double> fRecoilEventU, fRecoilEventV, fRecoilEventW;
    std::vector<double> fRecoilEventCode;
};

#endif
