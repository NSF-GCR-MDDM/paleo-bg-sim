#ifndef PALEOSIMMATERIALMANAGER_HH
#define PALEOSIMMATERIALMANAGER_HH

#include "G4Material.hh"
#include <map>
#include <string>

class PaleoSimMaterialManager {
public:
    ~PaleoSimMaterialManager() = default;
    static PaleoSimMaterialManager* Instance();

    G4Material* GetMaterial(const std::string& name) const;
    bool HasMaterial(const std::string& name) const;
    void PrintKnownMaterials() const;
    
private:
    PaleoSimMaterialManager();

    void DefineMaterials();
    std::map<std::string, G4Material*> materialMap;
};

#endif
