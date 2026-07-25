#include "TFile.h"
#include "TH1D.h"
#include "TRandom3.h"
#include "ROOT/TThreadExecutor.hxx" 
#include "TStyle.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

// Execute via terminal: root -l -b -q "mc_simulator.cpp+"

struct TemplateResult {
    double mass;
    double chi2;
    std::vector<double> bin_contents;
};

bool compareChi2(const TemplateResult& a, const TemplateResult& b) {
    return a.chi2 < b.chi2;
}

void mc_simulator() {
    ROOT::EnableImplicitMT();
    
    TFile* file = TFile::Open("w_boson_data.root");
    if (!file || file->IsZombie()) {
        std::cerr << "\n[FATAL ERROR]: w_boson_data.root missing. Ensure real CMS data is exported via the scout." << std::endl;
        return;
    }
    
    TH1D* h_data = (TH1D*)file->Get("W_Boson_MT");
    double max_data = h_data->GetMaximum(); 
    
    double anchor_mass = 80.377; 
    
    std::vector<double> trial_masses;
    for (double m = anchor_mass - 2.0; m <= anchor_mass + 2.01; m += 0.5) {
        trial_masses.push_back(m);
    }
    
    std::cout << "[JARVIS]: Igniting the Dual-Engine Drell-Yan Oracle..." << std::endl;
    std::cout << "[JARVIS]: Asymmetric Fit Window Re-Engaged: 65.0 - 100.0 GeV." << std::endl;

    auto simulate_universe = [&](double trial_mass) -> TemplateResult {
        TRandom3 local_rand(42 + int(trial_mass * 1000));
        
        TH1D h_template(Form("h_%.3f", trial_mass), "Temp", 100, 40.0, 140.0);
        h_template.SetDirectory(nullptr); 
        
        // WEAPONIZED CHAOS: Randomized Volumes and Ratios
        int total_W = 36000000 + local_rand.Integer(4000001); 
        double z_ratio = 9.0 + local_rand.Uniform(2.0);       
        int total_Z = total_W / z_ratio;
        
        // ========================================================
        // ENGINE 1: THE W BOSON SIGNAL
        // ========================================================
        for (int i = 0; i < total_W; ++i) {
            double true_m = local_rand.BreitWigner(trial_mass, 2.1);
            double pt_W = local_rand.Exp(22.0); 
            double phi_W = local_rand.Uniform(-M_PI, M_PI);
            
            double cos_theta = local_rand.Uniform(-1.0, 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
            double pt_mu_rest = (true_m / 2.0) * sin_theta;
            double phi_mu_rest = local_rand.Uniform(-M_PI, M_PI);
            
            double px_mu = pt_mu_rest * std::cos(phi_mu_rest) + 0.5 * pt_W * std::cos(phi_W);
            double py_mu = pt_mu_rest * std::sin(phi_mu_rest) + 0.5 * pt_W * std::sin(phi_W);
            double pt_mu_lab = std::sqrt(px_mu*px_mu + py_mu*py_mu);
            
            double px_nu = -pt_mu_rest * std::cos(phi_mu_rest) + 0.5 * pt_W * std::cos(phi_W);
            double py_nu = -pt_mu_rest * std::sin(phi_mu_rest) + 0.5 * pt_W * std::sin(phi_W);
            double pt_nu_lab = std::sqrt(px_nu*px_nu + py_nu*py_nu);
            
            // DYNAMIC HARDWARE RESOLUTION
            double pt_nu_safe = std::max(pt_nu_lab, 1.0); 
            double met_res = std::sqrt( std::pow(0.10 / std::sqrt(pt_nu_safe / 30.0), 2) + std::pow(0.10, 2) );
            
            double smeared_mu = local_rand.Gaus(pt_mu_lab, 0.02 * pt_mu_lab);  
            double smeared_met = local_rand.Gaus(pt_nu_lab, met_res * pt_nu_lab); 
            
            if (smeared_mu < 25.0 || smeared_met < 25.0) continue; 
            
            double phi_mu_lab = std::atan2(py_mu, px_mu);
            double phi_nu_lab = std::atan2(py_nu, px_nu);
            double dphi = phi_mu_lab - phi_nu_lab;
            
            while (dphi > M_PI)  dphi -= 2.0 * M_PI;
            while (dphi < -M_PI) dphi += 2.0 * M_PI;
            
            double mt_sim = std::sqrt(2.0 * smeared_mu * smeared_met * (1.0 - std::cos(dphi)));
            h_template.Fill(mt_sim);
        }
        
        // ========================================================
        // ENGINE 2: THE DRELL-YAN GHOST (Z BOSON)
        // ========================================================
        for (int i = 0; i < total_Z; ++i) {
            double true_m = local_rand.BreitWigner(91.187, 2.495); 
            double pt_Z = local_rand.Exp(22.0); 
            double phi_Z = local_rand.Uniform(-M_PI, M_PI);
            
            double cos_theta = local_rand.Uniform(-1.0, 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
            double pt_mu_rest = (true_m / 2.0) * sin_theta;
            double phi_mu_rest = local_rand.Uniform(-M_PI, M_PI);
            
            double px_mu1 = pt_mu_rest * std::cos(phi_mu_rest) + 0.5 * pt_Z * std::cos(phi_Z);
            double py_mu1 = pt_mu_rest * std::sin(phi_mu_rest) + 0.5 * pt_Z * std::sin(phi_Z);
            double pt_mu1_lab = std::sqrt(px_mu1*px_mu1 + py_mu1*py_mu1);
            
            double px_mu2 = -pt_mu_rest * std::cos(phi_mu_rest) + 0.5 * pt_Z * std::cos(phi_Z);
            double py_mu2 = -pt_mu_rest * std::sin(phi_mu_rest) + 0.5 * pt_Z * std::sin(phi_Z);
            double pt_mu2_lab = std::sqrt(px_mu2*px_mu2 + py_mu2*py_mu2);
            
            // GEOMETRY OF THE BLIND SPOT
            double eta_mu2 = local_rand.Gaus(0, 1.5); 
            double abs_eta = std::abs(eta_mu2);
            
            bool is_lost = false;
            if (abs_eta > 2.4) {
                is_lost = true; 
            } else {
                double prob_lost = 0.01 + 0.09 * ((abs_eta * abs_eta) / (2.4 * 2.4));
                if (local_rand.Uniform(1.0) < prob_lost) {
                    is_lost = true;
                }
            }
            
            if (is_lost) {
                double pt_mu2_safe = std::max(pt_mu2_lab, 1.0);
                double met_res = std::sqrt( std::pow(0.10 / std::sqrt(pt_mu2_safe / 30.0), 2) + std::pow(0.10, 2) );
                
                double smeared_mu = local_rand.Gaus(pt_mu1_lab, 0.02 * pt_mu1_lab);  
                double smeared_met = local_rand.Gaus(pt_mu2_lab, met_res * pt_mu2_lab); 
                
                if (smeared_mu < 25.0 || smeared_met < 25.0) continue; 
                
                double phi_mu_lab = std::atan2(py_mu1, px_mu1);
                double phi_nu_lab = std::atan2(py_mu2, px_mu2); 
                double dphi = phi_mu_lab - phi_nu_lab;
                
                while (dphi > M_PI)  dphi -= 2.0 * M_PI;
                while (dphi < -M_PI) dphi += 2.0 * M_PI;
                
                double mt_sim = std::sqrt(2.0 * smeared_mu * smeared_met * (1.0 - std::cos(dphi)));
                h_template.Fill(mt_sim);
            }
        }
        
        // ========================================================
        // CALIBRATION & EVALUATION
        // ========================================================
        double max_sim = h_template.GetMaximum();
        if (max_sim > 0) {
            h_template.Scale(max_data / max_sim);
        }
        
        double chi2 = 0.0;
        std::vector<double> contents(100, 0.0);
        
        // THE FIX: Asymmetric Fit Window Re-Engaged
        int bin_65 = h_template.FindBin(65.0);
        int bin_100 = h_template.FindBin(100.0);
        
        for (int b = 1; b <= 100; ++b) {
            contents[b-1] = h_template.GetBinContent(b);
            double obs = h_data->GetBinContent(b);
            double exp = contents[b-1];
            
            // Safe Evaluation Window: 65.0 to 100.0 GeV
            if (b >= bin_65 && b <= bin_100 && exp > 0) {
                chi2 += (obs - exp) * (obs - exp) / exp;
            }
        }
        
        return {trial_mass, chi2, contents};
    };

    ROOT::TThreadExecutor pool;
    auto results_root = pool.Map(simulate_universe, trial_masses); 
    std::vector<TemplateResult> results(results_root.begin(), results_root.end());
    std::sort(results.begin(), results.end(), compareChi2);
    
    std::cout << "\n========================================================" << std::endl;
    std::cout << "[JARVIS]: ORACLE HAS LOCKED THE TRUE MASS AT " << results[0].mass << " GeV." << std::endl;
    std::cout << "========================================================\n" << std::endl;
    
    std::ofstream out_csv("telemetry_data.csv");
    out_csv << "Mass,Data,Top1_" << results[0].mass 
            << ",Top2_" << results[1].mass 
            << ",Top3_" << results[2].mass << "\n";
            
    for (int b = 1; b <= 100; ++b) {
        out_csv << h_data->GetBinCenter(b) << "," 
                << h_data->GetBinContent(b) << ","
                << results[0].bin_contents[b-1] << ","
                << results[1].bin_contents[b-1] << ","
                << results[2].bin_contents[b-1] << "\n";
    }
    out_csv.close();
    file->Close();
}