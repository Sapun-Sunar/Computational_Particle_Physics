#include "01_qcd_cradle.h"
#include "LHAPDF/LHAPDF.h"
#include "TRandom3.h"
#include <cmath>
#include <iostream>
#include <algorithm>

LHAPDF::PDF* proton_grid = nullptr;
const double sqrt_s = 8000.0;

// FIX 1: Corrected spelling to match header and main core
void initialize_qcd_grid(){
    std::cout << "Loading Neural Grid Into Memory....." << std::endl;
    LHAPDF::setVerbosity(0);
    proton_grid = LHAPDF::mkPDF("NNPDF31_nnlo_as_0118", 0);
    std::cout << "Proton Structure Locked....." << std::endl;
}

TLorentzVector generate_w_boson_qcd(double target_mass){
    static thread_local TRandom3 qcd_rand(0);
    double pt_w = 0.0;
    double max_sudakov_weight = 4.11;

    while(true){
        double test_pt = qcd_rand.Uniform(0.0, 50.0);
        double test_weights = (test_pt * test_pt) * std::exp(-0.6 * test_pt);
        if (qcd_rand.Uniform(0.0, max_sudakov_weight) < test_weights){
            // FIX 2: Modified outer pt_w directly and assigned test_pt (momentum)
            pt_w = test_pt; 
            break;
        }
    }
    double phi_w = qcd_rand.Uniform(-M_PI, M_PI);

    // FIX 3: Added std::sqrt for true Transverse Mass (m_T)
    double mt_w = std::sqrt(target_mass * target_mass + pt_w * pt_w);
    
    // FIX 4: Corrected rapidity bound y_max = ln(sqrt_s / m_T)
    double y_max = std::log(sqrt_s / mt_w);
    double pz_w = 0.0;
    double energy_w = 0.0;
    
    while(true){
        double y_w = qcd_rand.Uniform(-y_max, y_max);
        
        // FIX 5: Corrected Bjorken x ratios: x = (m_T / sqrt_s) * exp(+/- y)
        double x1 = (mt_w / sqrt_s) * std::exp(y_w);
        double x2 = (mt_w / sqrt_s) * std::exp(-y_w);

        if (x1 > 1.0 || x2 > 1.0) continue;

        double u_x1 = proton_grid->xfxQ(2, x1, target_mass);
        double dbar_x2 = proton_grid->xfxQ(-1, x2, target_mass);

        double d_x1 = proton_grid->xfxQ(1, x1, target_mass);
        double ubar_x2 = proton_grid->xfxQ(-2, x2, target_mass);

        double pdf_weights = (u_x1 * dbar_x2 + d_x1 * ubar_x2);

        // PDF Rejection sampling
        if (qcd_rand.Uniform(0.0, 15.0) < pdf_weights){
            pz_w = mt_w * std::sinh(y_w);
            energy_w = mt_w * std::cosh(y_w);
            break;
        }
    }

    TLorentzVector p4_w;
    p4_w.SetPxPyPzE(
        pt_w * std::cos(phi_w),
        pt_w * std::sin(phi_w),
        pz_w,
        energy_w
    );
    return p4_w;
}