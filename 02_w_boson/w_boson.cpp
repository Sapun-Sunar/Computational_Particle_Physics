#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "TCanvas.h"
#include "TH1D.h"
#include "TF1.h"
#include <cmath>
#include <iostream>

// To execute, run in your terminal: root -l -b -q "w_boson_hunt.cpp+"

void w_boson() {
    // Ignite all available CPU cores for maximum parallel velocity
    ROOT::EnableImplicitMT(); 
    
    std::cout << "[JARVIS]: Initializing CMS 2012 SingleMuon Data Stream..." << std::endl;
    // The Target Data (Updated Benchmark Path)
    ROOT::RDataFrame df("Events", "/Users/sapunsunar/Documents/CODES2/Run2012B_SingleMu.root");

    std::cout << "[JARVIS]: Applying Quantum Isolation Filters..." << std::endl;
    // The Isolation Filter (Purges the 6-muon events you saw)
    auto df_filtered = df.Filter("nMuon == 1", "Strict Single Muon Signature")
                         .Filter("Muon_pt[0] > 25.0", "High-Energy Kinetic Kick");

    // The Mathematical Engine (Upgraded for RVec Array Handling)
    auto calc_MT = [](const ROOT::RVec<float>& muon_pt, const ROOT::RVec<float>& muon_phi, float met_pt, float met_phi) {
        
        // Extract the solitary muon from the array box
        float pt = muon_pt[0];
        float phi = muon_phi[0];
        
        // Calculate the raw azimuthal angle difference
        float dphi = phi - met_phi;
        
        // The Geometry Trap: Force the angle into the [-pi, pi] boundary
        while (dphi > M_PI)  dphi -= 2.0 * M_PI;
        while (dphi < -M_PI) dphi += 2.0 * M_PI;
        
        // The 2D Minkowski Transverse Projection
        return std::sqrt(2.0 * pt * met_pt * (1.0 - std::cos(dphi)));
    };

    std::cout << "[JARVIS]: Compiling Transverse Mass Lambda Engine..." << std::endl;
    auto df_mt = df_filtered.Define("Transverse_Mass", calc_MT, {"Muon_pt", "Muon_phi", "MET_pt", "MET_phi"});

    std::cout << "[JARVIS]: Projecting Shadows onto the 2D Jacobian Canvas..." << std::endl;
    // The Canvas (Strict Integer Binning Applied)
    auto h_MT = df_mt.Histo1D({"W_Boson_MT", "W Boson Transverse Mass (Jacobian Cliff);M_{T} [GeV];Events", 100, 40.0, 140.0}, "Transverse_Mass");


    // Rendering and Telemetry
    TCanvas c1("c1", "W Boson Jacobian Cliff", 800, 600);
    c1.SetGrid();
    
    // Aesthetic mapping for high-energy physics
    h_MT->SetLineColor(kRed+2);
    h_MT->SetLineWidth(2);
    h_MT->SetFillColorAlpha(kRed-9, 0.4);


    TF1 *fitFunc = new TF1("fitFunc","gaus", 60.0, 95.0);

    fitFunc -> SetLineColor(kBlue);
    fitFunc -> SetLineWidth(3);
    fitFunc -> SetLineStyle(2);
    
    h_MT -> Fit("fitFunc", "R");
    h_MT->Draw("HIST");
    fitFunc -> Draw("SAME");

    c1.SaveAs("jacobian_cliff_w_boson.png");
    
    std::cout << "[JARVIS]: Extraction Complete. The Ghost has been mapped." << std::endl;
}