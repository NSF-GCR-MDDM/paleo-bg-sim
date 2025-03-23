#include "MiniBooNEBeamlinePrimaryGeneratorAction.hh"
#include "MiniBooNEBeamlineConstruction.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Exception.hh"
#include <algorithm>
#include "TF1.h"

MiniBooNEBeamlinePrimaryGeneratorAction::MiniBooNEBeamlinePrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(), fGPS(new G4GeneralParticleSource())
{

}


void MiniBooNEBeamlinePrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	if (fSourceType == "muonGenerator") {

		GenerateMuonPrimaries(anEvent);
	}
}

void MiniBooNEBeamlinePrimaryGeneratorAction::SetSourceType(const G4String& sourceType) {

    fSourceType = sourceType;

	//List of valid source types
    fValidSourceTypes = {"muonGenerator"};

	//Check if our source type is in our list
	if (std::find(fValidSourceTypes.begin(), fValidSourceTypes.end(), fSourceType) == fValidSourceTypes.end()) {
		G4Exception("MiniBooNEBeamlinePrimaryGeneratorAction", "InvalidSource", FatalException,
			("Invalid source type: " + fSourceType).c_str());
	}

	//Initialization
	if (fSourceType == "muonGenerator") {
		InitializeMuons();
	}
}

MiniBooNEBeamlinePrimaryGeneratorAction::~MiniBooNEBeamlinePrimaryGeneratorAction() {
    if (fGPS) delete fGPS;

	//Muon generator stuff
	if (fMuonThetaDist) delete fMuonThetaDist;
	if (fMuonEnergyDist) delete fMuonEnergyDist;
	if (fMuonPhiDist) delete fMuonPhiDist;
}

//Muon generator functions 
void MiniBooNEBeamlinePrimaryGeneratorAction::InitializeMuons() {
	G4double h0_km = h0/km;

	//Make a function of phis to sample - uniform from 0 to 2*pi
	fMuonPhiDist = new TF1("fMuonPhiDist","1",0,2*CLHEP::pi);

	//Make a function of thetas to sample - sec(theta) from 0 to pi/2
	fMuonThetaDist = new TF1("fMuonThetaDist","([0]*exp(-[1]*[2]/cos(x))+[3]*exp(-[4]*[2]/cos(x)))*sin(x)/cos(x)",0,CLHEP::pi/2.-0.01);
	fMuonThetaDist->SetParameter(0,0.0000086);
	fMuonThetaDist->SetParameter(1,0.00000047); //TODO: Check
	fMuonThetaDist->SetParameter(2,h0_km);
	fMuonThetaDist->SetParameter(3,1./0.45);
	fMuonThetaDist->SetParameter(4,1./0.87);


	//Make a function of energies to sample - Energy in GeV
	fMuonEnergyDist = new TF1("fMuonEnergyDist","exp(-[0]*[1]*([2]-1)) * (x - [3]*(1-exp(-[0]*[1])))^(-[2])",1,3000);
	fMuonEnergyDist->SetParameter(0,0.4); 
	//Skip 1 since we calculate it from the sampled theta
	fMuonEnergyDist->SetParameter(2,3.77);
	fMuonEnergyDist->SetParameter(3,693);

}

void MiniBooNEBeamlinePrimaryGeneratorAction::GenerateMuonPrimaries(G4Event* anEvent) {
	G4double h0_km = h0/km;

	//Calculate the normalized muon flux rate at this effective depth - Eq. 4 from Mei & Hime (2006)
	G4double I0 = 67.97e-6 * std::exp(-h0_km/0.285) + 2.071e-6*std::exp(-h0_km/0.698);

	//Set the particle (muon - Later we'll put in mu+/mu- in appropriate ratios)
    G4ParticleDefinition* muonDef = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    fGPS->SetParticleDefinition(muonDef);

	//Sample theta
	G4double theta = fMuonThetaDist->GetRandom();

	//Sample phi
	G4double phi = fMuonPhiDist->GetRandom();

	//Calculate the h slant depth & set it
	G4double h_km = h0_km / std::cos(theta); 
	fMuonEnergyDist->SetParameter(1,h_km);

	//Set the energy & set it - TODO: rest mass?
	G4double E_GeV = fMuonEnergyDist->GetRandom();
    fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E_GeV);


	//For debugging
	G4cout 
    << "[Sampled Muon]" << G4endl
    << "  Theta (zenith)     = " << theta / deg << " deg" << G4endl
    << "  Phi (azimuth)    = " << phi / deg << " deg" << G4endl
    << "  Slant depth    = " << h_km << " km.w.e" << G4endl
    << "  Energy         = " << E_GeV << " GeV" 
    << G4endl;


	//Sample the position & set it
    G4ThreeVector position = SamplePointOnTopOfOverburden();

	//Set the momentum direction
	G4ThreeVector direction(
        std::sin(theta) * std::cos(phi),
        std::sin(theta) * std::sin(phi),
        -std::cos(theta)  // downward
    );    
	fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(direction);

	//Generate the Primary vertex
    fGPS->GeneratePrimaryVertex(anEvent);
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