#Goal is to replicate:
# A. Fig 8: primary muon spectrum on surface, primary muon spectrum entering room. Counts/h/100 keV, 3000 keV to 16000 keV
#    Their normalization is 200 +- 5 mu/m2/sec at surface, 107+-3 mu/m2/sec inside. x lin, y log
# B. Fig. 15: Cosmogenic neutrons at the surface, cosmogenic neutrons entering the room, muon-induced neutrons entering the room,
#    and total neutrons entering the room. From 10^-9 MeV to 10^3 MeV. Their plot is E_n * phi_n(E_n) / cm2 /day, log log plt
# C. Table 5: Neutron fluxes entering room in phi (/cm2/d) in the thermal (1e-9 to 4e-7), intermediate (4e-7 to 0.1), fast 
#    (0.1 to 20 MeV) and cascade (20, 1000 MeV) along with total

import ROOT
import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['axes.labelsize'] = 16
plt.rcParams['axes.titlesize'] = 16
plt.rcParams['xtick.labelsize'] = 14
plt.rcParams['ytick.labelsize'] = 14
plt.rcParams['legend.fontsize'] = 14

#############
##LOAD DATA##
#############
muon_primary_df = ROOT.RDataFrame("primariesTree", "/home/sch52/code/paleo-bgnd-sim-build/conus_cry_outputs/muons_new_*.root")
neutron_primary_df = ROOT.RDataFrame("primariesTree", "/home/sch52/code/paleo-bgnd-sim-build/conus_cry_outputs/neutrons_new_*.root")
muon_tally_df = ROOT.RDataFrame("neutronTallyTree", "/home/sch52/code/paleo-bgnd-sim-build/conus_cry_outputs/muons_new_*.root")
neutron_tally_df = ROOT.RDataFrame("neutronTallyTree", "/home/sch52/code/paleo-bgnd-sim-build/conus_cry_outputs/neutrons_new_*.root")
print("Loaded data!")

############
##FIGURE 8##
############
#Skipping as this plot kind of sucks and is not informative on what we need to do to recreate.
'''
primary_muon_energies_dict = muon_primary_df.AsNumpy(["energy"])
primary_muon_energies_MeV = np.concatenate([np.asarray(nrg, dtype=float) for nrg in primary_muon_energies_dict["energy"]]) #Make single 1D array
primary_muon_energies_keV = primary_muon_energies_MeV*0.001

muon_norm = 0.008635 #mu/cm2/sec
nMuons = primary_muon_energies_keV.size
print(nMuons)
overburden_area_cm2 = 3.762*100 * 3.762*100 
overburden_area_m2 = 3.762*3.762
nSeconds_simulated = (nMuons / overburden_area_cm2) / muon_norm
print(f"Simulated {nMuons} muons on an overburden of area {overburden_area_cm2}cm2 corresponding to {nSeconds_simulated/3600.} hours of real exposure!")
print(f"Muon rate on surface is {nMuons/(overburden_area_m2*nSeconds_simulated)} muons/m2/sec")

counts,edges = np.histogram(primary_muon_energies_keV,bins=130,range=(3000,16000))
centers = 0.5 * (edges[1:] + edges[:-1])
nHours_simulated = nSeconds_simulated / 3600.
scaleFactor = 1./nHours_simulated
counts_scaled = counts * scaleFactor
errors = np.sqrt(counts) * scaleFactor


fig,ax = plt.subplots()
ax.errorbar(
        centers, counts_scaled, yerr=errors,
        xerr=(edges[1]-edges[0])/2,
        fmt='o', markersize=3, capsize=2
    )

#ax.hist(edges[:-1],edges,weights=counts*scaleFactor)
ax.set_xlabel("Muon energy (keV)")
ax.set_ylabel(f"Counts/hour/100 keV/{overburden_area_m2}m2")
ax.set_yscale("log")
plt.show()
'''

#############
##FIGURE 15##
#############
#CONUS plots E_n * phi_n vs E_n. So, first we make a histogram. We're going to use 100 logx spaced bins between 10e-9 and 10e3
bin_edges = np.logspace(np.log10(1e-9), np.log10(1e3),101,endpoint=True)
centers = np.sqrt(bin_edges[:-1]*bin_edges[1:]) #log(x) bin centers

