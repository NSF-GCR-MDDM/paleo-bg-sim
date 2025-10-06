import numpy as np
import matplotlib.pyplot as plt
import ROOT

#1. Load up files into RDataFrame
df = ROOT.RDataFrame("recoilTree", "/home/sch52/code/paleo-bgnd-sim-build/conus_cry_outputs/*.root")
cols = df.AsNumpy(["energy","pdgCode","time"])

#2. Step through events--sum depositions within 10us. #TODO: quench NRs
integration_time_ns = 10000
edep_keV = []
for ihist in range(0,len(cols["energy"])):
    for idep in range(0,len(cols["energy"][ihist])):
        edep_keV.append(cols["energy"][ihist][idep])

#3. Normalize and plot
plt.hist(edep_keV,bins=35,range=(0.5,4),histtype="step")
plt.yscale("log")
plt.show()