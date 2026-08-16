#include <iostream>
#include <vector>
#include <thread>
#include <omp.h>
#include <ctime>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TString.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TVector3.h"

// The Four Physics Pillars
#include "01_qcd_cradle.h"
#include "02_helicity_matrix.h"
#include "03_qed_radiation.h"
#include "04_hardware_resolution.h"

struct MatrixPoint {
    double target_mass;
    double target_isr;
    TH1F* hist;
};

int main() {
    std::cout << "[ORIAS TIER VIII]: Initializing 2D W-Boson Master Matrix Forge..." << std::endl;

    ROOT::EnableThreadSafety();
    TH1::AddDirectory(kFALSE);

    initialize_qcd_grid();

    // ==========================================
    // THE FROZEN CALIBRATION CONSTANTS
    // ==========================================
    const double FROZEN_SMEAR = 0.006;   
    const double FROZEN_FSR = 0.2;       

    // The Upgraded Background Constants
    const double P_Z_BOSON = 0.05;  
    const double P_QCD_FAKE = 0.015; 
    const double P_W_TAU = 0.11;    
    const double Z_MASS_POLE = 91.1876;
    const double Z_WIDTH = 2.4952;
    
    const int events_per_universe = 1000000; 

    std::vector<MatrixPoint> matrix_grid;
    
    // 3. The 2D Scan: Mass (80.000 to 80.501) x ISR (16.0 to 22.0)
    for (double mass = 80.300; mass <= 80.451; mass += 0.002) {
        for (double isr = 19.0; isr <= 20.1; isr += 0.2) {
            MatrixPoint p;
            p.target_mass = mass;
            p.target_isr = isr;
            
            // Format: muon_pt_m80379_isr205
            TString name = TString::Format("muon_pt_m%d_isr%d", (int)(mass * 1000), (int)(isr * 10)); 
            p.hist = new TH1F(name, "CMS Smeared Muon p_{T}; p_{T} [GeV]; Events", 100, 20.0, 60.0);
            matrix_grid.push_back(p);
        }
    }

    int total_universes = matrix_grid.size();
    int total_cores = std::thread::hardware_concurrency();
    int active_cores = std::max(1, total_cores - 1);
    omp_set_num_threads(active_cores);
    
    std::cout << "[ORIAS]: 2D Grid Established. Total Universes: " << total_universes << std::endl;
    std::cout << "[ORIAS]: Commandeering " << active_cores << " Cores to forge the matrix..." << std::endl;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < total_universes; ++i) {
        
        int thread_id = omp_get_thread_num();
        TRandom3 local_rand(0);
        local_rand.SetSeed(time(NULL) + thread_id * 1000 + i);

        double current_mass = matrix_grid[i].target_mass;
        double current_isr = matrix_grid[i].target_isr;

        for (int evt = 0; evt < events_per_universe; ++evt) {
            
            TLorentzVector true_muon;
            int boson_charge = (local_rand.Uniform(0.0, 1.0) > 0.5) ? 1 : -1;

            // Apply DYNAMIC Hybrid ISR Recoil
            double isr_magnitude;
            if (local_rand.Uniform(0.0, 1.0) < 0.95) {
                isr_magnitude = TMath::Abs(local_rand.Gaus(0.0, current_isr));
            } else {
                isr_magnitude = current_isr + local_rand.Exp(current_isr * 1.5); 
            }
            double isr_phi = local_rand.Uniform(-TMath::Pi(), TMath::Pi());
            double isr_px = isr_magnitude * TMath::Cos(isr_phi);
            double isr_py = isr_magnitude * TMath::Sin(isr_phi);

            double quantum_dice = local_rand.Uniform(0.0, 1.0);

            if (quantum_dice < P_Z_BOSON) {
                double z_mass;
                do { z_mass = local_rand.BreitWigner(Z_MASS_POLE, Z_WIDTH); } while (z_mass < 50.0 || z_mass > 150.0);
                TLorentzVector z_boson = generate_w_boson_qcd(z_mass);
                TVector3 recoil_boost(isr_px / z_boson.E(), isr_py / z_boson.E(), 0.0);
                z_boson.Boost(recoil_boost);
                true_muon = decay_w_boson(z_boson, 1); 

            } else if (quantum_dice < P_Z_BOSON + P_QCD_FAKE) {
                double fake_pt = 20.0 + local_rand.Exp(6.0); 
                double fake_eta = local_rand.Uniform(-2.4, 2.4);
                double fake_phi = local_rand.Uniform(-TMath::Pi(), TMath::Pi());
                true_muon.SetPtEtaPhiM(fake_pt, fake_eta, fake_phi, 0.10566);

            } else {
                TLorentzVector w_boson = generate_w_boson_qcd(current_mass);
                TVector3 recoil_boost(isr_px / w_boson.E(), isr_py / w_boson.E(), 0.0);
                w_boson.Boost(recoil_boost);

                if (local_rand.Uniform(0.0, 1.0) < P_W_TAU) {
                    TLorentzVector tau_lepton = decay_w_boson(w_boson, boson_charge);
                    true_muon = tau_lepton * local_rand.Uniform(0.2, 0.6); 
                } else {
                    true_muon = decay_w_boson(w_boson, boson_charge);
                }
            }

            TLorentzVector qed_muon = apply_qed_fsr(true_muon);
            TLorentzVector radiated_photon = true_muon - qed_muon; 
            radiated_photon *= FROZEN_FSR;                        
            TLorentzVector scaled_qed_muon = true_muon - radiated_photon; 

            double true_pt = scaled_qed_muon.Pt();
            double pt_error = true_pt * FROZEN_SMEAR;
            double smeared_pt = local_rand.Gaus(true_pt, pt_error);
            
            TLorentzVector cms_muon;
            cms_muon.SetPtEtaPhiM(smeared_pt, scaled_qed_muon.Eta(), scaled_qed_muon.Phi(), 0.10566);

            if (cms_muon.Pt() < 24.5) continue; 
            if (TMath::Abs(cms_muon.Eta()) >= 2.1) continue;

            matrix_grid[i].hist->Fill(cms_muon.Pt());
        }
        
        #pragma omp critical 
        {
            if (i % 100 == 0) {
                std::cout << "[THREAD " << thread_id << "]: Secured Universe " << i << " / " << total_universes << std::endl;
            }
        }
    }

    std::cout << "\n[ORIAS]: All timelines converged. Writing 2D master measurement matrix..." << std::endl;
    TFile* output_file = new TFile("orias_w_measurement_matrix.root", "RECREATE");
    for (int i = 0; i < total_universes; ++i) {
        matrix_grid[i].hist->Write();
        delete matrix_grid[i].hist;
    }
    output_file->Close();
    delete output_file;

    std::cout << "[ORIAS]: Phase II Complete. The 2D Master Matrix is locked." << std::endl;
    return 0;
}