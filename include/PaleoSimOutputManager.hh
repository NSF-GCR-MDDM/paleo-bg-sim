#ifndef PALEOSIMOUTPUTMANAGER_HH
#define PALEOSIMOUTPUTMANAGER_HH

#include <vector>
#include <string>

#include "globals.hh"
#include "PaleoSimMessenger.hh"

class PaleoSimOutputManager {
public:
    //Constructor/destructor
    PaleoSimOutputManager(PaleoSimMessenger& messenger);
    ~PaleoSimOutputManager() = default;

    //Lifecycle
    void Book();
    void BeginOfRun();
    void EndOfRun();

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
    // If you want to add more columns to the primaries ntuple, write setters here
    //
    // Mei & Hime muon generator
    void SetPrimaryMuonTheta(double val) { fPrimaryMuonTheta = val; };
    void SetPrimaryMuonPhi(double val) { fPrimaryMuonPhi = val; };
    void SetPrimaryMuonSlant(double val) { fPrimaryMuonSlant = val; };

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
    void PushNeutronTallyEventAngleRelMuon(double val) { fNeutron_angle.push_back(val); };
    void PushNeutronTallyEventDistanceToMuonTrack(double val) { fNeutron_distance.push_back(val); };
    void IncrementNeutronTallyEventMultiplicity() { fNeutronEntryMultiplicity++; };
    void PushNeutronTallyVolumeNumber(int val) { fNeutronTallyVolumeNumbers.push_back(val); };
    void PushPrevNeutronTallyVolumeNumber(int val) { fPrevNeutronTallyVolumeNumbers.push_back(val); };

    //SECONDARY CAPTURE TREE
    void FillSecondaryCaptureTreeEvent();
    void ClearSecondaryCaptureTreeEvent();
    void PushSecondaryCaptureEventID(int val) { fSecondaryCaptureEventID = val; };
    void PushSecondaryCaptureEventEntryPDG(int val) { fSecondary_entryPDG.push_back(val); };
    void PushSecondaryCaptureEventEntryEnergy(double val) { fSecondary_entryEnergy.push_back(val); };
    void PushSecondaryCaptureEventEntryX(double val) { fSecondary_entryX.push_back(val); };
    void PushSecondaryCaptureEventEntryY(double val) { fSecondary_entryY.push_back(val); };
    void PushSecondaryCaptureEventEntryZ(double val) { fSecondary_entryZ.push_back(val); };
    void PushSecondaryCaptureEventEntryU(double val) { fSecondary_entryU.push_back(val); };
    void PushSecondaryCaptureEventEntryV(double val) { fSecondary_entryV.push_back(val); };
    void PushSecondaryCaptureEventEntryW(double val) { fSecondary_entryW.push_back(val); };
    void PushSecondaryCaptureEventCreationZ(double val) { fSecondary_creationZ.push_back(val); };

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
    void IncrementNRecoils() { fNRecoils++; };
    void PushRecoilVolumeNumber(int val) { fRecoilVolumeNumbers.push_back(val); };

    //Writing output
    void WriteVRMLGeometry(const G4String& vrmlFilename);

private:
    //Ntuple booking
    void BookHeader();
    void BookGeometry();
    void BookPrimaries();
    void BookMIN();
    void BookNeutronTally();
    void BookSecondaryCapture();
    void BookRecoil();

    //Run-level ntuples
    void FillRunLevelNtuples();

    PaleoSimMessenger& fMessenger;
    G4bool fMergeNtuples = false;

    //Ntuple IDs
    G4int fHeaderID = -1;
    G4int fGeometryID = -1;
    G4int fPrimariesID = -1;
    G4int fMINID = -1;
    G4int fNeutronTallyID = -1;
    G4int fSecondaryCaptureID = -1;
    G4int fRecoilID = -1;

    //Header scalar column IDs
    G4int fHeaderNpsCol = -1;
    G4int fHeaderSourceTypeCol = -1;

    //Mei & Hime header columns
    G4int fHeaderMeiHimeEffectiveDepthCol = -1;
    G4int fHeaderMeiHimeFluxNormalizationCol = -1;

    //CRY header columns
    G4int fHeaderCRYAltitudeCol = -1;
    G4int fHeaderCRYLatitudeCol = -1;
    G4int fHeaderCRYNormCol = -1;

