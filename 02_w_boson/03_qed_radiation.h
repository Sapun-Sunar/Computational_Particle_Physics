#ifndef QED_RADIATION_H
#define QED_RADIATION_H

#include "TLorentzVector.h"

// ========================================================
// TIER III: QED FINAL STATE RADIATION (FSR)
// ========================================================

// Applies collinear Bremsstrahlung radiation to the muon
// Returns the degraded muon 4-vector as seen by the detector
TLorentzVector apply_qed_fsr(const TLorentzVector& muon);

#endif