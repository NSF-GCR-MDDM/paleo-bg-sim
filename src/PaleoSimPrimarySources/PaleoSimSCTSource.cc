#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RandomDirection.hh"

#include "PaleoSimUserEventInformation.hh"
#include "PaleoSimPrimarySources/PaleoSimSCTSource.hh"

PaleoSimSCTSource::PaleoSimSCTSource(PaleoSimMessenger& messenger)
: PaleoSimPrimarySource(messenger)
{
}

PaleoSimSCTSource::~PaleoSimSCTSource() {
  if (SCTFileLoaded) {
    SCTFile->Close();
    delete SCTFile;
  }
}

void PaleoSimSCTSource::InitializeSource() {
  //Load file
  G4String filename = fMessenger.GetSCTFilename();
  G4String userBootstrap = fMessenger.GetSCTBootstrap();

  //Check and open root file
  std::ifstream testFile(filename);
  if (!testFile.good()) {
    G4Exception("InitializeSCTGenerator", "SCTGenerator001", FatalException,
                ("Cannot open SCT file: " + filename).c_str());
  }
  testFile.close();
  SCTFile = TFile::Open(filename);
  if (!SCTFile || SCTFile->IsZombie()) {
    G4Exception("InitializeSCTGenerator", "SCTGenerator002", FatalException,
                ("Failed to open SCT ROOT file: " + filename).c_str());
  }
  
  //Check bootstrapping method
  if (userBootstrap=="standard" || userBootstrap=="rrs") {
    SCTBootstrap = userBootstrap;
    std::cout<<"Bootstrapping method set to: "<<SCTBootstrap<<std::endl;
  }
  else {
    SCTBootstrap = "standard";
    std::cout<<"Invalid bootstrapping method; defaulting to standard"<<std::endl;
  }
  //Read in tree
  SCTTree = dynamic_cast<TTree*>(SCTFile->Get("secondaryCaptureTree"));
  if (!SCTTree) {
    G4Exception("InitializeSCTGenerator", "SCTGenerator003", FatalException,
                "Tree 'secondaryCaptureTree' not found in ROOT file.");
  }
  SCTTree->SetBranchAddress("pdgCode",    &sct_pdgcode);
  SCTTree->SetBranchAddress("pdgCode",    &sct_pdgcode);
  SCTTree->SetBranchAddress("energy",     &sct_energy);
  SCTTree->SetBranchAddress("entry_u",    &sct_u);
  SCTTree->SetBranchAddress("entry_v",    &sct_v);
  SCTTree->SetBranchAddress("entry_w",    &sct_w);
  SCTTree->SetBranchAddress("entry_x",    &sct_x);
  SCTTree->SetBranchAddress("entry_y",    &sct_y);
  
  nSCTEntries = SCTTree->GetEntries();
  std::cout<<"Number of captured events to sample is "<<nSCTEntries<<std::endl;
  SCTFileLoaded = true;

  G4ThreeVector randomPosition = SamplePointOnTopOfWorldVolume();
  G4double overburdenZ = randomPosition.z();

  //Preload into memory
  if (SCTBootstrap=="standard") {
    for (Long64_t i = 0; i < nSCTEntries; ++i) {
      SCTTree->GetEntry(i); 
  
      if (i%1000000 ==0 ) {
        std::cout<<"On entry "<<i<<" of "<<nSCTEntries<<std::endl;
      }
  
      if (sct_pdgcode->empty()) {
        continue;
      }
      all_sct_pdgcodes.push_back(*sct_pdgcode); // copy vector
      all_sct_energy.push_back(*sct_energy);
      all_sct_u.push_back(*sct_u);
      all_sct_v.push_back(*sct_v);
      all_sct_w.push_back(*sct_w);
      all_sct_x.push_back(*sct_x);
      all_sct_y.push_back(*sct_y);
    }
  }
  else if (SCTBootstrap=="rrs") {
    for (Long64_t i = 0; i < nSCTEntries; ++i) {
      SCTTree->GetEntry(i); 
  
      if (i%1000000 ==0 ) {
        std::cout<<"On entry "<<i<<" of "<<nSCTEntries<<std::endl;
      }
  
      if (sct_pdgcode->empty()) {
        continue;
      }

      G4ThreeVector entryPosition = G4ThreeVector(sct_x->at(0), sct_y->at(0), overburdenZ);
      if (!IsWithinTopSurface(entryPosition)) {
        continue;
      }
      all_sct_pdgcodes.push_back(*sct_pdgcode); // copy vector
      all_sct_energy.push_back(*sct_energy);
      all_sct_u.push_back(*sct_u);
      all_sct_v.push_back(*sct_v);
      all_sct_w.push_back(*sct_w);
      all_sct_x.push_back(*sct_x);
      all_sct_y.push_back(*sct_y);
    }
  }

  nSCTEntries = all_sct_pdgcodes.size();
  std::cout << "Actual pool size for sampling after filtering out empty entries: " << nSCTEntries << std::endl;

  //Get header info
  TTree * headerTree = dynamic_cast<TTree*>(SCTFile->Get("headerTree"));
  double norm;
  headerTree->SetBranchAddress("meiHimeFluxNormalization_per_cm2_per_s", &norm); // CHANGE LATER
  headerTree->GetEvent(0);
  fMessenger.SetSCTNorm(static_cast<double>(norm));
  fMessenger.SetSCTCapturedParticles(static_cast<int>(nSCTEntries));
  delete headerTree;
}

