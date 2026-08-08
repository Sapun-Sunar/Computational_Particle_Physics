#ifndef HELICITY_MATRIX_H
#define HELICITY_MATRIX_H

#include "TLorentzVector.h"

// ========================================================
// TIER II: THE HELICITY MATRIX (QUANTUM DECAY)
// ========================================================

// Shatters the W Boson according to V-A Weak Force coupling
// Returns the final Laboratory Frame 4-vector of the generated Muon
TLorentzVector decay_w_boson(const TLorentzVector& w_boson, int w_charge);

#endif