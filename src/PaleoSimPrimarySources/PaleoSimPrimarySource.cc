#include "Randomize.hh"
#include "G4PhysicalConstants.hh"

#include "PaleoSimVolumeDefinition.hh"
#include "PaleoSimPrimarySources/PaleoSimPrimarySource.hh"

PaleoSimPrimarySource::PaleoSimPrimarySource(PaleoSimMessenger& messenger)
: fMessenger(messenger)
{
}

PaleoSimPrimarySource::~PaleoSimPrimarySource() {
}

void PaleoSimPrimarySource::InitializeSource() {
}

void PaleoSimPrimarySource::GeneratePrimaries(G4Event*) {
}


//Sample point on disk via MC.
G4ThreeVector PaleoSimPrimarySource::SamplePointOnDisk(double radius, const G4ThreeVector& center, const G4ThreeVector& axis) {
    G4ThreeVector n = axis.unit();
    //If almost completely oriented along z-axis, use z-axis
    G4ThreeVector temp = (std::fabs(n.z()) < 0.99999) ? G4ThreeVector(0,0,1) : G4ThreeVector(1,0,0);
    G4ThreeVector u = n.cross(temp).unit();
    G4ThreeVector v = n.cross(u).unit();
    
    double r = radius * std::sqrt(G4UniformRand());
    double theta = 2 * CLHEP::pi * G4UniformRand();
    double x_local = r * std::cos(theta);
    double y_local = r * std::sin(theta);

    // Transform to global coordinates
    return center + x_local * u + y_local * v;
}


//Gets random point on top of world volume. Requires world to be a cylinder or box for now
G4ThreeVector PaleoSimPrimarySource::SamplePointOnTopOfWorldVolume() {
  //Find the world volume
  const auto& volumes = fMessenger.GetVolumes();
  const PaleoSimVolumeDefinition* worldDef = nullptr;
  for (const auto* vol : volumes) {
      if (vol->parentName == "None") {
          worldDef = vol;
          break;
      }
  }

  //Generate a random point on top surface if it's a box or cylinder, error out otherwise
  G4ThreeVector localTop;
  if (worldDef->shape == "box") {
      G4double x = (G4UniformRand() - 0.5) * 2 * worldDef->halfLengths.x();
      G4double y = (G4UniformRand() - 0.5) * 2 * worldDef->halfLengths.y();
      G4double z = +worldDef->halfLengths.z();
      localTop = G4ThreeVector(x, y, z);
  }
  else if (worldDef->shape == "cylinder") {
      G4double r = worldDef->radius * std::sqrt(G4UniformRand());
      G4double theta = G4UniformRand() * 2 * CLHEP::pi;
      G4double x = r * std::cos(theta);
      G4double y = r * std::sin(theta);
      G4double z = worldDef->halfHeight;
      localTop = G4ThreeVector(x, y, z);
  }
  else {
      G4Exception("SamplePointOnTopOfWorldVolume", "UnsupportedShape", FatalException,
                  ("Top surface sampling not supported for shape: " + worldDef->shape).c_str());
  }

  return worldDef->absolutePosition + localTop;
}

//Checks if a point is on the top surface or not
G4bool PaleoSimPrimarySource::IsWithinTopSurface(const G4ThreeVector& point) {
  const auto& volumes = fMessenger.GetVolumes();
  const PaleoSimVolumeDefinition* worldDef = nullptr;
  for (const auto* vol : volumes) {
      if (vol->parentName == "None") {
          worldDef = vol;
          break;    
      }
  }

  if (worldDef->shape == "box") {
      if (std::abs(point.x()) <= worldDef->halfLengths.x() && std::abs(point.y()) <= worldDef->halfLengths.y()) {
        return true;
      }
  }
  else if (worldDef->shape == "cylinder") {
      double r2 = point.x()*point.x() + point.y()*point.y();
      if (r2 <= worldDef->radius * worldDef->radius) {
        return true;
      }
  }
  else {
      G4Exception("IsWithinTopSurface", "UnsupportedShape", FatalException,
                  ("Top surface sampling not supported for shape: " + worldDef->shape).c_str());
  }

  return false;
}