void PaleoSimSCTSource::GeneratePrimaries(G4Event* anEvent) {
  //Get random event
  Long64_t entry = G4RandFlat::shootInt(nSCTEntries);
      
  sct_pdgcode = &all_sct_pdgcodes.at(entry);
  sct_energy  = &all_sct_energy.at(entry);
  sct_u       = &all_sct_u.at(entry);
  sct_v       = &all_sct_v.at(entry);
  sct_w       = &all_sct_w.at(entry);
  sct_x       = &all_sct_x.at(entry);
  sct_y       = &all_sct_y.at(entry);

  if (SCTBootstrap=="standard") {
    // Sample a position on the top of the world volume
    G4ThreeVector randomPosition = SamplePointOnTopOfWorldVolume();
    G4double globalX = randomPosition.x() * mm;
    G4double globalY = randomPosition.y() * mm;
    G4double globalZ = randomPosition.z() * mm;
    // To handle local spacing for multiparticle events
    G4double anchorParticleX = sct_x->at(0) * mm;
    G4double anchorParticleY = sct_y->at(0) * mm;

    //Load all particles into vertices
    for (size_t i = 0; i < sct_pdgcode->size(); i++) {
      int pdgCode = sct_pdgcode->at(i);
      G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
      if (!particleDef) {
          G4cerr << "Unknown PDG code in SCT: " << pdgCode << G4endl;
          continue;
      }

      double Ekin = sct_energy->at(i) * MeV;
      double mass = particleDef->GetPDGMass();
      double Etot = Ekin + mass;
      double p = std::sqrt(Etot * Etot - mass * mass);

      G4double localOffsetX = sct_x->at(i) - anchorParticleX;
      G4double localOffsetY = sct_y->at(i) - anchorParticleY;

      G4ThreeVector position = G4ThreeVector(globalX + localOffsetX, globalY + localOffsetY, globalZ);
      if (IsWithinTopSurface(position)) {
        G4ThreeVector direction(sct_u->at(i), sct_v->at(i), sct_w->at(i));
        G4ThreeVector momentum = direction * p;

        G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef,
                                                          momentum.x(),
                                                          momentum.y(),
                                                          momentum.z());

        G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
        vertex->SetPrimary(primary);
        anEvent->AddPrimaryVertex(vertex);
      }
      auto* info = new PaleoSimUserEventInformation();
      anEvent->SetUserInformation(info);
    }
  }

  else if (SCTBootstrap=="rrs") {
    // Sample a position on the top of the world volume
    G4ThreeVector randomPosition = SamplePointOnTopOfWorldVolume();
    G4double globalZ = randomPosition.z() * mm;

    // To handle local spacing for multiparticle events
    G4double anchorParticleX = sct_x->at(0) * mm;
    G4double anchorParticleY = sct_y->at(0) * mm;

    // Base point
    G4double globalX = sct_x->at(0) * mm;
    G4double globalY = sct_y->at(0) * mm;
    
    int randomRefRot = G4RandFlat::shootInt(8);
    RandomReflectionRotation(globalX, globalY, randomRefRot);
    LocalSquareSmear(globalX, globalY, nSCTEntries);

    //Load all particles into vertices
    for (size_t i = 0; i < sct_pdgcode->size(); i++) {
      int pdgCode = sct_pdgcode->at(i);
      G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
      if (!particleDef) {
          G4cerr << "Unknown PDG code in SCT: " << pdgCode << G4endl;
          continue;
      }

      double Ekin = sct_energy->at(i) * MeV;
      double mass = particleDef->GetPDGMass();
      double Etot = Ekin + mass;
      double p = std::sqrt(Etot * Etot - mass * mass);

      G4double localOffsetX = sct_x->at(i) - anchorParticleX;
      G4double localOffsetY = sct_y->at(i) - anchorParticleY;

      G4double particleU = sct_u->at(i);
      G4double particleV = sct_v->at(i);
      RandomReflectionRotation(particleU, particleV, randomRefRot);

      G4ThreeVector position = G4ThreeVector(globalX + localOffsetX, globalY + localOffsetY, globalZ);
      if (IsWithinTopSurface(position)) {
        G4ThreeVector direction(particleU, particleV, sct_w->at(i));
        G4ThreeVector momentum = direction * p;

        G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef,
                                                          momentum.x(),
                                                          momentum.y(),
                                                          momentum.z());

        G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
        vertex->SetPrimary(primary);
        anEvent->AddPrimaryVertex(vertex);
      }
      auto* info = new PaleoSimUserEventInformation();
      anEvent->SetUserInformation(info);
    }
  }
}

