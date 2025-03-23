#ifndef PALEOSIMMATERIALMANAGER_HH
#define PALEOSIMMATERIALMANAGER_HH

#include "G4Material.hh"
#include <map>
#include <string>

class PaleoSimMaterialManager {
public:
    PaleoSimMaterialManager();
    ~PaleoSimMaterialManager() = default;

    G4Material* GetMaterial(const std::string& name) const;

private:
    void DefineMaterials();
    std::map<std::string, G4Material*> materialMap;
};

#endif
