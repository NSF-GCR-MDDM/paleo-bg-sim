#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TAxis.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TChain.h"
#include "TCut.h"
#include <iostream>
#include <string>

/* [STYLE COMMENTS]
* - SUFFIXES: SB = Sudbury, GS = Gran Sasso
* - TREE NAMING: gran = Gran Sasso, bert and bic are Sudbury simulations with different Physics lists
* - Replace file names with output files
*/

// SITE-RELATED CONSTANTS
const double SUDBURY_DEPTH = 5.993; // kilometres
const double SUDBURY_MUON_NORM = 3.867e-10;
const double GRAN_SASSO_DEPTH = 3.1; // kilometres
const double GRAN_SASSO_MUON_NORM = 2.568e-8;
const double GAMMA = 3.77;
const double EPSILON = 693;
const double B = 0.4;

// HISTOGRAM CONSTANTS
const int NUM_BINS = 70;
const int NUM_BINS_THETA = 3000;
const int NUM_BINS_RE = 100;
const int NUM_BINS_RE2 = 34;

// MEI-HIME FIT CONSTANTS
const double E_MUON_SB = 356.;
const double A0_SB = 7.774;
const double A1_SB = 2.134;
const double A2_SB = -2.939e-16;
const double A3_SB = 2.859;
const double A_MU_SB = 2.492e-11; // TBC

const double E_MUON_GS = 278.;
const double A0_GS = 7.828;
const double A1_GS = 2.23;
const double A2_GS = -7.505e-15;
const double A3_GS = 2.831;
const double A_MU_GS = 1.246e-9; // TBC

// MUON FLUX CONSTANTS
const int MUON_NPS = 1.2e6;
const int WORLD_AREA = 2000 * 2000; // cm
const double MUON_NPS_PER_CM2 = static_cast<double>(MUON_NPS)/static_cast<double>(WORLD_AREA);
const double SIMTIME_SB = MUON_NPS_PER_CM2/SUDBURY_MUON_NORM;
const double TIME_SCALING_FACTOR_SB = 1./SIMTIME_SB;
const double SIMTIME_GS = MUON_NPS_PER_CM2/GRAN_SASSO_MUON_NORM;
const double TIME_SCALING_FACTOR_GS = 1./SIMTIME_GS;

const int CAVITY_AREA = 600 * 600; // cm
const double UNITS_SCALING = 1e-9;
const double FLUX_SCALE_FACTOR_SB = TIME_SCALING_FACTOR_SB/CAVITY_AREA;
const double FLUX_SCALE_FACTOR_GS = TIME_SCALING_FACTOR_GS/CAVITY_AREA;

void ScaleXAxis(TH1F *myHist, double scalar) {
    myHist->GetXaxis()->Set(NUM_BINS, myHist->GetXaxis()->GetXmin()/scalar, myHist->GetXaxis()->GetXmax()/scalar);
}

double neutron_flux_sb(double E) {
    return (A_MU_SB * (TMath::Exp(-A0_SB*E)/E + (0.324 - 0.641*TMath::Exp(-0.014*E_MUON_SB)) * TMath::Exp(-A1_SB*E)) + A2_SB * TMath::Power(E, -A3_SB));
}

double neutron_flux_gs(double E) {
    return (A_MU_GS * (TMath::Exp(-A0_GS*E)/E + (0.324 - 0.641*TMath::Exp(-0.014*E_MUON_GS)) * TMath::Exp(-A1_GS*E)) + A2_GS * TMath::Power(E, -A3_GS));
}

double neutron_ang_sb(double cos) {
    return (1./(TMath::Power(1 - cos, 0.482 * TMath::Power(E_MUON_SB, 0.045)) + 0.832 * TMath::Power(E_MUON_SB, -0.152)));
}

double neutron_ang_gs(double cos) {
    return (1./(TMath::Power(1 - cos, 0.482 * TMath::Power(E_MUON_GS, 0.045)) + 0.832 * TMath::Power(E_MUON_GS, -0.152)));
}

double neutron_mult_sb(double M) {
    return (TMath::Exp(-M * 0.321 * TMath::Power(E_MUON_SB, -0.247)) + 318.1 * TMath::Exp(-0.01421 * E_MUON_SB) * TMath::Exp(-M * 2.02 * TMath::Exp(-0.006959 * E_MUON_SB)));
}

