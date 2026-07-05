#include "physics_math.h"
#include <cmath>

// The raw piecewise mathematics for the Double-Sided Crystal Ball 
// plus the Exponential Drell-Yan background.

double dscb_bkg_math(double *x, double *par) {
    // Standardize the X-axis coordinate based on Mean and Sigma
    double t = (x[0] - par[1]) / par[2]; 
    
    // Extract tail boundaries and steepness parameters
    double alphaL = std::abs(par[3]);
    double nL     = par[4];
    double alphaR = std::abs(par[5]);
    double nR     = par[6];

    double core = 0.0;

    // LEFT TAIL: Bremsstrahlung Radiation Logic
    if (t < -alphaL) {
        double A = std::pow(nL / alphaL, nL) * std::exp(-0.5 * alphaL * alphaL);
        double B = nL / alphaL - alphaL;
        core = A * std::pow(B - t, -nL);
    } 
    // RIGHT TAIL: Magnetic Over-measurement Logic
    else if (t > alphaR) {
        double A = std::pow(nR / alphaR, nR) * std::exp(-0.5 * alphaR * alphaR);
        double B = nR / alphaR - alphaR;
        core = A * std::pow(B + t, -nR);
    } 
    // CORE: Pure Quantum Gaussian
    else {
        core = std::exp(-0.5 * t * t);
    }

    // Multiply the shape by the Amplitude Volume Knob
    double signal = par[0] * core;
    
    // Add the Exponential Background underneath it
    double background = std::exp(par[7] + par[8] * x[0]);

    return signal + background;
}

void physics_math(){
}