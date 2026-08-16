#include "03_qed_radiation.h"
#include "TRandom3.h"
#include <cmath>

TLorentzVector apply_qed_fsr(const TLorentzVector& muon) {
    // Thread-safe random number generator
    static thread_local TRandom3 qed_rand(0);
    
    // The Infrared Cutoff Boundaries
    double z_min = 0.85;  // We cap the max energy loss at 15% for a single hard emission
    double z_max = 0.999; // The mathematical shield against the Infrared Catastrophe
    
    double z_kept = 1.0;  // Default: Muon keeps 100% of its energy
    
    // Quantum Probability: Not every muon radiates a hard photon.
    // We estimate roughly 20% of muons undergo significant, measurable FSR.
    if (qed_rand.Uniform(0.0, 1.0) < 0.20) {
        
        // Calculate the maximum possible height of the probability curve for our rejection box
        double max_weight = (1.0 + z_max * z_max) / (1.0 - z_max); 
        
        while (true) {
            double test_z = qed_rand.Uniform(z_min, z_max);
            double test_weight = (1.0 + test_z * test_z) / (1.0 - test_z);
            
            // Hit-or-Miss against the Altarelli-Parisi splitting function
            if (qed_rand.Uniform(0.0, max_weight) < test_weight) {
                z_kept = test_z;
                break;
            }
        }
    }
    
    // ---------------------------------------------------------
    // COLLINEAR VECTOR SCALING
    // ---------------------------------------------------------
    // We clone the original pristine muon passed by reference...
    TLorentzVector degraded_muon = muon;
    
    // ...and we multiply its entire 4-dimensional momentum by the energy fraction it kept.
    // The lost energy is carried away by an invisible photon traveling in the exact same direction.
    degraded_muon *= z_kept; 
    
    return degraded_muon;
}