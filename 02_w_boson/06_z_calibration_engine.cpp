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

// The Physics Pillars
#include "01_qcd_cradle.h"
#include "02_helicity_matrix.h"
#include "03_qed_radiation.h"
#include "04_hardware_resolution.h"

// The 3D Parameter Space Structure
struct ScanPoint {
    double isr_sigma;
    double smear_factor;
    double fsr_scale;
    TH1F* hist;
};

int main() {
    std::cout << "[ORIAS TIER VI]: Initializing 3D Multi-Threaded Z-Boson Calibrator..." << std::endl;

    // 1. Thread Safety Protocols for CERN ROOT
    ROOT::EnableThreadSafety();
    TH1::AddDirectory(kFALSE); // Isolate memory to prevent segregation faults

    // 2. Boot the CERN Neural Network Grid
    initialize_qcd_grid();

    // 3. Define the Physics Constants (Anchored on Z Pole)
    const int events_per_universe = 1000000; // Accelerated coarse-grid scanning
    const double Z_MASS_POLE = 91.1876;
    const double Z_WIDTH = 2.4952;

    // 4. Construct the 3D Parameter Space (The Scout Matrix)
    std::vector<ScanPoint> matrix_grid;
    
    // Variable A: ISR Recoil (13.4 to 13.6, step 0.1)
    for (double isr = 13.4; isr <= 13.6; isr += 0.1) {
        // Variable B: Smearing (0.004 to 0.010, step 0.001)
        for (double smear = 0.004; smear <= 0.01; smear += 0.001) {
            // Variable C: FSR Scale (0.17 to 0.22, step 0.01)
            for (double fsr = 0.17; fsr <= 0.22; fsr += 0.01) {
                
                ScanPoint p;
                p.isr_sigma = isr;
                p.smear_factor = smear;
                p.fsr_scale = fsr;
                
                TString name = TString::Format("z_calib_isr_%.1f_smear_%.3f_fsr_%.1f", isr, smear, fsr);
                p.hist = new TH1F(name, "Z Calibration; Smeared p_{T} [GeV]; Events", 100, 20.0, 60.0);
                
                matrix_grid.push_back(p);
            }
        }
    }

    int total_universes = matrix_grid.size(); 
    std::cout << "[ORIAS]: 3D Scout Grid Constructed. Total Universes: " << total_universes << std::endl;

    // 5. Apple Silicon Core Allocation
    int total_cores = std::thread::hardware_concurrency();
    int active_cores = std::max(1, total_cores - 2); // Leave 2 cores alive
    omp_set_num_threads(active_cores);
    
    std::cout << "[ORIAS]: Commandeering " << active_cores << " of " << total_cores << " CPU Cores..." << std::endl;
    std::cout << "[ORIAS]: Forging " << (total_universes * events_per_universe) / 1000000.0 << " Million Quantum Events. Commencing calculation...\n" << std::endl;

    // 6. The Parallel OpenMP Forge
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < total_universes; ++i) {
        
        // Thread-Local Random Generator Seeded via Time and Hardware ID
        int thread_id = omp_get_thread_num();
        TRandom3 local_rand(0);
        local_rand.SetSeed(time(NULL) + thread_id * 1000 + i);

        double current_isr = matrix_grid[i].isr_sigma;
        double current_smear = matrix_grid[i].smear_factor;
        double current_fsr = matrix_grid[i].fsr_scale;

        for (int evt = 0; evt < events_per_universe; ++evt) {
            
            // Tier V Background: Generate Z Mass with Kinematic Firewall
            double z_mass;
            do {
                z_mass = local_rand.BreitWigner(Z_MASS_POLE, Z_WIDTH);
            } while (z_mass < 50.0 || z_mass > 150.0);

            // Tier I: Spawn the Z Boson
            TLorentzVector z_boson = generate_w_boson_qcd(z_mass);
            
            // ==========================================
            // HYBRID ISR RECOIL (Gaussian Core + Power Tail)
            // ==========================================
            double isr_magnitude;
            
            if (local_rand.Uniform(0.0, 1.0) < 0.95) {
                isr_magnitude = TMath::Abs(local_rand.Gaus(0.0, current_isr));
            } else {
                isr_magnitude = current_isr + local_rand.Exp(current_isr * 1.5); 
            }

            double isr_phi = local_rand.Uniform(-TMath::Pi(), TMath::Pi());
            double isr_px = isr_magnitude * TMath::Cos(isr_phi);
            double isr_py = isr_magnitude * TMath::Sin(isr_phi);

            TVector3 recoil_boost(isr_px / z_boson.E(), isr_py / z_boson.E(), 0.0);
            z_boson.Boost(recoil_boost);
            // ==========================================

            // Tier II: Decay Z -> Muon + Muon
            TLorentzVector true_muon = decay_w_boson(z_boson, 1); 

            // Tier III: Baseline QED Collinear Radiation
            TLorentzVector qed_muon = apply_qed_fsr(true_muon);

            // TIER III INJECTION: The Dynamic FSR Scaler
            TLorentzVector radiated_photon = true_muon - qed_muon; 
            radiated_photon *= current_fsr;                        
            TLorentzVector scaled_qed_muon = true_muon - radiated_photon; 

            // Tier IV: Hardware Smearing
            double true_pt = scaled_qed_muon.Pt();
            double pt_error = true_pt * current_smear;
            double smeared_pt = local_rand.Gaus(true_pt, pt_error);
            
            TLorentzVector cms_muon;
            cms_muon.SetPtEtaPhiM(smeared_pt, scaled_qed_muon.Eta(), scaled_qed_muon.Phi(), 0.10566);

            matrix_grid[i].hist->Fill(cms_muon.Pt());
        }
        
        #pragma omp critical 
        {
            std::cout << "[THREAD " << thread_id << "]: Secured Universe " << i + 1 << " / " << total_universes << std::endl;
        }
    }

    // 7. Master Thread Re-synchronization
    std::cout << "\n[ORIAS]: All timelines converged. Writing matrix to digital vault..." << std::endl;
    TFile* output_file = new TFile("orias_z_calibration_matrix.root", "RECREATE");
    
    for (int i = 0; i < total_universes; ++i) {
        matrix_grid[i].hist->Write();
        delete matrix_grid[i].hist; 
    }

    output_file->Close();
    delete output_file;

    std::cout << "[ORIAS]: Phase I Complete. The Control Region Matrix is locked." << std::endl;
    return 0;
}