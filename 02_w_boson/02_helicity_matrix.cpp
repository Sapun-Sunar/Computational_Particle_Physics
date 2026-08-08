#include "02_helicity_matrix.h"
#include "TRandom3.h"
#include <cmath>
#include <iostream>

TLorentzVector decay_w_boson(const TLorentzVector& w_boson, int w_charge) {
    // Thread-safe random number generator
    static thread_local TRandom3 decay_rand(0); 
    


    // ---------------------------------------------------------
    // STEP 1: THE W BOSON REST FRAME KINEMATICS
    // ---------------------------------------------------------
    // In its own rest frame, the W Boson is stationary. 
    // It splits its mass perfectly equally between the muon and neutrino.
    // (We assume the muon mass ~0.105 GeV is negligible compared to 80 GeV).
    double p_muon = w_boson.M() / 2.0; 
    


    // ---------------------------------------------------------
    // STEP 2: THE V-A HELICITY ANGULAR FILTER
    // ---------------------------------------------------------
    double cos_theta = 0.0;
    double max_weight = 4.0; // The maximum value of (1 +/- cos(theta))^2 is always 4.
    
    while (true) {
        // Guess a random angle between straight forward (1) and straight backward (-1)
        double test_cos = decay_rand.Uniform(-1.0, 1.0);
        double test_weight = 0.0;
        
        // Apply the Weak Force Parity Violation Matrix
        if (w_charge > 0) {
            // W+ Boson: Prefers to fire the antimuon forward
            test_weight = std::pow((1.0 + test_cos), 2);
        } else {
            // W- Boson: Prefers to fire the muon backward
            test_weight = std::pow((1.0 - test_cos), 2);
        }
        
        // Monte Carlo Hit-or-Miss against the probability curve
        if (decay_rand.Uniform(0.0, max_weight) < test_weight) {
            cos_theta = test_cos;
            break;
        }
    }
    
    // Calculate sine from cosine: sin^2(x) + cos^2(x) = 1
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
    
    // The azimuthal angle (rotation around the axis) remains uniform
    double phi = decay_rand.Uniform(-M_PI, M_PI);
    



    // ---------------------------------------------------------
    // STEP 3: BUILD MUON IN REST FRAME
    // ---------------------------------------------------------
    TLorentzVector muon_rest_frame;
    muon_rest_frame.SetPxPyPzE(
        p_muon * sin_theta * std::cos(phi), // Px
        p_muon * sin_theta * std::sin(phi), // Py
        p_muon * cos_theta,                 // Pz
        p_muon                              // Energy (E = p for massless approximation)
    );
    


    // ---------------------------------------------------------
    // STEP 4: LORENTZ BOOST TO LABORATORY FRAME
    // ---------------------------------------------------------
    // The muon vector is currently trapped in the paused timeline of the W Boson.
    // We must apply Einstein's Special Relativity to boost it back to the CMS cavern.
    
    TLorentzVector final_muon_lab = muon_rest_frame;
    final_muon_lab.Boost(w_boson.BoostVector());
    
    // The CMS detector only sees the muon. The neutrino is invisible to us.
    return final_muon_lab;
}