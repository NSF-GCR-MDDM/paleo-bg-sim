#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"

#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimPrimarySources/PaleoSimCrySource.hh"

PaleoSimCrySource::PaleoSimCrySource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimCrySource::~PaleoSimCrySource() {
  if (cryFileLoaded) {
    cryFile->Close();
    delete cryFile;
  }
}

void PaleoSimCrySource::InitializeSource() {
  //Load file
  G4String filename = fMessenger.GetCRYFilename();

  //Check and open root file
  std::ifstream testFile(filename);
  if (!testFile.good()) {
      G4Exception("InitializeCRY", "CRY001", FatalException,
                  ("Cannot open CRY file: " + filename).c_str());
  }
  testFile.close();
  cryFile = TFile::Open(filename);
  if (!cryFile || cryFile->IsZombie()) {
      G4Exception("InitializeCRY", "CRY002", FatalException,
                  ("Failed to open CRY ROOT file: " + filename).c_str());
  }

  //Read in tree
  cryTree = dynamic_cast<TTree*>(cryFile->Get("cryTree"));
  if (!cryTree) {
      G4Exception("InitializeCRY", "CRY003", FatalException,
                  "Tree 'cryTree' not found in ROOT file.");
  }
  //cryTree->SetBranchStatus("*", 0); 
  cryTree->SetBranchAddress("pdgCode",    &cry_pdgcode);
  cryTree->SetBranchAddress("energy_MeV", &cry_energy);
  cryTree->SetBranchAddress("u",          &cry_u);
  cryTree->SetBranchAddress("v",          &cry_v);
  cryTree->SetBranchAddress("w",          &cry_w);
  cryTree->SetBranchAddress("x_mm",       &cry_x);
  cryTree->SetBranchAddress("y_mm",       &cry_y);

  nCryEntries = cryTree->GetEntries();
  std::cout<<"Number of CRY particles to sample is "<<nCryEntries<<std::endl;
  cryFileLoaded = true;

  //Preload into memory
  for (Long64_t i = 0; i < nCryEntries; ++i) {
      cryTree->GetEntry(i); // fills cry_* branch buffers for one event

      all_cry_pdgcodes.push_back(*cry_pdgcode); // copy vector
      all_cry_energy.push_back(*cry_energy);
      all_cry_u.push_back(*cry_u);
      all_cry_v.push_back(*cry_v);
      all_cry_w.push_back(*cry_w);
      all_cry_x.push_back(*cry_x);
      all_cry_y.push_back(*cry_y);
      if (i%1000000 ==0 ) {
          std::cout<<"On entry "<<i<<" of "<<nCryEntries<<std::endl;
      }
  }

  //Get header info
  TTree * headerTree = dynamic_cast<TTree*>(cryFile->Get("headerTree"));
  float altitude,latitude,norm;
  headerTree->SetBranchAddress("altitude", &altitude);
  headerTree->SetBranchAddress("latitude",          &latitude);
  headerTree->SetBranchAddress("primaries_per_cm2_per_s",          &norm);

  headerTree->GetEvent(0);
  fMessenger.SetCRYAltitude(static_cast<double>(altitude));
  fMessenger.SetCRYLatitude(static_cast<double>(latitude));
  fMessenger.SetCRYNorm(static_cast<double>(norm));
  delete headerTree;
}


void PaleoSimCrySource::GeneratePrimaries(G4Event* anEvent) {

  Long64_t entry = G4RandFlat::shootInt(nCryEntries);
    
  cry_pdgcode = &all_cry_pdgcodes.at(entry);
  cry_energy  = &all_cry_energy.at(entry);
  cry_u       = &all_cry_u.at(entry);
  cry_v       = &all_cry_v.at(entry);
  cry_w       = &all_cry_w.at(entry);
  cry_x       = &all_cry_x.at(entry);
  cry_y       = &all_cry_y.at(entry);


  // Sample a position on the top of the world volume
  G4ThreeVector basePosition = SamplePointOnTopOfWorldVolume();

  //Load all particles into vertices
  for (size_t i = 0; i < cry_pdgcode->size(); i++) {
    int pdgCode = cry_pdgcode->at(i);
    G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
    if (!particleDef) {
        G4cerr << "Unknown PDG code in CRY: " << pdgCode << G4endl;
        continue;
    }

    double Ekin = cry_energy->at(i) * MeV;
    double mass = particleDef->GetPDGMass();
    double Etot = Ekin + mass;
    double p = std::sqrt(Etot * Etot - mass * mass);

    G4ThreeVector position = basePosition + G4ThreeVector(cry_x->at(i) * mm, cry_y->at(i) * mm, 0);
    if (IsWithinTopSurface(position)) {
      G4ThreeVector direction(cry_u->at(i), cry_v->at(i), cry_w->at(i));
      G4ThreeVector momentum = direction * p;

      G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef,
                                                        momentum.x(),
                                                        momentum.y(),
                                                        momentum.z());

      G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
      vertex->SetPrimary(primary);
      anEvent->AddPrimaryVertex(vertex);
    }
  }

  //Store primary info in userEventInfo
  auto* info = new PaleoSimUserEventInformation();
  // Compute total energy
  double totalEnergy = 0.0;
  G4ThreeVector weightedDirection(0,0,0);
  for (size_t i = 0; i < cry_pdgcode->size(); ++i) {
      double energy = cry_energy->at(i);  // in MeV
      totalEnergy += energy;
      weightedDirection += G4ThreeVector(cry_u->at(i), cry_v->at(i), cry_w->at(i)) * energy;
  }
  G4ThreeVector dir = weightedDirection.unit();
  info->CRYCoreTheta = dir.theta();  // radians
  info->CRYCorePhi   = dir.phi();    // radians
  info->CRYTotalEnergy = totalEnergy;  // MeV
  info->CRYCorePosition = basePosition;
  anEvent->SetUserInformation(info); //G4 takes ownership, no need to delete
}
