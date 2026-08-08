// Named after Orias - My Local AI!

#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TRandom3.h"

// The Four Pillars of the Architecture
#include "01_qcd_cradle.h"
#include "02_helicity_matrix.h"
#include "03_qed_radiation.h"
#include "04_hardware_resolution.h"

int main() {
    std::cout << "[ORIAS]: Initializing Theoretical Quantum Engine..." << std::endl;

    // 1. Boot the CERN Neural Network Grid
    initialize_qcd_grid();

    // 2. Prepare the ROOT output file to store our synthetic data
    TFile* output_file = new TFile("orias_synthetic_data.root", "RECREATE");
    
    // We will track the final Transverse Momentum (pT) of the CMS Muon
    TH1F* hist_muon_pt = new TH1F("muon_pt", "CMS Smeared Muon Transverse Momentum; p_{T} [GeV]; Events", 100, 20.0, 60.0);

    // Standard Model W Boson Mass (GeV)
    const double target_w_mass = 80.379; 
    const int total_events = 3000000; // 3,000,000 collisions for the drun

    TRandom3 master_rand(0);

    std::cout << "[ORIAS]: Commencing 8 TeV Proton Collisions..." << std::endl;

    for (int i = 0; i < total_events; ++i) {
        
        // Randomly decide if we are generating a W+ (1) or W- (-1)
        int w_charge = (master_rand.Uniform(0.0, 1.0) > 0.5) ? 1 : -1;

        // TIER I: Spawn the W Boson from the Quark Sea
        TLorentzVector w_boson = generate_w_boson_qcd(target_w_mass);

        // TIER II: Shatter the W Boson using Weak Force V-A Coupling
        TLorentzVector true_muon = decay_w_boson(w_boson, w_charge);

        // TIER III: Bleed the kinetic energy via QED Collinear Radiation
        TLorentzVector qed_muon = apply_qed_fsr(true_muon);

        // TIER IV: Blind the measurement with CMS Hardware Resolution
        TLorentzVector cms_muon = apply_hardware_smear(qed_muon);

        // Log the final, degraded muon into the ROOT histogram
        hist_muon_pt->Fill(cms_muon.Pt());

        if (i % 100000 == 0 && i > 0) {
            std::cout << "[ORIAS]: " << i << " quantum events processed..." << std::endl;
        }
    }

    std::cout << "[ORIAS]: Simulation Complete. Writing to disk." << std::endl;

    // Save and close
    hist_muon_pt->Write();
    output_file->Close();
    delete output_file;

    std::cout << "[ORIAS]: Data secured in orias_synthetic_data.root." << std::endl;
    return 0;
}