double neutron_mult_gs(double M) {
    return (TMath::Exp(-M * 0.321 * TMath::Power(E_MUON_GS, -0.247)) + 318.1 * TMath::Exp(-0.01421 * E_MUON_GS) * TMath::Exp(-M * 2.02 * TMath::Exp(-0.006959 * E_MUON_GS)));
}

int target_bin(double theta) {
    return (TMath::Floor((NUM_BINS_RE/2.) * TMath::Cos(theta)) + NUM_BINS_RE/2 + 1);
}

void physics_lists() {
    TFile *bert = TFile::Open("bert_sudbury_merged.root", "READ");
    TFile *bic = TFile::Open("bic_sudbury_merged.root", "READ");
    TFile *gran = TFile::Open("bert_gran_sasso_merged.root", "READ");

    // Header Trees
    TTree *bert_header = (TTree*)bert->Get("headerTree;1");
    TTree *bic_header = (TTree*)bic->Get("headerTree;1");
    TTree *gran_header = (TTree*)gran->Get("headerTree;1");
    
    // [1] ENERGY SPECTRA
    TH1F *h_energy_bert = new TH1F("h_energy_bert", "Neutron Energy Spectra by Location and Physics List; Energy (GeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 35e2);
    TTree *bert_tree = (TTree*)bert->Get("neutronTallyTree;1");
    TH1F *h_energy_bic = new TH1F("h_energy_bic", "Neutron Energy Spectra by Location and Physics List; Energy (GeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 35e2);
    TTree *bic_tree = (TTree*)bic->Get("neutronTallyTree;1");
    TH1F *h_energy_gs = new TH1F("h_energy_gs", "Neutron Energy Spectra by Location and Physics List; Energy (GeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 35e2);
    TTree *gs_tree = (TTree*)gran->Get("neutronTallyTree;1");
   
    bert_tree->Draw("entry_energy>>h_energy_bert");
    bic_tree->Draw("entry_energy>>h_energy_bic");
    gs_tree->Draw("entry_energy>>h_energy_gs");

    // NEUTRON-FLUX RELATED CONSTANTS
    const int NEUTRON_NPS_BERT = h_energy_bert->GetEntries();
    const int NEUTRON_NPS_BIC = h_energy_bic->GetEntries();
    const int NEUTRON_NPS_GS = h_energy_gs->GetEntries();
    const double NEUTRON_NORM_BERT = NEUTRON_NPS_BERT * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_BIC = NEUTRON_NPS_BIC * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_GS = NEUTRON_NPS_GS * FLUX_SCALE_FACTOR_GS;

    // [1.5] ENERGY THRESHOLD CATEGORIZATION
    TString cutEnergies[4] = { "0.", "1.", "10.", "100."};
    TCut thresholds[4] = { "entry_energy > 0.", 
                           "entry_energy > 1.",
                           "entry_energy > 10.",
                           "entry_energy > 100."}; // MeV
    double numNeutrons[4];
    double meiHimeValsSB[4] = {0.054, 0.020, 0.018, 0.005};
    double meiHimeValsGS[4] = {2.72, 0.81, 0.73, 0.201};
    TH1F *buffer = new TH1F("buffer", "bufferTitle", NUM_BINS, 0, 35e2);

    for (int i = 0; i < 4; i++) {
        bert_tree->Draw("entry_energy>>buffer", thresholds[i]);
        numNeutrons[i] = buffer->GetEntries();
        buffer->Reset();
    }

    double flux;
    for (int i = 0; i < 4; i++) {
        flux = numNeutrons[i] * FLUX_SCALE_FACTOR_SB;
        flux /= UNITS_SCALING;
        cout << "GEANT4: BERT Flux at Sudbury > " << cutEnergies[i] << " MeV is " << flux << "e-09 n/cm2/sec, "
             << "Mei & Hime is " << meiHimeValsSB[i] << "e-09 n/cm2/sec." << endl;
    }

    for (int i = 0; i < 4; i++) {
        bic_tree->Draw("entry_energy>>buffer", thresholds[i]);
        numNeutrons[i] = buffer->GetEntries();
        buffer->Reset();
    }

    for (int i = 0; i < 4; i++) {
        flux = numNeutrons[i] * FLUX_SCALE_FACTOR_SB;
        flux /= UNITS_SCALING;
        cout << "GEANT4: BIC Flux at Sudbury > " << cutEnergies[i] << " MeV is " << flux << "e-09 n/cm2/sec, "
             << "Mei & Hime is " << meiHimeValsSB[i] << "e-09 n/cm2/sec." << endl;
    }

    for (int i = 0; i < 4; i++) {
        gs_tree->Draw("entry_energy>>buffer", thresholds[i]);
        numNeutrons[i] = buffer->GetEntries();
        buffer->Reset();
    }

    for (int i = 0; i < 4; i++) {
        flux = numNeutrons[i] * FLUX_SCALE_FACTOR_GS;
        flux /= UNITS_SCALING;
        cout << "GEANT4: BERT Flux at Gran Sasso > " << cutEnergies[i] << " MeV is " << flux << "e-09 n/cm2/sec, "
             << "Mei & Hime is " << meiHimeValsGS[i] << "e-09 n/cm2/sec." << endl;
    }

    // reproducing mei-hime
    double E_av;
    TH1F *h_energy_meihime_sb = new TH1F("h_energy_meihime_sb", "Neutron Energy Spectra by Location and Physics List; Energy (GeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 3.5);
    for (int i = 0; i < NUM_BINS; i++) {
        E_av = i * 4./NUM_BINS + 2./NUM_BINS;
        h_energy_meihime_sb->SetBinContent(i + 1, neutron_flux_sb(E_av));
    }

    TH1F *h_energy_meihime_gs = new TH1F("h_energy_meihime_gs", "Neutron Energy Spectra by Location and Physics List; Energy (GeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 3.5);
    for (int i = 0; i < NUM_BINS; i++) {
        E_av = i * 4./NUM_BINS + 2./NUM_BINS;
        h_energy_meihime_gs->SetBinContent(i + 1, neutron_flux_gs(E_av));
    }

    // rescaling x-axis
    ScaleXAxis(h_energy_bert, 1000.0);
    ScaleXAxis(h_energy_bic, 1000.0);
    ScaleXAxis(h_energy_gs, 1000.0);

    // normalizing
    h_energy_bert->Scale(NEUTRON_NORM_BERT/h_energy_bert->Integral(), "width");
    h_energy_bic->Scale(NEUTRON_NORM_BIC/h_energy_bic->Integral(), "width");
    h_energy_gs->Scale(NEUTRON_NORM_GS/h_energy_gs->Integral(), "width");

    // create canvas 
    TCanvas *c1 = new TCanvas("c1", "Neutron Tally Energy Spectrum", 800, 600);
    gPad->SetLogy();
    gStyle->SetOptStat(0);
    TCanvas *c2 = new TCanvas("c2", "Neutron Tally Angular Distribution", 800, 600);
    c2->cd();
    gPad->SetLogy();
    gStyle->SetOptStat(0);
    TCanvas *c3 = new TCanvas("c3", "Neutron Tally Multiplicity", 800, 600);
    c3->cd();
    gPad->SetLogy();
    gStyle->SetOptStat(0);
    TCanvas *c4 = new TCanvas("c4", "Neutron Tally Energy Spectrum: Low Energy", 800, 600);
    c4->cd();
    gPad->SetLogy();
    gStyle->SetOptStat(0);
    c1->cd();

    // create legend
    TLegend *l1 = new TLegend(0.78, 0.78, 0.9, 0.9);
    l1->AddEntry(h_energy_bert, "Sudbury: Geant4 (BERT)", "l");
    l1->AddEntry(h_energy_bic, "Sudbury: Geant4 (BIC)", "l");
    l1->AddEntry(h_energy_gs, "Gran Sasso: Geant4 (BERT)", "l");
    l1->AddEntry(h_energy_meihime_sb, "Sudbury: Mei-Hime Fit", "l");
    l1->AddEntry(h_energy_meihime_gs, "Gran Sasso: Mei-Hime Fit", "l");
    l1->SetFillColor(kWhite);
    l1->SetHeader("Data");

    // draw
    h_energy_bert->SetLineColor(kBlue);
    h_energy_bert->SetLineWidth(3);
    h_energy_bic->SetLineColor(kRed);
    h_energy_bic->SetLineWidth(3);
    h_energy_gs->SetLineColor(kViolet);
    h_energy_gs->SetLineWidth(3);
    h_energy_meihime_sb->SetLineColor(kGreen);
    h_energy_meihime_sb->SetLineWidth(3);
    h_energy_meihime_sb->SetLineStyle(5);
    h_energy_meihime_gs->SetLineColor(kCyan);
    h_energy_meihime_gs->SetLineWidth(3);
    h_energy_meihime_gs->SetLineStyle(5);
    h_energy_meihime_sb->GetYaxis()->SetRangeUser(10e-16, 10e-7);
    h_energy_meihime_sb->Draw("HIST");
    h_energy_meihime_gs->Draw("HIST SAME");
    h_energy_bert->Draw("HIST SAME");
    h_energy_bic->Draw("HIST SAME");
    h_energy_gs->Draw("HIST SAME");
    l1->Draw();

    c1->Update();
    c2->cd();

    // [2] ANGULAR DISTRIBUTION
    TH1F *reqd_bert = new TH1F("reqd_bert", "Neutron Angular Distribution by Location and Physics List; cos(#theta); Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE, -1., 1.);
    TH1F *reqd_bic = new TH1F("reqd_bic", "Neutron Angular Distribution by Location and Physics List; cos(#theta); Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE, -1., 1.);
    TH1F *reqd_gs = new TH1F("reqd_gs", "Neutron Angular Distribution by Location and Physics List; cos(#theta); Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE, -1., 1.);
    
    TH1F *h_ang_bert = new TH1F("h_ang_bert", "Neutron Angular Distribution by Location and Physics List; #theta; Neutrons (cm^{-2}s^{-1})", NUM_BINS_THETA, 0, 3.142);
    TH1F *h_ang_bic = new TH1F("h_ang_bic", "Neutron Angular Distribution by Location and Physics List; #theta; Neutrons (cm^{-2}s^{-1})", NUM_BINS_THETA, 0, 3.142);
    TH1F *h_ang_gs = new TH1F("h_ang_gs", "Neutron Angular Distribution by Location and Physics List; #theta; Neutrons (cm^{-2}s^{-1})", NUM_BINS_THETA, 0, 3.142);
   
    // define cut
    TCut remove_low_energy = "entry_energy > 3.";

    bert_tree->Draw("angleRelMuon>>h_ang_bert", remove_low_energy);
    bic_tree->Draw("angleRelMuon>>h_ang_bic", remove_low_energy);
    gs_tree->Draw("angleRelMuon>>h_ang_gs", remove_low_energy);

    // POST-CUT NEUTRON-FLUX RELATED CONSTANTS
    const int NEUTRON_NPS_BERT_ANG = h_ang_bert->GetEntries();
    const int NEUTRON_NPS_BIC_ANG = h_ang_bic->GetEntries();
    const int NEUTRON_NPS_GS_ANG = h_ang_gs->GetEntries();
    const double NEUTRON_NORM_BERT_ANG = NEUTRON_NPS_BERT_ANG * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_BIC_ANG = NEUTRON_NPS_BIC_ANG * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_GS_ANG = NEUTRON_NPS_GS_ANG * FLUX_SCALE_FACTOR_GS;

    // standard -> reqd data transfer
    int currentBinContent = 0;
    int targetBinNumber = 0;
    for (int i = 0; i < NUM_BINS_THETA; i++) {
        currentBinContent = h_ang_bert->GetBinContent(i + 1);
        targetBinNumber = target_bin(h_ang_bert->GetBinLowEdge(i + 1) + h_ang_bert->GetBinWidth(i + 1)/2.);
        reqd_bert->AddBinContent(targetBinNumber, currentBinContent);
    }
    for (int i = 0; i < NUM_BINS_THETA; i++) {
        currentBinContent = h_ang_bic->GetBinContent(i + 1);
        targetBinNumber = target_bin(h_ang_bic->GetBinLowEdge(i + 1) + h_ang_bic->GetBinWidth(i + 1)/2.);
        reqd_bic->AddBinContent(targetBinNumber, currentBinContent);
    }
    for (int i = 0; i < NUM_BINS_THETA; i++) {
        currentBinContent = h_ang_gs->GetBinContent(i + 1);
        targetBinNumber = target_bin(h_ang_gs->GetBinLowEdge(i + 1) + h_ang_gs->GetBinWidth(i + 1)/2.);
        reqd_gs->AddBinContent(targetBinNumber, currentBinContent);
    }

    // reproducing mei-hime
    double cos_av;
    TH1F *h_ang_meihime_sb = new TH1F("h_ang_meihime_sb", "Neutron Angular Distribution by Location and Physics List; cos(#theta); Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE, -1, 1);
    for (int i = 0; i < NUM_BINS_RE; i++) {
        cos_av = h_ang_meihime_sb->GetBinLowEdge(i + 1) + h_ang_meihime_sb->GetBinWidth(i + 1)/2.;
        h_ang_meihime_sb->SetBinContent(i + 1, neutron_ang_sb(cos_av));
    }
    TH1F *h_ang_meihime_gs = new TH1F("h_ang_meihime_gs", "Neutron Angular Distribution by Location and Physics List; cos(#theta); Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE, -1, 1);
    for (int i = 0; i < NUM_BINS_RE; i++) {
        cos_av = h_ang_meihime_gs->GetBinLowEdge(i + 1) + h_ang_meihime_gs->GetBinWidth(i + 1)/2.;
        h_ang_meihime_gs->SetBinContent(i + 1, neutron_ang_gs(cos_av));
    }

    // normalizing
    reqd_bert->Scale(NEUTRON_NORM_BERT_ANG/reqd_bert->Integral(),"width");
    reqd_bic->Scale(NEUTRON_NORM_BIC_ANG/reqd_bic->Integral(),"width");
    reqd_gs->Scale(NEUTRON_NORM_GS_ANG/reqd_gs->Integral(),"width");
    h_ang_meihime_sb->Scale(0.054e-9/h_ang_meihime_sb->Integral(),"width");
    h_ang_meihime_gs->Scale(2.72e-9/h_ang_meihime_gs->Integral(),"width");

    // create legend
    TLegend *l2 = new TLegend(0.78, 0.78, 0.9, 0.9);
    l2->AddEntry(reqd_bert, "Sudbury: Geant4 (BERT)", "l");
    l2->AddEntry(reqd_bic, "Sudbury: Geant4 (BIC)", "l");
    l2->AddEntry(reqd_gs, "Gran Sasso: Geant4 (BERT)", "l");
    l2->AddEntry(h_ang_meihime_sb, "Sudbury: Mei-Hime Fit", "l");
    l2->AddEntry(h_ang_meihime_gs, "Gran Sasso: Mei-Hime Fit", "l");
    l2->SetFillColor(kWhite);
    l2->SetHeader("Data");

    // draw
    reqd_bert->SetLineColor(kBlue);
    reqd_bert->SetLineWidth(3);
    reqd_bic->SetLineColor(kRed);
    reqd_bic->SetLineWidth(3);
    reqd_gs->SetLineColor(kViolet);
    reqd_gs->SetLineWidth(3);
    h_ang_meihime_sb->SetLineColor(kGreen);
    h_ang_meihime_sb->SetLineWidth(3);
    h_ang_meihime_sb->SetLineStyle(5);
    h_ang_meihime_gs->SetLineColor(kCyan);
    h_ang_meihime_gs->SetLineWidth(3);
    h_ang_meihime_gs->SetLineStyle(5);
    reqd_bert->GetYaxis()->SetRangeUser(10e-13, 10e-9);
    reqd_bert->Draw("HIST");
    reqd_bic->Draw("HIST SAME");
    reqd_gs->Draw("HIST SAME");
    h_ang_meihime_sb->Draw("HIST SAME");
    h_ang_meihime_gs->Draw("HIST SAME");
    l2->Draw();

    c2->Update();
    c3->cd();

    // [3] MULTIPLICITY
    TH1F *h_mult_bert = new TH1F("h_mult_bert", "Neutron Multiplicity by Location and Physics List; Multiplicity; Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE2, 0, 102);
    TH1F *h_mult_bic = new TH1F("h_mult_bic", "Neutron Multiplicity by Location and Physics List; Multiplicity; Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE2, 0, 102);
    TH1F *h_mult_gs = new TH1F("h_mult_gs", "Neutron Multiplicity by Location and Physics List; Multiplicity; Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE2, 0, 102);

    bert_tree->Draw("numNeutronsEntered>>h_mult_bert");
    bic_tree->Draw("numNeutronsEntered>>h_mult_bic");
    gs_tree->Draw("numNeutronsEntered>>h_mult_gs");

    // NEUTRON-FLUX RELATED CONSTANTS
    const int NEUTRON_NPS_BERT_MULT = h_mult_bert->GetEntries();
    const int NEUTRON_NPS_BIC_MULT = h_mult_bic->GetEntries();
    const int NEUTRON_NPS_GS_MULT = h_mult_gs->GetEntries();
    const double NEUTRON_NORM_BERT_MULT = NEUTRON_NPS_BERT_MULT * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_BIC_MULT = NEUTRON_NPS_BIC_MULT * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_GS_MULT = NEUTRON_NPS_GS_MULT * FLUX_SCALE_FACTOR_GS;

    // reproducing meihime
    double mult_av;
    TH1F *h_mult_meihime_sb = new TH1F("h_mult_meihime_sb", "Neutron Multiplicity by Location and Physics List; Multiplicity; Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE2, 0, 102);
    for (int i = 0; i < NUM_BINS_RE2; i++) {
        mult_av = h_mult_meihime_sb->GetBinLowEdge(i + 1) + h_mult_meihime_sb->GetBinWidth(i + 1)/2.;
        h_mult_meihime_sb->SetBinContent(i + 1, neutron_mult_sb(mult_av));
    }

    TH1F *h_mult_meihime_gs = new TH1F("h_mult_meihime_gs", "Neutron Multiplicity by Location and Physics List; Multiplicity; Neutrons (cm^{-2}s^{-1})", NUM_BINS_RE2, 0, 102);
    for (int i = 0; i < NUM_BINS_RE2; i++) {
        mult_av = h_mult_meihime_gs->GetBinLowEdge(i + 1) + h_mult_meihime_gs->GetBinWidth(i + 1)/2.;
        h_mult_meihime_gs->SetBinContent(i + 1, neutron_mult_gs(mult_av));
    }

    // normalizing
    h_mult_bert->Scale(NEUTRON_NORM_BERT_MULT/h_mult_bert->Integral(),"width");
    h_mult_bic->Scale(NEUTRON_NORM_BIC_MULT/h_mult_bic->Integral(),"width");
    h_mult_gs->Scale(NEUTRON_NORM_GS_MULT/h_mult_gs->Integral(),"width");
    h_mult_meihime_sb->Scale(0.054e-9/h_mult_meihime_sb->Integral(),"width");
    h_mult_meihime_gs->Scale(2.72e-9/h_mult_meihime_gs->Integral(),"width");

    // create legend
    TLegend *l3 = new TLegend(0.78, 0.78, 0.9, 0.9);
    l3->AddEntry(h_mult_bert, "Sudbury: Geant4 (BERT)", "l");
    l3->AddEntry(h_mult_bic, "Sudbury: Geant4 (BIC)", "l");
    l3->AddEntry(h_mult_gs, "Gran Sasso: Geant4 (BERT)", "l");
    l3->AddEntry(h_mult_meihime_sb, "Sudbury: Mei-Hime Fit", "l");
    l3->AddEntry(h_mult_meihime_gs, "Gran Sasso: Mei-Hime Fit", "l");
    l3->SetFillColor(kWhite);
    l3->SetHeader("Data");

    // draw
    h_mult_bert->SetLineColor(kBlue);
    h_mult_bert->SetLineWidth(3);
    h_mult_bic->SetLineColor(kRed);
    h_mult_bic->SetLineWidth(3);
    h_mult_gs->SetLineColor(kViolet);
    h_mult_gs->SetLineWidth(3);
    h_mult_meihime_gs->SetLineColor(kCyan);
    h_mult_meihime_gs->SetLineWidth(3);
    h_mult_meihime_gs->SetLineStyle(5);
    h_mult_meihime_sb->SetLineColor(kGreen);
    h_mult_meihime_sb->SetLineWidth(3);
    h_mult_meihime_sb->SetLineStyle(5);
    h_mult_meihime_sb->GetYaxis()->SetRangeUser(10e-16, 10e-8);
    h_mult_meihime_sb->Draw("HIST");
    h_mult_meihime_gs->Draw("HIST SAME");
    h_mult_bert->Draw("HIST SAME");
    h_mult_bic->Draw("HIST SAME");
    h_mult_gs->Draw("HIST SAME");
    l3->Draw();

    c3->Update();
    c4->cd();

    // [4] ENERGY SPECTRA: Zoomed In
    TH1F *h_lowenergy_bert = new TH1F("h_lowenergy_bert", "Low Energy Neutron Energy Spectra; Energy (MeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 50);
    TH1F *h_lowenergy_bic = new TH1F("h_lowenergy_bic", "Low Energy Neutron Energy Spectra; Energy (MeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 50);
    TH1F *h_lowenergy_gs = new TH1F("h_lowenergy_gs", "Low Energy Neutron Energy Spectra; Energy (MeV); Neutron Flux (cm^{-2}s^{-1})", NUM_BINS, 0, 50);
    TCut remove_high_energy = "entry_energy <= 50.";
    bert_tree->Draw("entry_energy>>h_lowenergy_bert", remove_high_energy);
    bic_tree->Draw("entry_energy>>h_lowenergy_bic", remove_high_energy);
    gs_tree->Draw("entry_energy>>h_lowenergy_gs", remove_high_energy);

    // normalization constants
    const int NEUTRON_NPS_BERT_LE = h_lowenergy_bert->GetEntries();
    const int NEUTRON_NPS_BIC_LE = h_lowenergy_bic->GetEntries();
    const int NEUTRON_NPS_GS_LE = h_lowenergy_gs->GetEntries();
    const double NEUTRON_NORM_BERT_LE = NEUTRON_NPS_BERT_LE * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_BIC_LE = NEUTRON_NPS_BIC_LE * FLUX_SCALE_FACTOR_SB;
    const double NEUTRON_NORM_GS_LE = NEUTRON_NPS_GS_LE * FLUX_SCALE_FACTOR_GS;

    // normalizing
    h_lowenergy_bert->Scale(NEUTRON_NORM_BERT_LE/h_lowenergy_bert->Integral(),"width");
    h_lowenergy_bic->Scale(NEUTRON_NORM_BIC_LE/h_lowenergy_bic->Integral(),"width");
    h_lowenergy_gs->Scale(NEUTRON_NORM_GS_LE/h_lowenergy_gs->Integral(),"width");

    // create legend
    TLegend *l4 = new TLegend(0.78, 0.78, 0.9, 0.9);
    l4->AddEntry(h_lowenergy_bert, "Sudbury: Geant4 (BERT)", "l");
    l4->AddEntry(h_lowenergy_bic, "Sudbury: Geant4 (BIC)", "l");
    l4->AddEntry(h_lowenergy_gs, "Gran Sasso: Geant4 (BERT)", "l");
    l4->SetFillColor(kWhite);
    l4->SetHeader("Data");

    // draw
    h_lowenergy_bert->SetLineColor(kBlue);
    h_lowenergy_bert->SetLineWidth(3);
    h_lowenergy_bic->SetLineColor(kRed);
    h_lowenergy_bic->SetLineWidth(3);
    h_lowenergy_gs->SetLineColor(kViolet);
    h_lowenergy_gs->SetLineWidth(3);
    h_lowenergy_bert->GetYaxis()->SetRangeUser(10e-14, 10e-7);
    h_lowenergy_bert->Draw("HIST");
    h_lowenergy_bic->Draw("HIST SAME");
    h_lowenergy_gs->Draw("HIST SAME");
    l4->Draw();

    c4->Update();
}