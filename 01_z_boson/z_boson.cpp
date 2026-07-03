#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include "Math/Vector4D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TF1.h"
#include "cstdio"
using std::printf;

void z_boson(){
    const auto *esourl = "root://eospublic.cern.ch//eos/opendata/cms/derived-data/AOD2NanoAODOutreachTool/Run2012BC_DoubleMuParked_Muons.root";
    ROOT::RDataFrame df_full("Events",esourl);
    auto df_partial = df_full.Range(50000);
    auto df_filtered = df_partial.Filter("nMuon == 2", "only two muons detected").Filter("Muon_charge[0] != Muon_charge[1]", "opposite charge (neutral parents)");

    auto compute_invariant_mass = [](const ROOT::RVec<float>& pt,
                                     const ROOT::RVec<float>& eta,
                                     const ROOT::RVec<float>& phi,
                                     const ROOT::RVec<float>& mass){
    ROOT::Math::PtEtaPhiMVector mu1(pt[0],eta[0],phi[0],mass[0]);
    ROOT::Math::PtEtaPhiMVector mu2(pt[1],eta[1],phi[1],mass[1]);
    return (mu1 + mu2).M();
    };
    
    auto df_mass = df_filtered.Define("Dimuon_mass",compute_invariant_mass,{"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass"});
    auto h_mass  = df_mass.Histo1D({"hmass","Parent Detector via Muon;Invariant Mass; Events",80,70,110},"Dimuon_mass");

    TF1 *z_fit = new TF1("z_fit", "[0]*exp(-0.5*((x-[1])/[2])^2) + exp([3] + [4]*x)", 70.0, 110.0);
    z_fit->SetParameter(0,500.0);
    z_fit->SetParameter(1,91.2);
    z_fit->SetParameter(2,2.5);
    z_fit->SetParameter(3,5.0);
    z_fit->SetParameter(4,-0.05);

    z_fit -> SetParName(0, "Signal Amplitude");
    z_fit -> SetParName(1, "Z Rest Mass");
    z_fit -> SetParName(2, "Quantum Width");
    z_fit -> SetParName(3, "Background Constant");
    z_fit -> SetParName(4, "Background Noise");

    h_mass -> Fit("z_fit", "R");

    auto z_mass = z_fit->GetParameter(1);
    auto z_mass_error = z_fit->GetParError(1);
    auto z_width = z_fit->GetParameter(2);

    auto c = new TCanvas("c","Z_Boson_Fit",1000,700);

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);

    h_mass -> SetLineColor("kBlack");
    h_mass -> SetFillColor("kAzure+1");
    h_mass -> SetMarkerStyle(20);
    h_mass -> Draw("E1");
    z_fit -> SetLineColor("kRed");
    z_fit -> SetLineWidth(3);
    z_fit -> Draw("SAME");
    
    c -> SaveAs("z_boson_fit.png");

    printf("The Rest Mass of The Boson is %.4f and The error is %.4f \nThe Width of the resolutin is %.2f GeV/c^2", z_mass, z_mass_error, z_width);
}