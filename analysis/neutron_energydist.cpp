#include <iostream>
#include <TFile.h>
#include <TF1.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TAxis.h>

void neutron_energydist(TFile* fi) {
    if (fi && !fi->IsZombie()) {
        // Retrieve the histogram from the ROOT file
        TH1F *hist = (TH1F*)fi->Get("neutronKin");
        
        if (hist) {
            // Modify the axis titles
            hist->SetXTitle("Energy (MeV)");
            hist->SetYTitle("Counts (a.u.)");
            
            // hist->Scale(1.0 / hist->Integral()); // Normalize to unit area (if needed)
            
            // Create a canvas to draw the histogram
            TCanvas *canvas = new TCanvas("canvas", "Canvas for Plotting", 800, 600);
            hist->Draw();
			canvas->SetLogy();

			int nBins = hist->GetNbinsX();
			const TArrayD *xaxis = hist->GetXaxis()->GetXbins();

			TF1 *func = new TF1("func", "(-10000/3500) * x + 10000",
								hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());

			TGraph *graph = new TGraph();

			for (int i = 1; i <= nBins; ++i) {
				double binCenter = hist->GetBinCenter(i);
				double funcValue = func->Eval(binCenter);
				graph->SetPoint(i - 1, binCenter, funcValue);
			}

			graph->SetMarkerStyle(20);
			graph->SetMarkerColor(kBlue);
			graph->Draw("P SAME");  // Overlay function values as points

			canvas->Update();
			canvas->SaveAs("neutron_KE.png");
        } else {
            std::cerr << "Histogram not found in the file." << std::endl;
        }
    } else {
        std::cerr << "File could not be opened." << std::endl;
    }
}