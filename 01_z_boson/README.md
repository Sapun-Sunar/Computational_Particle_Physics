# Module: Z Boson Resonance Extraction

## Physical Context

The $Z$ Boson is the electrically neutral force carrier of the Weak Nuclear Force, responsible for fundamental interactions such as solar fusion and radioactive decay. Due to the massive energy required to mediate force over subatomic distances (governed by Heisenberg's Uncertainty Principle, $\Delta E \Delta t \ge \frac{\hbar}{2}$), the $Z$ Boson is incredibly heavy and highly unstable, decaying in approximately $10^{-25}$ seconds.

This module isolates the $Z \rightarrow \mu^+ \mu^-$ decay channel.

## Mathematical Architecture

Initial extractions using symmetrical Gaussian fits resulted in a mass deficit (e.g., 90.71 GeV vs the true 91.18 GeV). This occurs due to Bremsstrahlung Radiation. As muons traverse the detector material, they lose energy, creating an asymmetrical tail on the lower-mass side of the data peak.

To strictly account for this physical reality, this script abandons the standard Gaussian and deploys a 7-Parameter Convolution:

* The Signal: A Crystal Ball Function (crystalball), which perfectly stitches a Gaussian core to a power-law tail defined by parameters $\alpha$ and $n$, recapturing the lost radiation energy.

* The Background: An Exponential decay function (expo) to model the Drell-Yan quantum noise.

## Computational Scale

This module utilizes ROOT Implicit Multi-Threading (IMT). By bypassing standard batch limits, it parallelizes the relativistic tensor algebra across all available CPU cores, processing the entire CMS Open Data dataset for maximum statistical precision.

## Execution

Run the compiled macro via the ROOT framework:
```bash
root -l -b -q z_boson.cpp
```