#ifndef HARDWARE_RESOLUTION_H
#define HARDWARE_RESOLUTION_H

#include "TLorentzVector.h"

// ========================================================
// TIER IV: THE HARDWARE RESOLUTION MATRIX
// ========================================================

// Applies a 1.5% Gaussian smear to the muon's momentum
// Returns the final, pixelated 4-vector as seen by the CMS Tracker
TLorentzVector apply_hardware_smear(const TLorentzVector& true_muon);

#endif