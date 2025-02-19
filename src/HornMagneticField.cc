#include "HornMagneticField.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include <iomanip>

HornMagneticField::HornMagneticField()
: G4MagneticField(),
  fMessenger(0)
{}

HornMagneticField::~HornMagneticField()
{
    delete fMessenger;
}

void HornMagneticField::GetFieldValue(const G4double point[4], double* field) const
{
  //Mode is 1.0 for neutrino mode and -1.0 for antineutrino mode.
  double mode = -1.0;
  //G4cout << "IN MAGNETIC FIELD!" << G4endl;
  G4double fieldStrengthConst = 1.5*tesla;
  G4double innerHornRadius = 23.0*mm;
  field[0] = 0.0; field[1] = 0.0; field[2] = 0.0;
  field[3] = 0.0; field[4] = 0.0; field[5] = 0.0;
  
  //Error checking that is apparently needed in Geant
  if (point[0] != point[0]) return;

//  G4cout << "=================================================" << G4endl;
  G4double x = point[0]*mm;
  G4double y = point[1]*mm;
//  G4cout << std::setprecision(5) << "x: " << x/cm << " (cm) y: " << y/cm << " (cm)" << G4endl;
  
  //Make sure we are in the cavity region
  G4double radius = sqrt(x*x + y*y);
//  G4cout << std::setprecision(5) << "r: " << radius/cm << " (cm)" << G4endl;
  if (radius > 299.0 || radius < 23.0) return;
  //The magnetic field is in the phi direction
  G4double scaleX = -y/radius;
  G4double scaleY = x/radius;
  G4double fieldMag = fieldStrengthConst*(innerHornRadius/radius);
//  G4cout << std::setprecision(5) << "scaleX: " << scaleX << " scaleY: " <<  scaleY << " fieldMag: " << fieldMag/tesla << " (T)" << G4endl;
  field[0] = scaleX*fieldMag*mode;
  field[1] = scaleY*fieldMag*mode;
//  G4cout << std::setprecision(5) << "field[0]: " << field[0]/tesla << " (T) field[1]: " << field[1]/tesla << " (T)" << G4endl;
}
