#ifndef PALEO_SIM_MESSENGER_HH
#define PALEO_SIM_MESSENGER_HH

#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "globals.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"

class PaleoSimMessenger : public G4UImessenger {
public:
    PaleoSimMessenger();
    ~PaleoSimMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

    //Getters for the macro commands
    G4double GetUserOverburdenSideLength() const { return fOverburdenSideLength; };
    G4String GetUserOverburdenMaterial() const { return fOverburdenMaterial; };
    G4double GetAirCavitySideLength() const { return fAirCavitySideLength; };
    G4double GetTargetSideLength() const { return fTargetSideLength; };
    G4String GetTargetMaterial() const { return fTargetMaterial; };
    
    //Output
    G4String GetOutputPath() const { return fOutputFile; };
    G4bool GetUserPrimariesTreeOutputStatus() const { return fUserPrimariesTreeOutputStatus; };
    G4bool GetUserNeutronTallyTreeOutputStatus() const { return fUserNeutronTallyTreeOutputStatus; };

    //Generator
    vector<G4StrinG> GetValidSourceTypes() const { return fValidSourceTypes; };
    G4String GetSourceType() const {return fSourceType;};
    G4int GetNPS() const { return fNPS; };

    //Add your own generator commands here
    //CUSTOM_GENERATOR_HOOK
    //
    //Mei & Hime muon generator
    G4double GetMuonEffectiveDepth() const { return fMuonEffectiveDepth;};

private:
    G4UIdirectory* fGeomDirectory;
    G4UIcmdWithADoubleAndUnit* fOverburdenSizeCmd;  
    G4UIcmdWithAString* fOverburdenMaterialCmd;  
    G4UIcmdWithADoubleAndUnit* fAirCavitySizeCmd;  
    G4UIcmdWithADoubleAndUnit* fTargetSizeCmd;
    G4UIcmdWithAString* fTargetMaterialCmd;

    //
    G4double fOverburdenSideLength = 20 * m;
    G4String fOverburdenMaterial = "Norite";
    G4double fAirCavitySideLength = 0 * m;
    G4double fTargetSideLength = 1 * cm;
    G4String fTargetMaterial = "Norite";

    // Output configuration
    G4UIdirectory* fOutputDirectory;
    G4UIcmdWithAString* fSetOutputFileCmd;
    G4UIcmdWithABool* fUserPrimariesTreeStatusCmd;
    G4UIcmdWithABool* fUserNeutronTallyTreeStatusCmd;

    G4String fOutputFile = "outputFiles/output.root";
    G4bool fUserPrimariesTreeOutputStatus = true;
    G4bool fUserNeutronTallyTreeOutputStatus = false;

    //Generator general
    G4UIdirectory* fGeneratorDirectory;
    G4UIcmdWithAString* fSourceTypeCmd;
    G4UIcmdWithAnInteger* fNPSCmd;

    //Source types:
    // CUSTOM_GENERATOR_HOOK
    // Add new generator names to the list below
    vector<G4String> fValidSourceTypes = {
          "muonGenerator", //Mei & Hime muon generator, with TF1s
          "muonGeneratorC++" //Equivalent generator but with C++ instead of TF1s (from Alex)
      };}
    
    // CUSTOM_GENERATOR_HOOK
    // Implement your own generator variables/functions here
    //
    // "muonGenerator"
    G4String fSourceType = "muonGenerator";
    int fNPS = 100;
    G4UIcmdWithADoubleAndUnit* fMuonEffectiveDepthCmd;
    G4double fMuonEffectiveDepth = 6 * km;
    //
    // "muonGeneratorC++" (from Alex)
    void InitializeAngularDistribution(); // Initializes the angular distribution
    void InitializeEnergyIntervals(); // Initializes e_intervals
	  std::vector<double> GetEnergyDistribution(double theta); // Gets energy distribution from angle
	  // Samples energy from the distribution
    double SampleCDF(std::vector<double> cdf, std::vector<double> intervals);
    // Data members for energy/angular distributions
    std::vector<double> e_intervals; // Energy intervals
    std::vector<double> theta_intervals; // Angle intervals
    std::vector<double> theta_cdf; // Cumulative distribution function for angles
	  std::map<double, std::vector<double> > e_cdfs;


};

#endif

