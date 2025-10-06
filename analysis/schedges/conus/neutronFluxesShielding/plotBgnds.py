import ROOT
import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['axes.labelsize'] = 16
plt.rcParams['axes.titlesize'] = 16
plt.rcParams['xtick.labelsize'] = 14
plt.rcParams['ytick.labelsize'] = 14
plt.rcParams['legend.fontsize'] = 14

#############
##Load data##
#############
fname = "/home/sch52/code/paleo-bgnd-sim-build/conus_cry_outputs/complete/combined_*.root"
header_df = ROOT.RDataFrame("headerTree", fname)
neutron_tally_df = ROOT.RDataFrame("neutronTallyTree", fname)
recoilTree_df = ROOT.RDataFrame("recoilTree", fname)
print("Loaded data!")

###############
##Definitions##
###############
#Not currently used, but want to be able to plot spectra by PDG code
map_PDG_to_name = {
                   "1000010020":"deuteron",
                   "1000010030":"triton",
                   "1000020040":"alpha",
                   "1000030060":"Li-6",
                   "1000030070":"Li-7",
                   "1000090190":"F-19"
                   }

LiF_halflength_cm = 3.62
density_LiF = 2.635 #g/cm3

#Shielding/LiF parameters
outerShieldThickness = 50
outerVolumeNumber = 4
innerShieldThickness = 0
innerVolumeNumber = 14
shieldStepSize = 5
shielding_thicknesses_cm = np.arange(outerShieldThickness,innerShieldThickness-shieldStepSize,-shieldStepSize)
volumeNumber_array = np.asarray(range(outerVolumeNumber,innerVolumeNumber+1))

shielding_half_lengths = LiF_halflength_cm+shielding_thicknesses_cm
outer_surface_areas_cm2 = 6*np.power(shielding_half_lengths*2,2)

#Normalization parameters
cr_rate_per_cm2_per_s = 0.01053
overburden_area_cm2 = 3.762*100 * 3.762*100 

##############################
##Neutron vs. shielding plot##
##############################
tally_cols = neutron_tally_df.AsNumpy(["volumeNumbers","prevVolumeNumbers","entry_energy"])
#Flatten
entry_volumeNumbers = np.concatenate([np.asarray(volNum,dtype=int) for volNum in tally_cols["volumeNumbers"]])
entry_prevVolumeNumbers = np.concatenate([np.asarray(volNum,dtype=int) for volNum in tally_cols["prevVolumeNumbers"]])
entry_energies = np.concatenate([np.asarray(nrg,dtype=float) for nrg in tally_cols["entry_energy"]])

#Get normalization
numCRs = header_df.Sum("nps").GetValue()
nSeconds_simulated = (numCRs / overburden_area_cm2) / cr_rate_per_cm2_per_s
nDays_simulated = nSeconds_simulated/86400.
print(f"Simulated {numCRs} cosmic ray showers corresponding to {nSeconds_simulated/3600.:.2f} hours of exposure")

#Make plot--we require the neutron to enter the volume from a previous cell with a lower volume number (outside-in) to count
outside_in_mask = (entry_volumeNumbers>=entry_prevVolumeNumbers)
entry_volumeNumbers = entry_volumeNumbers[outside_in_mask]
entry_energies = entry_energies[outside_in_mask]

#Histogram volumeNumbers
counts,bins = np.histogram(entry_volumeNumbers,bins=len(volumeNumber_array),range=(outerVolumeNumber,innerVolumeNumber+1))
#Normalize, get bin centers, calculate errors
bin_centers = shielding_thicknesses_cm+shieldStepSize/2.

#Want to convert to counts / cm2 / day
normalized_counts = counts / (outer_surface_areas_cm2 * nDays_simulated)

#Plot
fig,ax = plt.subplots(figsize=(8,4))
ax.invert_xaxis()
ax.bar(bin_centers,normalized_counts,width = shieldStepSize)
#ax.errorbar(bin_centers,normalized_counts,yerr=yerrs,fmt='o',ecolor='k',markersize=0,color="k")
ax.set_xlabel("Water shielding thickness [cm$^2$]")
ax.set_ylabel("Counts/cm$^2$/day")
ax.set_xlim(left=outerShieldThickness,right=innerShieldThickness)
ax.set_yscale("log")
plt.tight_layout()
plt.savefig("neutronFlux.png",dpi=300,bbox_inches="tight")

for ivol,vol in enumerate(volumeNumber_array):
    print(vol,normalized_counts[ivol])

#Print output for patrick
outFile = open(f"incident_neutron_flux.txt","w")
for ivol,vol in enumerate(volumeNumber_array):
    line=""
    if ivol==0:
        line="#Shielding thickness (cm), Neutrons/cm2/day incident on this shield layer"
    else:
        line+=f"\n{shielding_thicknesses_cm[ivol]:.1f},{normalized_counts[ivol]:.3g}" 
    outFile.write(line)
outFile.close()
#########################################################
##Neutron spectrum incident on surface, incident on LiF##
#########################################################
nBins = 100
binMin = 1e-9
binMax = 10000.
binSize = (binMax-binMin)/float(nBins)

fig5,ax5 = plt.subplots(figsize=(8,4))

log_edges = np.logspace(np.log10(binMin), np.log10(binMax), nBins + 1)

