#ifndef QCD_CRADLE_H
#define QCD_CRADLE_H

#include "TLorentzVector.h"

void initialize_qcd_grid();
TLorentzVector generate_w_boson_qcd(double target_mass);

#endif