#Calculate neutron sim norm
primary_surface_neutron_energies_dict = neutron_primary_df.AsNumpy(["energy"])
primary_surface_neutron_energies_MeV = np.concatenate([np.asarray(nrg,dtype=float) for nrg in primary_surface_neutron_energies_dict["energy"]])
neutron_norm = 0.002356 #neutrons/cm2/sec. CONUS reports 0.014 n/cm2/sec, a factor of 5.94 times higher than CRY
nNeutrons = primary_surface_neutron_energies_dict["energy"].size
overburden_area_cm2 = 3.762*100 * 3.762*100 
overburden_area_m2 = 3.762*3.762
neutron_nSeconds_simulated = (nNeutrons / overburden_area_cm2) / neutron_norm
neutron_nDays_simulated = neutron_nSeconds_simulated/86400.
print(f"Simulated {nNeutrons} neutrons on an overburden of area {overburden_area_cm2:.2f}cm2 corresponding to {neutron_nSeconds_simulated/3600.:.2f} hours of real exposure!")
print(f"Neutron rate on surface is {nNeutrons/(overburden_area_m2*neutron_nSeconds_simulated):.2f} neutrons/m2/sec")

#Calculate muon sim norm
primary_surface_muons_energies_dict = muon_primary_df.AsNumpy(["energy"])
primary_surface_muons_energies_MeV = np.concatenate([np.asarray(nrg,dtype=float) for nrg in primary_surface_muons_energies_dict["energy"]])
muon_norm = 0.008635 #muons/cm2/sec. CONUS Reports 0.02033, a factor of 2.35 higher than CRY
nMuons = primary_surface_muons_energies_dict["energy"].size
muon_nSeconds_simulated = (nMuons / overburden_area_cm2) / muon_norm
muon_nDays_simulated = muon_nSeconds_simulated/86400.
print(f"Simulated {nMuons} muons on an overburden of area {overburden_area_cm2:.2f}cm2 corresponding to {muon_nSeconds_simulated/3600.:.2f} hours of real exposure!")
print(f"Neutron rate on surface is {nMuons/(overburden_area_m2*muon_nSeconds_simulated):.2f} neutrons/m2/sec")

#Calculate inner room size
room_overburden_area_m2 = 3.412*3.412
room_overburden_area_cm2 = 3.412*100 * 3.412*100

##Neutrons thrown on surface##
counts,_ = np.histogram(primary_surface_neutron_energies_MeV,bins=bin_edges)
bin_widths =  np.log(bin_edges[1:] / bin_edges[:-1])
surface_ys = counts / (neutron_nDays_simulated * overburden_area_cm2 * bin_widths)
surface_xs = centers

##Neutrons entering room from neutron sim##
neutron_sim_room_energies_dict = neutron_tally_df.AsNumpy(["entry_energy"])
neutron_sim_room_energies_MeV = np.concatenate([np.asarray(nrg,dtype=float) for nrg in neutron_sim_room_energies_dict["entry_energy"]])
counts,_ = np.histogram(neutron_sim_room_energies_MeV,bins=bin_edges)
neutron_sim_room_ys = counts / (neutron_nDays_simulated * room_overburden_area_cm2 * bin_widths)
neutron_sim_room_xs = centers

##Neutrons entering room from muon sim##
muon_sim_room_energies_dict = muon_tally_df.AsNumpy(["entry_energy"])
muon_sim_room_energies_MeV = np.concatenate([np.asarray(nrg,dtype=float) for nrg in muon_sim_room_energies_dict["entry_energy"]])
counts,_ = np.histogram(muon_sim_room_energies_MeV,bins=bin_edges)
muon_sim_room_ys = counts / (muon_nDays_simulated * room_overburden_area_cm2 * bin_widths)
muon_sim_room_xs = centers

##Total neutrons##
total_room_ys = muon_sim_room_ys+neutron_sim_room_ys
total_room_xs = centers

#Print output for patrick
outFile = open("all_neutrons_entering_room.txt","w")
for i,x in enumerate(total_room_xs):
    line=""
    if i==0:
        line="#Neutron energy (MeV), Neutron energy * flux/cm2/day"
    else:
        line=f"\n{x:.3f},{total_room_ys[i]:.2g}"
    outFile.write(line)
outFile.close()

#Print output for patrick
outFile = open("cosmic_neutron_spectrum.txt","w")
for i,x in enumerate(surface_xs):
    line=""
    if i==0:
        line="#Neutron energy (MeV), Neutron energy * flux/cm2/day"
    else:
        line=f"\n{x:.3f},{surface_ys[i]:.2g}"
    outFile.write(line)
outFile.close()

#Print output for patrick
outFile = open("neutrons_entering_room_from_neutron_sim.txt","w")
for i,x in enumerate(neutron_sim_room_xs):
    line=""
    if i==0:
        line="#Neutron energy (MeV), Neutron energy * flux/cm2/day"
    else:
        line=f"\n{x:.3f},{neutron_sim_room_ys[i]:.2g}"
    outFile.write(line)
