import sys
import numpy as np
import uproot as up
import ROOT
import math
#Open file
inpFilename = sys.argv[1]
inpFile = up.open(inpFilename)

#Get trees, can selectively load branches by passing in arrays=["names"] into arrays() call
headerTree = inpFile["headerTree"].arrays(library="np")
norm = headerTree["meiHimeFluxNormalization_per_cm2_per_s"]
nps = headerTree["nps"]

tallyTree = inpFile["neutronTallyTree"].arrays(library="np")
multiplicityHist = ROOT.TH1D("multiplicityHist",";Multiplicity;Neutrons/cm^{2}/sec",34,0,102)
distanceHist = ROOT.TH1D("distanceHist",";Distance (m);Fraction of neutrons",20,0,5)
energyHist = ROOT.TH1D("energyHist",";Energy (GeV);Neutrons/cm^{2}/GeV/sec",70,0,3.5)
cosAngleHist = ROOT.TH1D("cosAngleHist",";cos(theta);Neutrons/cm^{2}/sec",100,-1,1)

scaleFactor = norm / nps #units of /cm2/sec


for entry in range(0,len(tallyTree["numNeutronsEntered"])):
    multiplicityHist.Fill(tallyTree["numNeutronsEntered"][entry])
    for i in range(0,len(tallyTree["entry_energy"][entry])):
        energyHist.Fill(tallyTree["entry_energy"][entry][i]/1000.)
        distanceHist.Fill(tallyTree["distanceToMuonTrack"][entry][i]/1000.)
        cosAngleHist.Fill(math.cos(tallyTree["angleRelMuon"][entry][i]))
    
multiplicityHist.Scale(scaleFactor)
energyHist.Scale(scaleFactor)
distanceHist.Scale(scaleFactor)
cosAngleHist.Scale(scaleFactor)
distanceHist.Scale(1./distanceHist.Integral())

multiplicityHist.SetStats(0)
energyHist.SetStats(0)
distanceHist.SetStats(0)
cosAngleHist.SetStats(0)

c1=ROOT.TCanvas()
c1.Divide(2,2)
c1.cd(1)
multiplicityHist.Draw("hist")
c1.cd(2)
energyHist.Draw("hist")
c1.cd(3)
distanceHist.Draw("hist")
c1.cd(4)
cosAngleHist.Draw("hist")
c1.Modified()
c1.Update()


secondaryTree = inpFile["MINTree"].arrays(library="np")
MINMultiplicityHist = ROOT.TH1D("MINMultiplicityHist",";Multiplicity;Neutrons/cm^{2}/sec",34,0,102)
MINDistanceHist = ROOT.TH1D("distMINDistanceHistanceHist",";Distance (cm);Fraction of Neutrons",20,0,5)
MINEnergyHist = ROOT.TH1D("MINEnergyHist",";Energy (GeV);Neutrons/cm^{2}/GeV/sec",70,0,3.5)
MINCosAngleHist = ROOT.TH1D("MINCosAngleHist",";cos(theta);Neutrons/cm^{2}/sec",100,-1,1)

MINMultiplicityHist.SetStats(0)
MINEnergyHist.SetStats(0)
MINDistanceHist.SetStats(0)
MINCosAngleHist.SetStats(0)

for entry in range(0,len(secondaryTree["multiplicity"])):
    MINMultiplicityHist.Fill(secondaryTree["multiplicity"][entry])
    for i in range(0,len(secondaryTree["energy"][entry])):
        MINEnergyHist.Fill(secondaryTree["energy"][entry][i]/1000.)
        MINDistanceHist.Fill(secondaryTree["distanceToMuonTrack"][entry][i]/1000.)
        MINCosAngleHist.Fill(math.cos(secondaryTree["angleRelToMuon"][entry][i]))

MINMultiplicityHist.Scale(scaleFactor)
MINEnergyHist.Scale(scaleFactor)
MINDistanceHist.Scale(scaleFactor)
MINCosAngleHist.Scale(scaleFactor)

MINDistanceHist.Scale(1./MINDistanceHist.Integral())

c2=ROOT.TCanvas("C2","c2")
c2.Divide(2,2)
c2.cd(1)
MINMultiplicityHist.Draw("hist")
c2.cd(2)
MINEnergyHist.Draw("hist")
c2.cd(3)
MINDistanceHist.Draw("hist")
c2.cd(4)
MINCosAngleHist.Draw("hist")
c2.Modified()
c2.Update()
try:
    input("Press enter")
except SyntaxError:
    pass