/*
///////////////////////////////////////////////////////
// Captured Particle(secondaryCaptureTree) generator //
///////////////////////////////////////////////////////
void PaleoSimPrimaryGeneratorAction::InitializeSCTGenerator() {
    //Load file
    G4String filename = fMessenger.GetSCTFilename();
  
    //Check and open root file
    std::ifstream testFile(filename);
    if (!testFile.good()) {
      G4Exception("InitializeSCTGenerator", "SCTGenerator001", FatalException,
                  ("Cannot open SCT file: " + filename).c_str());
    }
    testFile.close();
    SCTFile = TFile::Open(filename);
    if (!SCTFile || SCTFile->IsZombie()) {
      G4Exception("InitializeSCTGenerator", "SCTGenerator002", FatalException,
                  ("Failed to open SCT ROOT file: " + filename).c_str());
    }
    
    //Read in tree
    SCTTree = dynamic_cast<TTree*>(SCTFile->Get("secondaryCaptureTree"));
    if (!SCTTree) {
      G4Exception("InitializeSCTGenerator", "SCTGenerator003", FatalException,
                  "Tree 'secondaryCaptureTree' not found in ROOT file.");
    }
    SCTTree->SetBranchAddress("pdgCode",    &sct_pdgcode);
    SCTTree->SetBranchAddress("energy",     &sct_energy);
    SCTTree->SetBranchAddress("entry_u",    &sct_u);
    SCTTree->SetBranchAddress("entry_v",    &sct_v);
    SCTTree->SetBranchAddress("entry_w",    &sct_w);
    SCTTree->SetBranchAddress("entry_x", &sct_x);
    SCTTree->SetBranchAddress("entry_y", &sct_y);
  
    nSCTEntries = SCTTree->GetEntries();
    std::cout<<"Number of captured events to sample is "<<nSCTEntries<<std::endl;
    SCTFileLoaded = true;
  
    //Preload into memory
    for (Long64_t i = 0; i < nSCTEntries; ++i) {
        SCTTree->GetEntry(i); 
  
        if (i%1000000 ==0 ) {
          std::cout<<"On entry "<<i<<" of "<<nSCTEntries<<std::endl;
        }
  
        if (sct_pdgcode->empty()) {
          continue;
        }
        all_sct_pdgcodes.push_back(*sct_pdgcode); // copy vector
        all_sct_energy.push_back(*sct_energy);
        all_sct_u.push_back(*sct_u);
        all_sct_v.push_back(*sct_v);
        all_sct_w.push_back(*sct_w);
        all_sct_x.push_back(*sct_x);
        all_sct_y.push_back(*sct_y);
    }
  
    nSCTEntries = all_sct_pdgcodes.size();
    std::cout << "Actual pool size for sampling after filtering out empty entries: " << nSCTEntries << std::endl;
  
    //Get header info
    TTree * headerTree = dynamic_cast<TTree*>(SCTFile->Get("headerTree"));
    float norm;
    headerTree->SetBranchAddress("meiHimeFluxNormalization_per_cm2_per_s", &norm); // CHANGE LATER
    headerTree->GetEvent(0);
    fMessenger.SetSCTNorm(static_cast<double>(norm));
    delete headerTree;
  }
  
  void PaleoSimPrimaryGeneratorAction::GenerateSCTPrimaries(G4Event* anEvent) {
    //Get random event
    Long64_t entry = G4RandFlat::shootInt(nSCTEntries);
      
    sct_pdgcode = &all_sct_pdgcodes.at(entry);
    sct_energy  = &all_sct_energy.at(entry);
    sct_u       = &all_sct_u.at(entry);
    sct_v       = &all_sct_v.at(entry);
    sct_w       = &all_sct_w.at(entry);
    sct_x       = &all_sct_x.at(entry);
    sct_y       = &all_sct_y.at(entry);
  
    // Sample a position on the top of the world volume
    G4ThreeVector basePosition = SamplePointOnTopOfWorldVolume();
    G4double OverburdenZ = basePosition.z();
  
    //Load all particles into vertices
    for (size_t i = 0; i < sct_pdgcode->size(); i++) {
      int pdgCode = sct_pdgcode->at(i);
      G4ParticleDefinition* particleDef = G4ParticleTable::GetParticleTable()->FindParticle(pdgCode);
      if (!particleDef) {
          G4cerr << "Unknown PDG code in SCT: " << pdgCode << G4endl;
          continue;
      }
  
      double Ekin = sct_energy->at(i) * MeV;
      double mass = particleDef->GetPDGMass();
      double Etot = Ekin + mass;
      double p = std::sqrt(Etot * Etot - mass * mass);
  
      G4ThreeVector position = G4ThreeVector(sct_x->at(i) * mm, sct_y->at(i) * mm, OverburdenZ * mm);
      if (IsWithinTopSurface(position)) {
        G4ThreeVector direction(sct_u->at(i), sct_v->at(i), sct_w->at(i));
        G4ThreeVector momentum = direction * p;
  
        G4PrimaryParticle* primary = new G4PrimaryParticle(particleDef,
                                                          momentum.x(),
                                                          momentum.y(),
                                                          momentum.z());
  
        G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0.0);
        vertex->SetPrimary(primary);
        anEvent->AddPrimaryVertex(vertex);
      }
      auto* info = new PaleoSimUserEventInformation();
      anEvent->SetUserInformation(info);
    }
  }
*/