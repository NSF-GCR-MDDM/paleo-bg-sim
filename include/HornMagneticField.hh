#ifndef HornMagneticField_h
#define HornMagneticField_h 1

#include "G4MagneticField.hh"

class G4GenericMessenger;

class HornMagneticField : public G4MagneticField
{
    public:
        HornMagneticField();
        virtual ~HornMagneticField();
    
        virtual void GetFieldValue(const G4double point[4], double* bField ) const;
        
    private:
        G4GenericMessenger* fMessenger;
};

#endif
