#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "PaleoSimOutputManager.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Exception.hh"

#include "TF1.h"
#include <algorithm>

MiniBooNEBeamlinePrimaryGeneratorAction::MiniBooNEBeamlinePrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(), fGPS(new G4GeneralParticleSource())
{}

MiniBooNEBeamlinePrimaryGeneratorAction::~MiniBooNEBeamlinePrimaryGeneratorAction() {
    if (fGPS) delete fGPS;
    if (fMuonThetaDist) delete fMuonThetaDist;
    if (fMuonEnergyDist) delete fMuonEnergyDist;
}

void MiniBooNEBeamlinePrimaryGeneratorAction::SetSourceType(const G4String& sourceType) {
    fSourceType = sourceType;

    // List of valid sources
    fValidSourceTypes = {
        // CUSTOM_GENERATOR_HOOK
        // Add new generator names to the list above
		//
		//Mei & Hime muon generator
        "muonGenerator"
    };

    if (std::find(fValidSourceTypes.begin(), fValidSourceTypes.end(), fSourceType) == fValidSourceTypes.end()) {
        G4Exception("MiniBooNEBeamlinePrimaryGeneratorAction", "InvalidSource", FatalException,
                    ("Invalid source type: " + fSourceType).c_str());
    }
}

//TODO move intialization of the source into its own function somewhere else
void MiniBooNEBeamlinePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    // CUSTOM_GENERATOR_HOOK
    // Call initialization logic here if needed for new generators
	//
	//Mei & Hime muon generator
    if (fSourceType == "muonGenerator" && initializedSource==false) {
        InitializeMuons();
        initializedSource=true;
    }
	
    // CUSTOM_GENERATOR_HOOK
    // Add dispatch logic here for new generators
	//
	//Mei & Hime muon generator
    if (fSourceType == "muonGenerator") {
        GenerateMuonPrimaries(anEvent);
    }

}

// ---------------------------
// Output Tree Helpers
// ---------------------------

void MiniBooNEBeamlinePrimaryGeneratorAction::AddPrimaries(int pdgID,
	double energy,
	const G4ThreeVector& position,
	const G4ThreeVector& momentum,
	//CUSTOM_GENERATOR_HOOK
	//If you have custom output fields, add here
	//
	//Mei & Hime muon generator
	double theta,
	double phi,
	double slant)
{
	fPrimaryPDGIDs.push_back(pdgID);
	fPrimaryEnergies.push_back(energy);
	fPrimary_x.push_back(position.x());
	fPrimary_y.push_back(position.y());
	fPrimary_z.push_back(position.z());
	fPrimary_px.push_back(momentum.x());
	fPrimary_py.push_back(momentum.y());
	fPrimary_pz.push_back(momentum.z());

	// CUSTOM_GENERATOR_HOOK
	// If you have custom output fields, push_back here
	//
	//Mei & Hime muon generator
	fMuonTheta.push_back(theta);
	fMuonPhi.push_back(phi);
	fMuonSlant.push_back(slant);
}

// CUSTOM_GENERATOR_HOOK - YOUR METHODS GO HERE

// ---------------------------
// Mei & Hime Muon Generator
// ---------------------------
void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeMuons() {
    G4double h0_km = h0 / km;

    G4cout<<h0<<G4endl; 

    //Total, NOT differential!
    fMuonThetaDist = new TF1("fMuonThetaDist",
                          "( [0]*exp(-[2]/([3]*cos(x))) + [1]*exp(-[2]/([4]*cos(x))) ) / cos(x)",
                          0, CLHEP::pi / 2. - 0.001);
    //fMuonThetaDist = newTF1("fMuonThetaDist",
    // ""
    //)
    fMuonThetaDist->SetParameter(0, 0.0000086); //I1
    fMuonThetaDist->SetParameter(1, 0.00000044); //I2
    fMuonThetaDist->SetParameter(2, h0_km);
    fMuonThetaDist->SetParameter(3, 0.45); //1/lambda1
    fMuonThetaDist->SetParameter(4, 0.87); //1/lambda2
    fMuonThetaDist->SetNpx(1000);
    for (int i=0; i < 5; i++) {
        G4cout<<i<<","<<fMuonThetaDist->GetParameter(i)<<G4endl;
    }
    for (double i = 0; i < 3.1415/2.; i+=0.1) {
        G4cout<<i<<", "<<fMuonThetaDist->Eval(i)<<G4endl;
    }

    fMuonEnergyDist = new TF1("fMuonEnergyDist",
        "exp(-[0]*[1]*([2]-1)) * (x + [3]*(1-exp(-[0]*[1])))^(-[2])", 1, 3000);
    fMuonEnergyDist->SetParameter(0, 0.4);
    //Par 1 set after sampling theta and calculating slant depth
    fMuonEnergyDist->SetParameter(2, 3.77);
    fMuonEnergyDist->SetParameter(3, 693);
    fMuonEnergyDist->SetNpx(1000);
}

void MiniBooNEBeamlinePrimaryGeneratorAction::GenerateMuonPrimaries(G4Event* anEvent) {
    G4double h0_km = h0 / km;

    // Flux normalization (Mei & Hime Eq. 4, not used directly)
    //G4double I0 = 67.97e-6 * std::exp(-h0_km / 0.285) + 2.071e-6 * std::exp(-h0_km / 0.698);

    G4ParticleDefinition* muonDef = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    fGPS->SetParticleDefinition(muonDef);

    G4double theta = fMuonThetaDist->GetRandom();
    G4double phi = (2.0 * M_PI - 0.001) * G4UniformRand();

    G4double h_km = h0_km / std::cos(theta);
    fMuonEnergyDist->SetParameter(1, h_km);
    G4double E_GeV = fMuonEnergyDist->GetRandom();
    //G4double E_GeV = 1.0;
    fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E_GeV);

    /*
    G4cout << "[Sampled Muon]" << G4endl
           << "  Theta (zenith)     = " << theta / deg << " deg" << G4endl
           << "  Phi (azimuth)      = " << phi / deg << " deg" << G4endl
           << "  Slant depth        = " << h_km << " km.w.e" << G4endl
           << "  Energy             = " << E_GeV << " GeV" << G4endl;
    */
    
    G4ThreeVector position = SamplePointOnTopOfOverburden();
    G4ThreeVector direction(std::sin(theta) * std::cos(phi),
                            std::sin(theta) * std::sin(phi),
                            -std::cos(theta)); // downward
    fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);
    fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

    fGPS->GeneratePrimaryVertex(anEvent);

    // Record for output tree
	G4double mass = muonDef->GetPDGMass();
	G4double momentumMag = std::sqrt(E_GeV * E_GeV - mass * mass);
	G4ThreeVector momentum = direction * momentumMag;

    AddPrimaries(muonDef->GetPDGEncoding(), E_GeV, position, momentum,
                  theta, phi, h_km);

    
}

G4ThreeVector MiniBooNEBeamlinePrimaryGeneratorAction::SamplePointOnTopOfOverburden() const {
    auto* detector = static_cast<const MiniBooNEBeamlineConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    G4double sideLength = detector->GetOverburdenSideLength();
    G4double halfSideLength = sideLength / 2.0;

    G4double x = (G4UniformRand() - 0.5) * sideLength;
    G4double y = (G4UniformRand() - 0.5) * sideLength;
    G4double z = halfSideLength;

    return G4ThreeVector(x, y, z);
}

void MiniBooNEBeamlinePrimaryGeneratorAction::SetMuonEffectiveDepth(G4double depth) {
    h0 = depth;
}
