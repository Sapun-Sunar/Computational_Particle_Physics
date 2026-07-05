#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "Math/Vector4D.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TStyle.h"
#include <iostream>
#include <iomanip>

// IMPORT OUR CUSTOM MATHEMATICS CONTRACT
#include "physics_math.h"

// The main macro name MUST match the filename for ACLiC compilation
void z_boson() {

    ROOT::EnableImplicitMT(); 

    const char* eos_url = "root://eospublic.cern.ch//eos/opendata/cms/derived-data/AOD2NanoAODOutreachTool/Run2012BC_DoubleMuParked_Muons.root";
    ROOT::RDataFrame df("Events", eos_url);

    auto df_filtered = df.Filter("nMuon == 2", "Strictly two muons detected")
                         .Filter("Muon_charge[0] != Muon_charge[1]", "Opposite charge");

    auto compute_invariant_mass = [](const ROOT::RVec<float>& pt, 
                                     const ROOT::RVec<float>& eta, 
                                     const ROOT::RVec<float>& phi, 
                                     const ROOT::RVec<float>& mass) {
        ROOT::Math::PtEtaPhiMVector mu1(pt[0], eta[0], phi[0], mass[0]);
        ROOT::Math::PtEtaPhiMVector mu2(pt[1], eta[1], phi[1], mass[1]);
        return (mu1 + mu2).M();
    };

    auto df_mass = df_filtered.Define("Dimuon_Mass", compute_invariant_mass, 
                                     {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});

    auto h_mass = df_mass.Histo1D({"h_mass", "Z Boson Resonance (Double-Sided CB); Invariant Mass (GeV/c^{2}); Events", 80, 70.0, 110.0}, "Dimuon_Mass");




    
    double max_events = h_mass->GetMaximum();
    int tallest_bin = h_mass->GetMaximumBin();
    double peak_x = h_mass->GetBinCenter(tallest_bin);

    std::cout << "Blind peak detected at: " << peak_x << " GeV with " << max_events << " events.\n";

    // Inject our custom 9-parameter C++ function into the ROOT framework via the Header promise
    TF1 *z_fit = new TF1("z_fit", dscb_bkg_math, 70.0, 110.0, 9);

    // 1. DYNAMIC SEEDING
    z_fit->SetParameter(0, max_events);  
    z_fit->SetParameter(1, peak_x);      
    z_fit->SetParameter(2, 2.5);         
    z_fit->SetParameter(3, 1.5);         
    z_fit->SetParameter(4, 1.5);         
    z_fit->SetParameter(5, 1.5);         
    z_fit->SetParameter(6, 1.5);         
    z_fit->SetParameter(7, 12.0);        
    z_fit->SetParameter(8, -0.03);       

    // 2. FORGING THE 9-DIMENSIONAL CHAINS
    z_fit->SetParLimits(1, peak_x - 3.0, peak_x + 3.0); 
    z_fit->SetParLimits(3, 0.1, 5.0);    
    z_fit->SetParLimits(4, 0.1, 50.0);   
    z_fit->SetParLimits(5, 0.1, 5.0);    
    z_fit->SetParLimits(6, 0.1, 50.0);   

    z_fit->SetParName(0, "Amplitude");
    z_fit->SetParName(1, "Z Rest Mass");
    z_fit->SetParName(2, "Sigma");
    z_fit->SetParName(3, "Alpha L");
    z_fit->SetParName(4, "n L");
    z_fit->SetParName(5, "Alpha R");
    z_fit->SetParName(6, "n R");
    z_fit->SetParName(7, "Bkg Const");
    z_fit->SetParName(8, "Bkg Slope");

    std::cout << "Initiating 9-dimensional optimization matrix\n";
    h_mass->Fit("z_fit", "R");

    double z_mass = z_fit->GetParameter(1);
    double z_mass_error = z_fit->GetParError(1);
    
    auto c = new TCanvas("c", "Z Boson DSCB Fit", 1000, 700);
    gStyle->SetOptStat(0);             
    gStyle->SetOptFit(1111);           
    
    h_mass->SetLineColor(kBlack);
    h_mass->SetFillColor(kAzure+1);
    h_mass->SetMarkerStyle(20);        
    h_mass->Draw("E1");                
    
    z_fit->SetLineColor(kRed);         
    z_fit->SetLineWidth(3);
    z_fit->Draw("SAME");               

    c->SaveAs("cms_zboson_dscb_fit.png");

    std::cout << "Double-Sided Math Executed.\n";
    std::cout << "Target: Z Boson (Weak Force Mediator)\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Rest Mass  : " << z_mass << " +/- " << z_mass_error << " GeV/c^2\n";
}