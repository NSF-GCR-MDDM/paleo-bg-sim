#include "PaleoSimMaterialManager.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include <algorithm> // for std::transform
#include "G4Exception.hh"

PaleoSimMaterialManager::PaleoSimMaterialManager() {
    DefineMaterials();
}

void PaleoSimMaterialManager::DefineMaterials() {
    auto nist = G4NistManager::Instance();

    G4Element* H = nist->FindOrBuildElement("H");		   // Hydrogen
    G4Element* B = nist->FindOrBuildElement("B");		   // Boron
    G4Element* Li = nist->FindOrBuildElement("Li");		   // Li
    G4Element* C = nist->FindOrBuildElement("C");		   // Carbon
    G4Element* O = nist->FindOrBuildElement("O");		   // Oxygen
    G4Element* F = nist->FindOrBuildElement("F");		   // Fluorine
    G4Element* Na = nist->FindOrBuildElement("Na");		   // Sodium
    G4Element* Mg = nist->FindOrBuildElement("Mg");		   // Magnesium
    G4Element* Al = nist->FindOrBuildElement("Al");		   // Aluminum
    G4Element* Si = nist->FindOrBuildElement("Si");		   // Silicon
    G4Element* K = nist->FindOrBuildElement("K");		   // Potassium
    G4Element* Ca = nist->FindOrBuildElement("Ca");		   // Calcium
    G4Element* Mn = nist->FindOrBuildElement("Mn");		   // Manganese
    G4Element* Fe = nist->FindOrBuildElement("Fe");		   // Iron
    G4Element* Ti = nist->FindOrBuildElement("Ti");		   // Titanium

    //Begin Norite
    G4Material* Norite = new G4Material("Norite", 2.894*g/cm3, 12);
    // Add elements to the material (fractions are by mass)
    Norite->AddElement(H, 0.001425);  // 0.15% Silicon
    Norite->AddElement(C, 0.000325);  // 0.04% Carbon
    Norite->AddElement(O, 0.459925);  // 46.0% Oxygen
    Norite->AddElement(Na, 0.021925); // 2.2% Sodium
    Norite->AddElement(Mg, 0.032925); // 3.3% Magnesium
    Norite->AddElement(Al, 0.089925); // 9.0% Aluminum
    Norite->AddElement(Si, 0.261925); // 26.2% Sodium
    Norite->AddElement(K, 0.011925);  // 1.2% Potassium
    Norite->AddElement(Ca, 0.051925); // 5.2% Calcium
    Norite->AddElement(Mn, 0.000925); // 0.1% Manganese
    Norite->AddElement(Fe, 0.061925); // 6.2% Iron
    Norite->AddElement(Ti, 0.004925); // 0.5% Titanium

    G4Material* Borated_HDPE = new G4Material("Borated_HDPE", 1.04*g/cm3, 3);
    Borated_HDPE->AddElement(B, 0.0500);  // 5% Boron
    Borated_HDPE->AddElement(H, 0.1365);  // 13.66% Air
    Borated_HDPE->AddElement(C, 0.8135 ); // 81.3% carbon 

    G4Material* LiF = new G4Material("LiF", 2.635*g/cm3, 2);
    LiF->AddElement(Li, 0.268);  // 
    LiF->AddElement(F, 0.732);  // 

    //https://pdg.lbl.gov/2024/AtomicNuclearProperties/HTML/standard_rock.html
    G4Material* StandardRock = new G4Material("StandardRock", 11, 22, 2.65*g/cm3);

    //Add to map - MUST BE LOWER CASE
    materialMap["air"] = nist->FindOrBuildMaterial("G4_AIR");
    materialMap["norite"] = Norite;
    materialMap["concrete"] = nist->FindOrBuildMaterial("G4_CONCRETE");  
    materialMap["borated_hdpe"] = Borated_HDPE; 
    materialMap["stainless_steel"] = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");  
    materialMap["lead"] = nist->FindOrBuildMaterial("G4_Pb");  
    materialMap["lif"] = LiF;
    materialMap["standard_rock"] = StandardRock;
    materialMap["water"] = nist->FindOrBuildMaterial("G4_WATER");
    materialMap["concrete"] = nist->FindOrBuildMaterial("G4_CONCRETE");
    materialMap["steel"] = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
}


// Helper function to convert strings to lowercase
static std::string ToLower(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

G4Material* PaleoSimMaterialManager::GetMaterial(const std::string& name) const {
    std::string lowerName = ToLower(name);

    auto it = materialMap.find(lowerName);
    if (it != materialMap.end()) {
        return it->second;
    } else {
        G4String msg = "Material '" + name + "' not defined!";
        G4Exception("PaleoSimMaterialManager::GetMaterial", "MaterialNotFound",
                    FatalException, msg);
        return nullptr;  // Unreachable, but suppresses warnings
    }
}

bool PaleoSimMaterialManager::HasMaterial(const std::string& name) const {
    std::string lowerName = ToLower(name);
    return materialMap.find(lowerName) != materialMap.end();
}

PaleoSimMaterialManager* PaleoSimMaterialManager::Instance() {
    static PaleoSimMaterialManager instance;
    return &instance;
}

void PaleoSimMaterialManager::PrintKnownMaterials() const {
    G4cerr << "Available materials:\n";
    for (const auto& pair : materialMap) {
        G4cerr << "  " << pair.first << " (G4 name: " 
               << pair.second->GetName() << ")\n";
    }
}