for ivol, vol in enumerate(volumeNumber_array):
    volumeNumber_mask = (entry_volumeNumbers==vol)
    incident_energies = entry_energies[volumeNumber_mask]

    #Exterior plot
    counts,edges = np.histogram(incident_energies,bins=log_edges)
    bin_centers = np.sqrt(edges[:-1] * edges[1:])   
    bin_widths  = np.diff(edges)                    

    normalized_counts = counts / (outer_surface_areas_cm2[ivol] * nDays_simulated)
    print(vol,np.sum(normalized_counts))
    if ivol==0:
        ax5.step(
            edges[:-1],          # left edges
            normalized_counts,              # bin heights
            where="post",        # flat-top histogram look
            color = "k",
            label=f"{shielding_thicknesses_cm[ivol]} cm"
        )
    else:
        ax5.step(
            edges[:-1],          # left edges
            normalized_counts,              # bin heights
            where="post",  
            label=f"{shielding_thicknesses_cm[ivol]} cm"
        )

    #Print output for patrick
    outFile = open(f"incident_neutron_spectra_{shielding_thicknesses_cm[ivol]}cm.txt","w")
    for i,normCount in enumerate(normalized_counts):
        line=""
        if i==0:
            line="#Left edge (MeV),Right edge (MeV),Counts/cm2 shielding/bin/day"
        else:
            line+=f"\n{edges[i]:.3g},{edges[i+1]:.3g},{normalized_counts[i]:.3g}" 
        outFile.write(line)
    outFile.close()


#Plot
for i, line in enumerate(reversed(ax5.lines)):
    line.set_zorder(i)
ax5.set_xlabel("Energy of neutrons incident on shielding (MeV)")
ax5.set_ylabel("Counts/cm$^2$/day")
ax5.set_xscale("log")
ax5.legend(
    loc="upper left",            # place legend's upper left corner
    bbox_to_anchor=(1.02, 1),    # just outside the right edge, aligned to top
    borderaxespad=0,
)
plt.tight_layout()
plt.savefig("neutronSpectra.png",dpi=300,bbox_inches="tight")

#######################
##Recoil distribution##
#######################
recoil_cols = recoilTree_df.AsNumpy(["pdgCode","energy"])
#Flatten
pdgCodes = np.concatenate([np.asarray(pdgCode,dtype=int) for pdgCode in recoil_cols["pdgCode"]])
energies_MeV = np.concatenate([np.asarray(nrg,dtype=float) for nrg in recoil_cols["energy"]])
energies = energies_MeV*1000

#Get only nuclear recoils
ion_recoil_mask = (abs(pdgCodes) > 2112) #Greater than neutron PDG code
ion_energies = energies[ion_recoil_mask]

#Make full hist
counts,bins = np.histogram(ion_energies,bins=200,range=(0,10000))
#Normalize, get errors
bin_centers = (bins[1:]+bins[:-1])/2.
#Want to convert to counts / kg / day
LiF_kg = np.power(LiF_halflength_cm*2,3)*density_LiF / 1000.
normalized_counts = counts / (LiF_kg * nDays_simulated)
yerrs = np.asarray([np.sqrt(counts[i])/counts[i] * normalized_counts[i] if counts[i]!=0 else 0 for i in range(0,counts.size)])

#Print output for patrick
outFile = open("lif_cosmic_recoil_spectrum_full.txt","w")
for i,bin_center in enumerate(bin_centers):
    line=""
    if ivol==0:
        line="#Recoil energy (keV), Counts/50 keV/kg/day"
    else:
        line=f"\n{bin_center:.3f},{normalized_counts[i]:.2g}"
    outFile.write(line)
outFile.close()

#Plot
fig2,ax2=plt.subplots(figsize=(8,4))
ax2.step(bins[:-1], normalized_counts, where="post", color="black")
ax2.errorbar(bin_centers,normalized_counts,yerr=yerrs,fmt="o",color="k",markersize=0)
ax2.set_yscale("log")
ax2.set_xlabel("Nuclear recoil energy [keV]")
ax2.set_ylabel("Counts/50 keV/kg/day")
ax2.set_xlim(left=0)
plt.tight_layout()
plt.savefig("spectrum_full.png",dpi=300,bbox_inches="tight")

#Make zoomed hist
counts,bins = np.histogram(ion_energies,bins=200,range=(0,20))
#Normalize, get errors
bin_centers = (bins[1:]+bins[:-1])/2.
#Want to convert to counts / kg / day
LiF_kg = np.power(LiF_halflength_cm*2,3)*density_LiF / 1000.
normalized_counts = counts / (LiF_kg * nDays_simulated)
yerrs = np.asarray([np.sqrt(counts[i])/counts[i] * normalized_counts[i] if counts[i]!=0 else 0 for i in range(0,counts.size)])

#Print output for patrick
outFile = open("lif_cosmic_recoil_spectrum.txt","w")
for i,bin_center in enumerate(bin_centers):
    line=""
    if ivol==0:
        line="#Recoil energy (keV), Counts/100 eV/kg/day"
    else:
        line=f"\n{bin_center:.3f},{normalized_counts[i]:.2g}"
    outFile.write(line)
outFile.close()

#Plot
fig3,ax3=plt.subplots(figsize=(8,4))
ax3.step(bins[:-1], normalized_counts, where="post", color="black")
ax3.errorbar(bin_centers,normalized_counts,yerr=yerrs,fmt="o",color="k",markersize=0)
ax3.set_yscale("log")
ax3.set_xlabel("Nuclear recoil energy [keV]")
ax3.set_ylabel("Counts/100 eV/kg/day")
ax3.set_xlim(left=0)
plt.tight_layout()
plt.savefig("spectrum.png",dpi=300,bbox_inches="tight")
plt.show()

