#include <iostream>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TAxis.h>

void neutron_energydist(const char* fi) {
    TFile *file = TFile::Open(fi); // Open the ROOT file
    
    if (file && !file->IsZombie()) {
        // Retrieve the histogram from the ROOT file
        TH1F *hist = (TH1F*)file->Get("neutronKin");
        
        if (hist) {
            // Modify the axis titles
            hist->SetXTitle("Energy (MeV)");
            hist->SetYTitle("Counts (a.u.)");
            
            // hist->Scale(1.0 / hist->Integral()); // Normalize to unit area (if needed)
            
            // Create a canvas to draw the histogram
            TCanvas *canvas = new TCanvas("canvas", "Canvas for Plotting", 800, 600);
            hist->Draw();
			canvas->SetLogy();
            
            // You can save the canvas as an image (optional)
            canvas->SaveAs("neutron_KE.png");
        } else {
            std::cerr << "Histogram not found in the file." << std::endl;
        }

        // Close the ROOT file
        file->Close();
    } else {
        std::cerr << "File could not be opened." << std::endl;
    }
}