    //SCT header columns
    G4int fHeaderSCTCapturedParticlesCol = -1;

    //MUTE header columns
    G4int fHeaderMuteFluxNormalizationCol = -1;

    //Geometry scalar column IDs
    G4int fGeometryNameCol = -1;
    G4int fGeometryShapeCol = -1;
    G4int fGeometryParentCol = -1;
    G4int fGeometryMaterialCol = -1;
    G4int fGeometryNumberCol = -1;
    G4int fGeometryAbsXCol = -1;
    G4int fGeometryAbsYCol = -1;
    G4int fGeometryAbsZCol = -1;

    //Primary scalar column IDs
    G4int fPrimaryEventIDCol = -1;
    G4int fPrimaryMuonThetaCol = -1;
    G4int fPrimaryMuonPhiCol = -1;
    G4int fPrimaryMuonSlantCol = -1;

    //Muon-induced neutron scalar column IDs
    G4int fMINEventIDCol = -1;
    G4int fMINMultiplicityCol = -1;

    //Neutron tally scalar column IDs
    G4int fNeutronTallyEventIDCol = -1;
    G4int fNeutronTallyMultiplicityCol = -1;

    //Secondary capture scalar column IDs
    G4int fSecondaryCaptureEventIDCol = -1;

    //Recoil scalar column IDs
    G4int fRecoilEventIDCol = -1;
    G4int fRecoilNRecoilsCol = -1;

    //Geometry ntuple buffers
    std::vector<double> fGeomXs;
    std::vector<double> fGeomYs;
    std::vector<double> fGeomZs;

    //Primary Tree variables
    int fPrimaryEventID = -1;
    std::vector<int> fPrimaryPdgID;
    std::vector<double> fPrimaryEnergy;
    std::vector<double> fPrimaryX, fPrimaryY, fPrimaryZ;
    std::vector<double> fPrimaryPx, fPrimaryPy, fPrimaryPz;
    // CUSTOM_GENERATOR_HOOK
    // Create vectors to be stored as branches in the primaries tree for your custom generator here
    //
    //Mei & Hime Muon generator (and MUTE except for Slant)
    double fPrimaryMuonTheta = 0.;
    double fPrimaryMuonPhi = 0.;
    double fPrimaryMuonSlant = 0.;

    //Muon-induced neutron tree
    int fMINEventID = -1;
    int fMINEventMultiplicity = 0;
    std::vector<double> fMINEventAngleRelMuon;
    std::vector<double> fMINEventEnergy;
    std::vector<double> fMINEventDistanceToMuonTrack;

    //Neutron Tally Tree variables
    int fNeutronTallyEventID = -1;
    int fNeutronEntryMultiplicity = 0;
    std::vector<double> fNeutron_entryEnergy;
    std::vector<double> fNeutron_entryX, fNeutron_entryY, fNeutron_entryZ;
    std::vector<double> fNeutron_entryU, fNeutron_entryV, fNeutron_entryW;
    std::vector<double> fNeutron_angle, fNeutron_distance;
    std::vector<int> fNeutronTallyVolumeNumbers;
    std::vector<int> fPrevNeutronTallyVolumeNumbers;

    //Secondary capture tree variables
    int fSecondaryCaptureEventID = -1;
    std::vector<int> fSecondary_entryPDG;
    std::vector<double> fSecondary_entryEnergy;
    std::vector<double> fSecondary_entryX, fSecondary_entryY, fSecondary_entryZ;
    std::vector<double> fSecondary_entryU, fSecondary_entryV, fSecondary_entryW;
    std::vector<double> fSecondary_creationZ;

    //Recoil Tree variables
    int fRecoilEventID = -1;
    std::vector<int> fRecoilEventPDGCode, fRecoilEventParentPDGCode;
    int fNRecoils = 0;
    std::vector<double> fRecoilEventEnergy;
    std::vector<double> fRecoilEventTime;
    std::vector<double> fRecoilEventX, fRecoilEventY, fRecoilEventZ;
    std::vector<double> fRecoilEventU, fRecoilEventV, fRecoilEventW;
    std::vector<double> fRecoilEventCode;
    std::vector<int> fRecoilVolumeNumbers;
};

#endif