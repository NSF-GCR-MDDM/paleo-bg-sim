import mute.constants as mtc
import mute.underground as mtu
import numpy as np
import math
import ROOT

outputFile = ROOT.TFile("muons.root","RECREATE")
depth = 5.993  # km

# Setup MUTE
mtc.set_verbose(0)
mtc.set_vertical_depth(depth)
mtc.set_medium("rock")
mtc.set_density(2.65)
mtc.set_n_muon(100000)

# Get flux grid
fluxes = mtu.calc_u_fluxes(primary_model="GSF", interaction_model="DDM")  # shape (E, slant_depth)
energies = mtc.ENERGIES / 1e3  # MeV → GeV
slant_depths = mtc.slant_depths
thetas = np.arccos(depth / slant_depths)  # radians

# Energy: log bin edges, append 1e8 GeV (100 PeV)
energy_edges = np.append(energies, 1e8)

# Theta: uniform spacing → extend with extra edge
dtheta = thetas[1] - thetas[0]
theta_edges = np.append(thetas, thetas[-1] + dtheta)

# === Normalize flux to probability density
normalized_fluxes = fluxes / np.sum(fluxes)

# === Create TH2D
h2 = ROOT.TH2D("muonHist", "Muon Flux PDF;E_{#mu} [GeV];#theta [rad]",
               len(energy_edges)-1, energy_edges,
               len(theta_edges)-1, theta_edges)

# Fill histogram
for i in range(len(energies)):
    for j in range(len(thetas)):
        h2.SetBinContent(i+1, j+1, normalized_fluxes[i, j])  # 1-based indexing

# Write output
outputFile.cd()
h2.Write()
outputFile.Close()