outFile.close()

#Print output for patrick
outFile = open("neutrons_entering_room_from_muon_sim.txt","w")
for i,x in enumerate(muon_sim_room_xs):
    line=""
    if i==0:
        line="#Neutron energy (MeV), Neutron energy * flux/cm2/day"
    else:
        line=f"\n{x:.3f},{muon_sim_room_ys[i]:.2g}"
    outFile.write(line)
outFile.close()

#Plot
fig2,ax2 = plt.subplots(figsize=(8.00, 7.78))
ax2.plot(surface_xs,surface_ys,marker="o",lw=0,color="k",label = "Cosmogenic neutrons surface",markersize=4)
ax2.plot(total_room_xs,total_room_ys,marker="s",lw=0,color="tab:blue", label = "Total neutrons ZA28R027",markersize=4)
ax2.plot(neutron_sim_room_xs,neutron_sim_room_ys,marker="v",lw=0,color="tab:red",label = "Cosmogenic neutrons ZA28R027",markersize=4)
ax2.plot(muon_sim_room_xs,muon_sim_room_ys,marker="^",lw=0,color="tab:green", label = chr(956)+"-induced neutrons ZA28R027",markersize=4)
ax2.set_yscale("log")
ax2.set_xscale("log")
ax2.set_ylim(bottom=1e-3,top=3e4)
ax2.set_xlim(left=1e-9,right=1e3)
ax2.legend()
ax2.set_xlabel("Energy [MeV]")
ax2.set_ylabel("$E_n$"+chr(966)+"$_n$($E_n$)$cm^{-2}(d)^{-1}$")

#Calculate total counts entering room in energy cuts and statistical errors
thermal_mask = (total_room_xs>= 1e-9) & (total_room_xs <= 4e-7)
intermediate_mask = (total_room_xs>= 4e-7) & (total_room_xs <= 0.1)
fast_mask = (total_room_xs>= 0.1) & (total_room_xs <= 20)
cascade_mask = (total_room_xs>= 20) & (total_room_xs <= 1000)

thermal_counts = np.sum(total_room_ys[thermal_mask] * bin_widths[thermal_mask])
intermediate_counts = np.sum(total_room_ys[intermediate_mask] * bin_widths[intermediate_mask])
fast_counts = np.sum(total_room_ys[fast_mask] * bin_widths[fast_mask])
cascade_counts = np.sum(total_room_ys[cascade_mask] * bin_widths[cascade_mask])
total_counts = np.sum(total_room_ys * bin_widths)

total_counts_raw = neutron_sim_room_ys*(neutron_nDays_simulated * room_overburden_area_cm2 * bin_widths) + muon_sim_room_ys*(muon_nDays_simulated * room_overburden_area_cm2 * bin_widths)

thermal_counts_raw = np.sum(total_counts_raw[thermal_mask])
thermal_err_relative = np.sqrt(thermal_counts_raw)/thermal_counts_raw
thermal_err_abs = thermal_err_relative*thermal_counts

intermediate_counts_raw = np.sum(total_counts_raw[intermediate_mask])
intermediate_err_rel = np.sqrt(intermediate_counts_raw)/intermediate_counts_raw
intermediate_err_abs = intermediate_err_rel*intermediate_counts

fast_counts_raw = np.sum(total_counts_raw[fast_mask])
fast_err_rel = np.sqrt(fast_counts_raw)/fast_counts_raw
fast_err_abs = fast_err_rel*fast_counts

cascade_counts_raw = np.sum(total_counts_raw[cascade_mask])
cascade_err_rel = np.sqrt(cascade_counts_raw)/cascade_counts_raw
cascade_err_abs = cascade_err_rel*cascade_counts

total_err_rel = np.sqrt(np.sum(total_counts_raw))/np.sum(total_counts_raw)
total_err_abs = total_err_rel*total_counts

print(f"\nThermal flux is {thermal_counts:.2f}"+chr(177)+f"{thermal_err_abs:.2f} /cm2/day")
print(f"Intermediate flux is {intermediate_counts:.2f}"+chr(177)+f"{intermediate_err_abs:.2f} /cm2/day")
print(f"Fast flux is {fast_counts:.2f}"+chr(177)+f"{fast_err_abs:.2f} /cm2/day")
print(f"Cascade flux is {cascade_counts:.2f}"+chr(177)+f"{cascade_err_abs:.2f} /cm2/day")
print(f"Total flux is {total_counts:.2f}"+chr(177)+f"{total_err_abs:.2f} /cm2/day")

plt.savefig("neutron_fluxes_kll.png",dpi=300,bbox_inches="tight")
plt.show()