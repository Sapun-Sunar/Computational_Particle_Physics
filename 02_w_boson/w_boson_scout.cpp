#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include <cmath>
#include <iostream>

// Execute via terminal: root -l -b -q "w_boson_scout.cpp+"

void w_boson_scout() {
    ROOT::EnableImplicitMT(); 
    
    std::cout << "[JARVIS]: Initializing Base Solid-State Data Stream..." << std::endl;
    ROOT::RDataFrame df("Events", "Run2012B_SingleMu.root");

    // FIX 1: The Dual Hardware Machete (Muon AND MET > 25.0)
    auto df_filtered = df.Filter("nMuon == 1", "Single Muon")
                         .Filter("Muon_pt[0] > 25.0", "Muon Kinetic Kick")
                         .Filter("MET_pt > 25.0", "MET Kinetic Kick");

    auto calc_MT = [](const ROOT::RVec<float>& muon_pt, const ROOT::RVec<float>& muon_phi, float met_pt, float met_phi) {
        float dphi = muon_phi[0] - met_phi;
        
        while (dphi > M_PI)  dphi -= 2.0 * M_PI;
        while (dphi < -M_PI) dphi += 2.0 * M_PI;
        
        return std::sqrt(2.0 * muon_pt[0] * met_pt * (1.0 - std::cos(dphi)));
    };

    auto df_mt = df_filtered.Define("Transverse_Mass", calc_MT, {"Muon_pt", "Muon_phi", "MET_pt", "MET_phi"});

    // FIX 2: Strict 1-to-1 Geometric Alignment (40.0 to 140.0, 100 Bins = 1.0 GeV Width)
    auto h_MT = df_mt.Histo1D({"W_Boson_MT", "W Boson Transverse Mass;M_{T} [GeV];Events / 1.0 GeV", 100, 40.0, 140.0}, "Transverse_Mass");

    TCanvas c1("c1", "W Boson Baseline", 800, 600);
    c1.SetGrid();
    h_MT->SetLineColor(kRed+2);
    h_MT->SetLineWidth(2);
    h_MT->SetFillColorAlpha(kRed-9, 0.4);
    h_MT->Draw("HIST");
    
    c1.SaveAs("baseline_w_boson.png");
    
    // FIX 3: The Data Bridge (Exporting to the Oracle)
    TFile out_file("w_boson_data.root", "RECREATE");
    h_MT->Write();
    out_file.Close();
    
    std::cout << "[JARVIS]: Baseline rendering complete. Telemetry exported to w_boson_data.root for the Oracle." << std::endl;
}