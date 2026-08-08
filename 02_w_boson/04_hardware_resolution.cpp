#include "04_hardware_resolution.h"
#include "TRandom3.h"

TLorentzVector apply_hardware_smear(const TLorentzVector& true_muon) {
    // Thread-safe random number generator
    static thread_local TRandom3 hardware_rand(0);
    
    // The CMS Run 1 (8 TeV) Muon Tracker Resolution is ~1.5% for standard pt ranges.
    // We generate a random number from a Bell Curve centered at 1.0, with a width of 0.015.
    double smear_factor = hardware_rand.Gaus(1.0, 0.015);
    
    // Clone the muon passed from Tier III (QED)
    TLorentzVector smeared_muon = true_muon;
    
    // Scale the entire 4-dimensional vector by the random hardware jitter
    smeared_muon *= smear_factor;
    
    return smeared_muon;
}