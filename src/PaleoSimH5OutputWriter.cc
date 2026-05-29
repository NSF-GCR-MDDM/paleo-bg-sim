#include "PaleoSimH5OutputWriter.hh"

#include "PaleoSimOutputManager.hh"
#include "G4Exception.hh"
#include "H5Cpp.h"
#include "TTree.h"
#include <cstring>
#include <string>
#include <vector>

namespace {

template <typename T>
void WriteScalarDataset(H5::H5File& file, const std::string& path, const std::vector<T>& data, const H5::PredType& type) {
  hsize_t dim[1] = { static_cast<hsize_t>(data.size()) };
  H5::DataSpace space(1, dim);
  H5::DataSet ds = file.createDataSet(path, type, space);
  if (!data.empty()) {
    ds.write(data.data(), type);
  }
}

template <typename T>
void WriteJaggedDataset(H5::H5File& file,
                        const std::string& groupPath,
                        const std::vector<std::vector<T>>& rows,
                        const H5::PredType& type) {
  std::vector<T> flat;
  std::vector<unsigned long long> offsets;
  offsets.reserve(rows.size() + 1);
  offsets.push_back(0ULL);

  for (const auto& row : rows) {
    flat.insert(flat.end(), row.begin(), row.end());
    offsets.push_back(static_cast<unsigned long long>(flat.size()));
  }

  WriteScalarDataset(file, groupPath + "/data", flat, type);
  WriteScalarDataset(file, groupPath + "/offsets", offsets, H5::PredType::NATIVE_ULLONG);
}

void EnsureGroup(H5::H5File& file, const std::string& groupPath) {
  if (groupPath.empty() || groupPath == "/") return;
  size_t start = 1;
  while (start < groupPath.size()) {
    size_t slash = groupPath.find('/', start);
    std::string sub = (slash == std::string::npos) ? groupPath : groupPath.substr(0, slash);
    const htri_t exists = H5Lexists(file.getId(), sub.c_str(), H5P_DEFAULT);
    if (exists < 0) {
      G4Exception("PaleoSimOutputManager", "H5GroupCheckFailed", FatalException,
                  ("Failed to check whether HDF5 group exists: " + sub).c_str());
    }
    if (exists == 0) {
      file.createGroup(sub);
    }
    if (slash == std::string::npos) break;
    start = slash + 1;
  }
}

void WriteHeaderTree(H5::H5File& h5, TTree* tree) {
  if (!tree) return;
  EnsureGroup(h5, "/headerTree");

  long long nps = 0;
  char sourceType[256] = {0};

  if (tree->GetBranch("nps")) tree->SetBranchAddress("nps", &nps);
  if (tree->GetBranch("sourceType")) tree->SetBranchAddress("sourceType", sourceType);

  std::vector<long long> npsVals;
  std::vector<std::string> sourceVals;

  const Long64_t n = tree->GetEntries();
  npsVals.reserve(static_cast<size_t>(n));
  sourceVals.reserve(static_cast<size_t>(n));

  for (Long64_t i = 0; i < n; ++i) {
    tree->GetEntry(i);
    npsVals.push_back(nps);
    sourceVals.emplace_back(sourceType);
  }

  WriteScalarDataset(h5, "/headerTree/nps", npsVals, H5::PredType::NATIVE_LLONG);

  std::vector<char> chars;
  chars.resize(sourceVals.size() * 256, '\0');
  for (size_t i = 0; i < sourceVals.size(); ++i) {
    std::strncpy(&chars[i * 256], sourceVals[i].c_str(), 255);
  }
  hsize_t dims[2] = { static_cast<hsize_t>(sourceVals.size()), 256 };
  H5::DataSpace space(2, dims);
  H5::StrType strType(H5::PredType::C_S1, 256);
  H5::DataSet ds = h5.createDataSet("/headerTree/sourceType", strType, space);
  if (!chars.empty()) ds.write(chars.data(), strType);
}

void WriteGeometryTree(H5::H5File& h5, TTree* tree) {
  if (!tree) return;
  EnsureGroup(h5, "/geometryTree");

  char name[256] = {0}, shape[256] = {0}, parent[256] = {0}, material[256] = {0};
  int number = 0;
  double abs_x = 0., abs_y = 0., abs_z = 0.;
  std::vector<double>* xs = nullptr;
  std::vector<double>* ys = nullptr;
  std::vector<double>* zs = nullptr;

  if (tree->GetBranch("name")) tree->SetBranchAddress("name", name);
  if (tree->GetBranch("shape")) tree->SetBranchAddress("shape", shape);
  if (tree->GetBranch("parent")) tree->SetBranchAddress("parent", parent);
  if (tree->GetBranch("material")) tree->SetBranchAddress("material", material);
  if (tree->GetBranch("number")) tree->SetBranchAddress("number", &number);
  if (tree->GetBranch("abs_x")) tree->SetBranchAddress("abs_x", &abs_x);
  if (tree->GetBranch("abs_y")) tree->SetBranchAddress("abs_y", &abs_y);
  if (tree->GetBranch("abs_z")) tree->SetBranchAddress("abs_z", &abs_z);
  if (tree->GetBranch("pointCloud_xs")) tree->SetBranchAddress("pointCloud_xs", &xs);
  if (tree->GetBranch("pointCloud_ys")) tree->SetBranchAddress("pointCloud_ys", &ys);
  if (tree->GetBranch("pointCloud_zs")) tree->SetBranchAddress("pointCloud_zs", &zs);

  std::vector<std::string> names, shapes, parents, materials;
  std::vector<int> numbers;
  std::vector<double> ax, ay, az;
  std::vector<std::vector<double>> allXs, allYs, allZs;

  const Long64_t n = tree->GetEntries();
  names.reserve(static_cast<size_t>(n));
  shapes.reserve(static_cast<size_t>(n));
  parents.reserve(static_cast<size_t>(n));
  materials.reserve(static_cast<size_t>(n));
  numbers.reserve(static_cast<size_t>(n));
  ax.reserve(static_cast<size_t>(n));
  ay.reserve(static_cast<size_t>(n));
  az.reserve(static_cast<size_t>(n));
  allXs.reserve(static_cast<size_t>(n));
  allYs.reserve(static_cast<size_t>(n));
  allZs.reserve(static_cast<size_t>(n));

  for (Long64_t i = 0; i < n; ++i) {
    tree->GetEntry(i);
    names.emplace_back(name);
    shapes.emplace_back(shape);
    parents.emplace_back(parent);
    materials.emplace_back(material);
    numbers.push_back(number);
    ax.push_back(abs_x);
    ay.push_back(abs_y);
    az.push_back(abs_z);
    allXs.push_back(xs ? *xs : std::vector<double>{});
    allYs.push_back(ys ? *ys : std::vector<double>{});
    allZs.push_back(zs ? *zs : std::vector<double>{});
  }

  auto writeFixedStrings = [&](const std::string& path, const std::vector<std::string>& vals) {
    std::vector<char> chars(vals.size() * 256, '\0');
    for (size_t i = 0; i < vals.size(); ++i) std::strncpy(&chars[i * 256], vals[i].c_str(), 255);
    hsize_t dims[2] = { static_cast<hsize_t>(vals.size()), 256 };
    H5::DataSpace space(2, dims);
    H5::StrType strType(H5::PredType::C_S1, 256);
    H5::DataSet ds = h5.createDataSet(path, strType, space);
    if (!chars.empty()) ds.write(chars.data(), strType);
  };

  writeFixedStrings("/geometryTree/name", names);
  writeFixedStrings("/geometryTree/shape", shapes);
  writeFixedStrings("/geometryTree/parent", parents);
  writeFixedStrings("/geometryTree/material", materials);

  WriteScalarDataset(h5, "/geometryTree/number", numbers, H5::PredType::NATIVE_INT);
  WriteScalarDataset(h5, "/geometryTree/abs_x", ax, H5::PredType::NATIVE_DOUBLE);
  WriteScalarDataset(h5, "/geometryTree/abs_y", ay, H5::PredType::NATIVE_DOUBLE);
  WriteScalarDataset(h5, "/geometryTree/abs_z", az, H5::PredType::NATIVE_DOUBLE);

  EnsureGroup(h5, "/geometryTree/pointCloud_xs");
  EnsureGroup(h5, "/geometryTree/pointCloud_ys");
  EnsureGroup(h5, "/geometryTree/pointCloud_zs");
  WriteJaggedDataset(h5, "/geometryTree/pointCloud_xs", allXs, H5::PredType::NATIVE_DOUBLE);
  WriteJaggedDataset(h5, "/geometryTree/pointCloud_ys", allYs, H5::PredType::NATIVE_DOUBLE);
  WriteJaggedDataset(h5, "/geometryTree/pointCloud_zs", allZs, H5::PredType::NATIVE_DOUBLE);
}

void WriteNumericTrees(H5::H5File& h5, TTree* tree, const std::string& base) {
  if (!tree) return;
  EnsureGroup(h5, base);

  if (base == "/primariesTree") {
    int eventID = -1;
    std::vector<int>* pdgID = nullptr;
    std::vector<double>* energy = nullptr;
    std::vector<double>* x = nullptr; std::vector<double>* y = nullptr; std::vector<double>* z = nullptr;
    std::vector<double>* px = nullptr; std::vector<double>* py = nullptr; std::vector<double>* pz = nullptr;
    double muonTheta = 0., muonPhi = 0., muonSlant = 0.;
    double CRYCoreX = 0., CRYCoreY = 0., CRYCoreZ = 0., CRYCoreTheta = 0., CRYCorePhi = 0., CRYTotalEnergy = 0.;

    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("pdgID", &pdgID);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("x", &x); tree->SetBranchAddress("y", &y); tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("px", &px); tree->SetBranchAddress("py", &py); tree->SetBranchAddress("pz", &pz);
    tree->SetBranchAddress("muonTheta", &muonTheta);
    tree->SetBranchAddress("muonPhi", &muonPhi);
    tree->SetBranchAddress("muonSlant", &muonSlant);
    tree->SetBranchAddress("CRYCoreX", &CRYCoreX);
    tree->SetBranchAddress("CRYCoreY", &CRYCoreY);
    tree->SetBranchAddress("CRYCoreZ", &CRYCoreZ);
    tree->SetBranchAddress("CRYCoreTheta", &CRYCoreTheta);
    tree->SetBranchAddress("CRYCorePhi", &CRYCorePhi);
    tree->SetBranchAddress("CRYTotalEnergy", &CRYTotalEnergy);

    std::vector<int> v_eventID;
    std::vector<double> v_muonTheta, v_muonPhi, v_muonSlant, v_CRYCoreX, v_CRYCoreY, v_CRYCoreZ, v_CRYCoreTheta, v_CRYCorePhi, v_CRYTotalEnergy;
    std::vector<std::vector<int>> v_pdgID;
    std::vector<std::vector<double>> v_energy, v_x, v_y, v_z, v_px, v_py, v_pz;

    const Long64_t n = tree->GetEntries();
    for (Long64_t i = 0; i < n; ++i) {
      tree->GetEntry(i);
      v_eventID.push_back(eventID);
      v_pdgID.push_back(pdgID ? *pdgID : std::vector<int>{});
      v_energy.push_back(energy ? *energy : std::vector<double>{});
      v_x.push_back(x ? *x : std::vector<double>{});
      v_y.push_back(y ? *y : std::vector<double>{});
      v_z.push_back(z ? *z : std::vector<double>{});
      v_px.push_back(px ? *px : std::vector<double>{});
      v_py.push_back(py ? *py : std::vector<double>{});
      v_pz.push_back(pz ? *pz : std::vector<double>{});
      v_muonTheta.push_back(muonTheta);
      v_muonPhi.push_back(muonPhi);
      v_muonSlant.push_back(muonSlant);
      v_CRYCoreX.push_back(CRYCoreX);
      v_CRYCoreY.push_back(CRYCoreY);
      v_CRYCoreZ.push_back(CRYCoreZ);
      v_CRYCoreTheta.push_back(CRYCoreTheta);
      v_CRYCorePhi.push_back(CRYCorePhi);
      v_CRYTotalEnergy.push_back(CRYTotalEnergy);
    }

    WriteScalarDataset(h5, base + "/eventID", v_eventID, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/pdgID"); WriteJaggedDataset(h5, base + "/pdgID", v_pdgID, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/energy"); WriteJaggedDataset(h5, base + "/energy", v_energy, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/x"); WriteJaggedDataset(h5, base + "/x", v_x, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/y"); WriteJaggedDataset(h5, base + "/y", v_y, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/z"); WriteJaggedDataset(h5, base + "/z", v_z, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/px"); WriteJaggedDataset(h5, base + "/px", v_px, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/py"); WriteJaggedDataset(h5, base + "/py", v_py, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/pz"); WriteJaggedDataset(h5, base + "/pz", v_pz, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/muonTheta", v_muonTheta, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/muonPhi", v_muonPhi, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/muonSlant", v_muonSlant, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/CRYCoreX", v_CRYCoreX, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/CRYCoreY", v_CRYCoreY, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/CRYCoreZ", v_CRYCoreZ, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/CRYCoreTheta", v_CRYCoreTheta, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/CRYCorePhi", v_CRYCorePhi, H5::PredType::NATIVE_DOUBLE);
    WriteScalarDataset(h5, base + "/CRYTotalEnergy", v_CRYTotalEnergy, H5::PredType::NATIVE_DOUBLE);
  }

  if (base == "/MINTree") {
    int eventID = -1, multiplicity = 0;
    std::vector<double>* angle = nullptr;
    std::vector<double>* energy = nullptr;
    std::vector<double>* dist = nullptr;

    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("multiplicity", &multiplicity);
    tree->SetBranchAddress("angleRelToMuon", &angle);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("distanceToMuonTrack", &dist);

    std::vector<int> v_eventID, v_multiplicity;
    std::vector<std::vector<double>> v_angle, v_energy, v_dist;

    const Long64_t n = tree->GetEntries();
    for (Long64_t i = 0; i < n; ++i) {
      tree->GetEntry(i);
      v_eventID.push_back(eventID);
      v_multiplicity.push_back(multiplicity);
      v_angle.push_back(angle ? *angle : std::vector<double>{});
      v_energy.push_back(energy ? *energy : std::vector<double>{});
      v_dist.push_back(dist ? *dist : std::vector<double>{});
    }

    WriteScalarDataset(h5, base + "/eventID", v_eventID, H5::PredType::NATIVE_INT);
    WriteScalarDataset(h5, base + "/multiplicity", v_multiplicity, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/angleRelToMuon"); WriteJaggedDataset(h5, base + "/angleRelToMuon", v_angle, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/energy"); WriteJaggedDataset(h5, base + "/energy", v_energy, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/distanceToMuonTrack"); WriteJaggedDataset(h5, base + "/distanceToMuonTrack", v_dist, H5::PredType::NATIVE_DOUBLE);
  }

  if (base == "/neutronTallyTree") {
    int eventID = -1, num = 0;
    std::vector<double>* entry_energy = nullptr;
    std::vector<double>* entry_x = nullptr; std::vector<double>* entry_y = nullptr; std::vector<double>* entry_z = nullptr;
    std::vector<double>* entry_u = nullptr; std::vector<double>* entry_v = nullptr; std::vector<double>* entry_w = nullptr;
    std::vector<double>* angleRelMuon = nullptr; std::vector<double>* distanceToMuonTrack = nullptr;
    std::vector<int>* volumeNumbers = nullptr; std::vector<int>* prevVolumeNumbers = nullptr;

    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("numNeutronsEntered", &num);
    tree->SetBranchAddress("entry_energy", &entry_energy);
    tree->SetBranchAddress("entry_x", &entry_x); tree->SetBranchAddress("entry_y", &entry_y); tree->SetBranchAddress("entry_z", &entry_z);
    tree->SetBranchAddress("entry_u", &entry_u); tree->SetBranchAddress("entry_v", &entry_v); tree->SetBranchAddress("entry_w", &entry_w);
    tree->SetBranchAddress("angleRelMuon", &angleRelMuon);
    tree->SetBranchAddress("distanceToMuonTrack", &distanceToMuonTrack);
    tree->SetBranchAddress("volumeNumbers", &volumeNumbers);
    tree->SetBranchAddress("prevVolumeNumbers", &prevVolumeNumbers);

    std::vector<int> v_eventID, v_num;
    std::vector<std::vector<double>> v_entry_energy, v_entry_x, v_entry_y, v_entry_z, v_entry_u, v_entry_v, v_entry_w, v_angle, v_dist;
    std::vector<std::vector<int>> v_vol, v_prevVol;

    const Long64_t n = tree->GetEntries();
    for (Long64_t i = 0; i < n; ++i) {
      tree->GetEntry(i);
      v_eventID.push_back(eventID);
      v_num.push_back(num);
      v_entry_energy.push_back(entry_energy ? *entry_energy : std::vector<double>{});
      v_entry_x.push_back(entry_x ? *entry_x : std::vector<double>{});
      v_entry_y.push_back(entry_y ? *entry_y : std::vector<double>{});
      v_entry_z.push_back(entry_z ? *entry_z : std::vector<double>{});
      v_entry_u.push_back(entry_u ? *entry_u : std::vector<double>{});
      v_entry_v.push_back(entry_v ? *entry_v : std::vector<double>{});
      v_entry_w.push_back(entry_w ? *entry_w : std::vector<double>{});
      v_angle.push_back(angleRelMuon ? *angleRelMuon : std::vector<double>{});
      v_dist.push_back(distanceToMuonTrack ? *distanceToMuonTrack : std::vector<double>{});
      v_vol.push_back(volumeNumbers ? *volumeNumbers : std::vector<int>{});
      v_prevVol.push_back(prevVolumeNumbers ? *prevVolumeNumbers : std::vector<int>{});
    }

    WriteScalarDataset(h5, base + "/eventID", v_eventID, H5::PredType::NATIVE_INT);
    WriteScalarDataset(h5, base + "/numNeutronsEntered", v_num, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/entry_energy"); WriteJaggedDataset(h5, base + "/entry_energy", v_entry_energy, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/entry_x"); WriteJaggedDataset(h5, base + "/entry_x", v_entry_x, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/entry_y"); WriteJaggedDataset(h5, base + "/entry_y", v_entry_y, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/entry_z"); WriteJaggedDataset(h5, base + "/entry_z", v_entry_z, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/entry_u"); WriteJaggedDataset(h5, base + "/entry_u", v_entry_u, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/entry_v"); WriteJaggedDataset(h5, base + "/entry_v", v_entry_v, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/entry_w"); WriteJaggedDataset(h5, base + "/entry_w", v_entry_w, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/angleRelMuon"); WriteJaggedDataset(h5, base + "/angleRelMuon", v_angle, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/distanceToMuonTrack"); WriteJaggedDataset(h5, base + "/distanceToMuonTrack", v_dist, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/volumeNumbers"); WriteJaggedDataset(h5, base + "/volumeNumbers", v_vol, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/prevVolumeNumbers"); WriteJaggedDataset(h5, base + "/prevVolumeNumbers", v_prevVol, H5::PredType::NATIVE_INT);
  }

  if (base == "/recoilTree") {
    int historyNum = -1, nRecoils = 0;
    std::vector<int>* pdgCode = nullptr;
    std::vector<int>* parent_pdgCode = nullptr;
    std::vector<double>* energy = nullptr;
    std::vector<double>* x = nullptr; std::vector<double>* y = nullptr; std::vector<double>* z = nullptr;
    std::vector<double>* u = nullptr; std::vector<double>* v = nullptr; std::vector<double>* w = nullptr;
    std::vector<double>* time = nullptr; std::vector<double>* code = nullptr;
    std::vector<int>* volumeNumbers = nullptr;

    tree->SetBranchAddress("historyNum", &historyNum);
    tree->SetBranchAddress("pdgCode", &pdgCode);
    tree->SetBranchAddress("parent_pdgCode", &parent_pdgCode);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("x", &x); tree->SetBranchAddress("y", &y); tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("u", &u); tree->SetBranchAddress("v", &v); tree->SetBranchAddress("w", &w);
    tree->SetBranchAddress("time", &time);
    tree->SetBranchAddress("code", &code);
    tree->SetBranchAddress("nRecoils", &nRecoils);
    tree->SetBranchAddress("volumeNumbers", &volumeNumbers);

    std::vector<int> v_historyNum, v_nRecoils;
    std::vector<std::vector<int>> v_pdgCode, v_parentPdgCode, v_volumeNumbers;
    std::vector<std::vector<double>> v_energy, v_x, v_y, v_z, v_u, v_v, v_w, v_time, v_code;

    const Long64_t n = tree->GetEntries();
    for (Long64_t i = 0; i < n; ++i) {
      tree->GetEntry(i);
      v_historyNum.push_back(historyNum);
      v_nRecoils.push_back(nRecoils);
      v_pdgCode.push_back(pdgCode ? *pdgCode : std::vector<int>{});
      v_parentPdgCode.push_back(parent_pdgCode ? *parent_pdgCode : std::vector<int>{});
      v_energy.push_back(energy ? *energy : std::vector<double>{});
      v_x.push_back(x ? *x : std::vector<double>{});
      v_y.push_back(y ? *y : std::vector<double>{});
      v_z.push_back(z ? *z : std::vector<double>{});
      v_u.push_back(u ? *u : std::vector<double>{});
      v_v.push_back(v ? *v : std::vector<double>{});
      v_w.push_back(w ? *w : std::vector<double>{});
      v_time.push_back(time ? *time : std::vector<double>{});
      v_code.push_back(code ? *code : std::vector<double>{});
      v_volumeNumbers.push_back(volumeNumbers ? *volumeNumbers : std::vector<int>{});
    }

    WriteScalarDataset(h5, base + "/historyNum", v_historyNum, H5::PredType::NATIVE_INT);
    WriteScalarDataset(h5, base + "/nRecoils", v_nRecoils, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/pdgCode"); WriteJaggedDataset(h5, base + "/pdgCode", v_pdgCode, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/parent_pdgCode"); WriteJaggedDataset(h5, base + "/parent_pdgCode", v_parentPdgCode, H5::PredType::NATIVE_INT);
    EnsureGroup(h5, base + "/energy"); WriteJaggedDataset(h5, base + "/energy", v_energy, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/x"); WriteJaggedDataset(h5, base + "/x", v_x, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/y"); WriteJaggedDataset(h5, base + "/y", v_y, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/z"); WriteJaggedDataset(h5, base + "/z", v_z, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/u"); WriteJaggedDataset(h5, base + "/u", v_u, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/v"); WriteJaggedDataset(h5, base + "/v", v_v, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/w"); WriteJaggedDataset(h5, base + "/w", v_w, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/time"); WriteJaggedDataset(h5, base + "/time", v_time, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/code"); WriteJaggedDataset(h5, base + "/code", v_code, H5::PredType::NATIVE_DOUBLE);
    EnsureGroup(h5, base + "/volumeNumbers"); WriteJaggedDataset(h5, base + "/volumeNumbers", v_volumeNumbers, H5::PredType::NATIVE_INT);
  }
}

} // namespace

void PaleoSimH5OutputWriter::Write(PaleoSimOutputManager& manager) {
  const std::string outputPath = manager.fMessenger.GetOutputPath();
  H5::H5File h5(outputPath, H5F_ACC_TRUNC);

  WriteHeaderTree(h5, manager.fHeaderTree);
  WriteGeometryTree(h5, manager.fGeometryTree);

  if (manager.fMessenger.GetPrimariesTreeStatus()) WriteNumericTrees(h5, manager.fPrimariesTree, "/primariesTree");
  if (manager.fMessenger.GetMINTreeStatus()) WriteNumericTrees(h5, manager.fMINTree, "/MINTree");
  if (manager.fMessenger.GetNeutronTallyTreeStatus()) WriteNumericTrees(h5, manager.fNeutronTallyTree, "/neutronTallyTree");
  if (manager.fMessenger.GetRecoilTreeStatus()) WriteNumericTrees(h5, manager.fRecoilTree, "/recoilTree");

  h5.close();

  if (manager.fFile) {
    manager.fFile->Close();
    delete manager.fFile;
    manager.fFile = nullptr;
  }

  delete manager.fHeaderTree; manager.fHeaderTree = nullptr;
  delete manager.fGeometryTree; manager.fGeometryTree = nullptr;
  delete manager.fPrimariesTree; manager.fPrimariesTree = nullptr;
  delete manager.fMINTree; manager.fMINTree = nullptr;
  delete manager.fNeutronTallyTree; manager.fNeutronTallyTree = nullptr;
  delete manager.fRecoilTree; manager.fRecoilTree = nullptr;
}
