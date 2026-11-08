#include "PaleoSimPrimarySources/PaleoSimRadioactiveParentSource.hh"
#include "PaleoSimMessenger.hh"
#include "PaleoSimVolumeDefinition.hh"

#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "G4Exception.hh"

PaleoSimRadioactiveParentSource::PaleoSimRadioactiveParentSource(PaleoSimMessenger& messenger)
    : PaleoSimPrimarySource(messenger)
{
}

PaleoSimRadioactiveParentSource::~PaleoSimRadioactiveParentSource() {
}

void PaleoSimRadioactiveParentSource::InitializeSource() {
    // 1. Resolve source volume definition
    G4String volumeName = fMessenger.GetRadioactiveParentSourceVolumeName();
    fSourceVolumeDefinition = fMessenger.GetVolumeByName(volumeName);
    if (!fSourceVolumeDefinition) {
        G4Exception("PaleoSimRadiogenicSource::InitializeSource", "RadParentSource001", FatalException,
                    ("PaleoSimVolumeDefinition '" + volumeName + "' not found!").c_str());
    }

    // 2. Fetch parent ground-state nucleus from G4IonTable 
    // Supports 238U, 235U, and 232Th for now
    G4String isotope = fMessenger.GetRadioactiveParentSourceIsotope();
    G4IonTable* ionTable = G4IonTable::GetIonTable();

    if (isotope == "238U" || isotope == "u238" || isotope == "U238") {
        fParentIon = ionTable->GetIon(92, 238, 0.0);
    } else if (isotope == "232Th" || isotope == "th232" || isotope == "Th232") {
        fParentIon = ionTable->GetIon(90, 232, 0.0);
    } else if (isotope == "235U" || isotope == "u235" || isotope == "U235") {
        fParentIon = ionTable->GetIon(92, 235, 0.0);
    } else {
        G4Exception("PaleoSimRadiogenicSource::InitializeSource", "RadParentSource002", FatalException,
                    ("Unsupported radiogenic isotope: " + isotope).c_str());
    }

    if (!fParentIon) {
        G4Exception("PaleoSimRadiogenicSource::InitializeSource", "RadParentSource003", FatalException,
                    ("Failed to construct ion definition for: " + isotope).c_str());
    }
}

void PaleoSimRadioactiveParentSource::GeneratePrimaries(G4Event* anEvent) {
    // 1. Sample uniform random 3D position inside the crystal
    G4ThreeVector position = fSourceVolumeDefinition->GenerateRandomPointInside();

    // 2. Create primary parent at rest (0 Kinetic Energy, 0 Momentum, Time = 0.0)
    auto* primary = new G4PrimaryParticle(fParentIon, 0.0, 0.0, 0.0);
    primary->SetMass(fParentIon->GetPDGMass());

    // 3. Create primary vertex and attach to event
    auto* vertex = new G4PrimaryVertex(position, 0.0 * ns);
    vertex->SetPrimary(primary);
    anEvent->AddPrimaryVertex(vertex);
}